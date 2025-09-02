#include "medical_orders_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

static QString yuan(double v) {
    return QString::number(v, 'f', 2);
}

MedicalOrdersDialog::MedicalOrdersDialog(int patientId,
                                         const QString &patientName,
                                         QWidget *parent)
    : QDialog(parent),
      patientId_(patientId),
      patientName_(patientName)
{
    setWindowTitle("医嘱管理");
    resize(1000, 640);
    buildUi();
    applyStyles();
}

MedicalOrdersDialog::MedicalOrdersDialog(int patientId,
                                         int orderId,
                                         const QString &patientName,
                                         QWidget *parent)
    : MedicalOrdersDialog(patientId, patientName, parent)
{
    orderId_ = orderId;
    if (lblOrderId_) lblOrderId_->setText(orderId_ > 0
        ? QString("医嘱单：#%1").arg(orderId_)
        : QString("医嘱单：新建"));
}

void MedicalOrdersDialog::buildUi()
{
    auto *main = new QVBoxLayout(this);
    main->setContentsMargins(18,18,18,18);
    main->setSpacing(12);

    // 顶部区域：基本信息
    {
        auto *top = new QGridLayout();
        top->setHorizontalSpacing(12);
        top->setVerticalSpacing(8);

        lblPatient_ = new QLabel(QString("患者：%1（ID:%2）").arg(patientName_).arg(patientId_));
        lblOrderId_ = new QLabel("医嘱单：新建");

        edtDept_   = new QLineEdit(); edtDept_->setPlaceholderText("科室（可选）");
        edtDoctor_ = new QLineEdit(); edtDoctor_->setPlaceholderText("开立医生（可选）");
        edtNote_   = new QLineEdit(); edtNote_->setPlaceholderText("单据备注（可选）");

        int r=0;
        top->addWidget(lblPatient_, r,0,1,2);
        top->addWidget(lblOrderId_, r,2,1,2); r++;
        top->addWidget(new QLabel("科室"),  r,0); top->addWidget(edtDept_,   r,1);
        top->addWidget(new QLabel("医生"),  r,2); top->addWidget(edtDoctor_, r,3); r++;
        top->addWidget(new QLabel("备注"),  r,0); top->addWidget(edtNote_,   r,1,1,3);

        main->addLayout(top);
    }

    // 表格
    {
        tbl_ = new QTableWidget();
        tbl_->setObjectName("ordersItemsTable");
        tbl_->setColumnCount(8);
        tbl_->setHorizontalHeaderLabels(QStringList()
            << "项目/内容"  // 0: item_name/content
            << "剂量"      // 1: dose
            << "途径"      // 2: route
            << "频次"      // 3: frequency
            << "天数"      // 4: days
            << "数量"      // 5: qty
            << "金额"      // 6: amount(单价*数量 或直接金额)
            << "备注");    // 7: note
        tbl_->horizontalHeader()->setStretchLastSection(true);
        tbl_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tbl_->verticalHeader()->setVisible(false);
        tbl_->setSelectionBehavior(QAbstractItemView::SelectRows);
        tbl_->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl_->setEditTriggers(QAbstractItemView::AllEditTriggers);

        main->addWidget(tbl_, 1);

        // 任一可影响金额/合计的编辑变动 → 重算
        connect(tbl_, &QTableWidget::itemChanged, this, [this](QTableWidgetItem *){
            recalcTotal();
        });
    }

    // 底栏：合计 + 按钮
    {
        auto *bar = new QHBoxLayout();
        bar->addStretch();
        lblTotal_   = new QLabel("合计：¥0.00");
        btnAdd_     = new QPushButton("新增行");
        btnRemove_  = new QPushButton("删除行");
        btnRefresh_ = new QPushButton("刷新");
        btnSave_    = new QPushButton("保存");
        btnClose_   = new QPushButton("关闭");

        bar->addWidget(lblTotal_);
        bar->addSpacing(16);
        bar->addWidget(btnAdd_);
        bar->addWidget(btnRemove_);
        bar->addSpacing(16);
        bar->addWidget(btnRefresh_);
        bar->addWidget(btnSave_);
        bar->addWidget(btnClose_);
        main->addLayout(bar);

        connect(btnAdd_,    &QPushButton::clicked, this, &MedicalOrdersDialog::onAddRow);
        connect(btnRemove_, &QPushButton::clicked, this, &MedicalOrdersDialog::onRemoveRow);
        connect(btnRefresh_,&QPushButton::clicked, this, &MedicalOrdersDialog::onRefresh);
        connect(btnSave_,   &QPushButton::clicked, this, &MedicalOrdersDialog::onSave);
        connect(btnClose_,  &QPushButton::clicked, this, &QDialog::accept);
    }
}

void MedicalOrdersDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog { background: #fff; }
        #ordersItemsTable { gridline-color:#CBD5E1; }
        QTableWidget::item:selected { background:#E0F2FE; }
        QLabel { color:#1f2937; }
        QPushButton { height:32px; padding:0 12px; }
    )");
}

void MedicalOrdersDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    // 首次显示即刷新（如果 Main 在构造后立即主动 load 也没问题）
    emit requestLoadOrders(patientId_, orderId_);
}

void MedicalOrdersDialog::onRefresh()
{
    emit requestLoadOrders(patientId_, orderId_);
}

void MedicalOrdersDialog::onAddRow()
{
    const int r = tbl_->rowCount();
    tbl_->insertRow(r);
    for (int c=0; c<tbl_->columnCount(); ++c) {
        auto *it = new QTableWidgetItem();
        if (c==4 || c==5) it->setText("0");   // days/qty 缺省 0
        if (c==6)         it->setText("0.00");// amount 缺省 0
        tbl_->setItem(r, c, it);
    }
}

void MedicalOrdersDialog::onRemoveRow()
{
    const int r = tbl_->currentRow();
    if (r < 0) return;
    tbl_->removeRow(r);
    recalcTotal();
}

void MedicalOrdersDialog::onSave()
{
    // 最少要有一行内容
    if (tbl_->rowCount() == 0) {
        QMessageBox::warning(this, "提示", "请先新增至少一条医嘱项目。");
        return;
    }
    const auto payload = buildPayload();
    emit requestSaveOrders(payload);
}

void MedicalOrdersDialog::onOrdersLoaded(const QJsonObject &orderHeader,
                                         const QJsonArray  &items,
                                         bool found)
{
    // 标识/标题
    orderId_ = orderHeader.value("order_id").toInt(orderId_);
    if (lblOrderId_) lblOrderId_->setText(orderId_>0
        ? QString("医嘱单：#%1").arg(orderId_)
        : QString("医嘱单：新建"));

    // 填 header
    fillHeader(orderHeader);

    // 填 items
    fillItems(items);
}

void MedicalOrdersDialog::onOrdersSaved(bool ok, int orderId, const QString &msg)
{
    if (!ok) {
        QMessageBox::warning(this, "保存失败", msg.isEmpty() ? "保存失败" : msg);
        return;
    }
    orderId_ = orderId;
    if (lblOrderId_) lblOrderId_->setText(QString("医嘱单：#%1").arg(orderId_));
    QMessageBox::information(this, "已保存", "医嘱已保存。");

    // 保存后刷新一次，确保与服务器对齐
    emit requestLoadOrders(patientId_, orderId_);
}

void MedicalOrdersDialog::fillHeader(const QJsonObject &o)
{
    // 服务端字段名可能不同，尽量容错
    const QString dept   = asString(o, "department");
    const QString doctor = asString(o, "doctor_name");
    const QString note   = asString(o, "note");

    if (edtDept_)   edtDept_->setText(dept);
    if (edtDoctor_) edtDoctor_->setText(doctor);
    if (edtNote_)   edtNote_->setText(note);
}

void MedicalOrdersDialog::fillItems(const QJsonArray &arr)
{
    clearTable();
    tbl_->setRowCount(arr.size());
    for (int i=0;i<arr.size();++i) {
        const QJsonObject it = arr.at(i).toObject();
        auto get = [&](const char *k){ return it.value(k).toVariant().toString(); };

        auto set = [&](int col, const QString &val){
            auto *cell = new QTableWidgetItem(val);
            tbl_->setItem(i, col, cell);
        };
        // 尽量兼容不同返回字段名（content/item_name、amount/price等）
        set(0, get("content").isEmpty()? get("item_name") : get("content"));
        set(1, get("dose"));
        set(2, get("route"));
        set(3, get("frequency"));
        set(4, get("days"));
        set(5, get("qty").isEmpty()? get("quantity") : get("qty"));

        // 金额
        QString amt = get("amount");
        if (amt.isEmpty()) {
            const double price = asNumber(it, "price", 0.0);
            const double qty   = asNumber(it, "qty", asNumber(it,"quantity",0.0));
            amt = yuan(price * qty);
        }
        set(6, amt);
        set(7, get("note"));
    }
    recalcTotal();
}

void MedicalOrdersDialog::clearTable()
{
    tbl_->setRowCount(0);
}

void MedicalOrdersDialog::recalcTotal()
{
    double sum = 0.0;
    for (int r=0; r<tbl_->rowCount(); ++r) {
        bool ok=false;
        const double v = tbl_->item(r,6) ? tbl_->item(r,6)->text().toDouble(&ok) : 0.0;
        if (ok) sum += v;
    }
    if (lblTotal_) lblTotal_->setText(QString("合计：¥%1").arg(yuan(sum)));
}

QJsonObject MedicalOrdersDialog::buildPayload() const
{
    // header
    QJsonObject order;
    if (orderId_>0) order.insert("order_id", orderId_);
    order.insert("patient_id", patientId_);
    if (!edtDept_->text().trimmed().isEmpty())   order.insert("department",  edtDept_->text().trimmed());
    if (!edtDoctor_->text().trimmed().isEmpty()) order.insert("doctor_name", edtDoctor_->text().trimmed());
    if (!edtNote_->text().trimmed().isEmpty())   order.insert("note",        edtNote_->text().trimmed());

    // items
    QJsonArray items;
    for (int r=0; r<tbl_->rowCount(); ++r) {
        auto cell = [&](int c){ return tbl_->item(r,c) ? tbl_->item(r,c)->text().trimmed() : QString(); };
        QJsonObject it{
            {"content",   cell(0)},
            {"dose",      cell(1)},
            {"route",     cell(2)},
            {"frequency", cell(3)},
            {"days",      cell(4)},
            {"qty",       cell(5)},
            {"amount",    cell(6)},
            {"note",      cell(7)}
        };
        // 略做校验：内容不能为空
        if (it.value("content").toString().isEmpty()) continue;
        items.append(it);
    }
    // 最少 1 条
    if (items.isEmpty()) {
        QMessageBox::warning(nullptr, "提示", "至少填写一条有效医嘱项目（项目/内容不能为空）。");
        return {};
    }

    // 服务器这边的 save_medical_orders 没限定字段名，这里统一放在根
    // 也可根据你后端习惯改成 {"order":{...},"items":[...]}
    QJsonObject payload;
    for (auto it = order.begin(); it != order.end(); ++it) payload.insert(it.key(), it.value());
    payload.insert("items", items);
    return payload;
}

QString MedicalOrdersDialog::asString(const QJsonObject &o, const char *key) {
    const auto v = o.value(key);
    return v.isString() ? v.toString() : v.toVariant().toString();
}
double MedicalOrdersDialog::asNumber(const QJsonObject &o, const char *key, double def) {
    const auto v = o.value(key);
    if (v.isDouble()) return v.toDouble();
    bool ok=false; const double d=v.toVariant().toString().toDouble(&ok);
    return ok ? d : def;
}
