#include "medicine_search_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <widget.h>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QJsonArray>
#include <QButtonGroup>
#include <QJsonObject>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSpacerItem>
#include <QMessageBox>
class Widget;
// 药品详情对话框实现
MedicineDetailDialog::MedicineDetailDialog(const Medicine &medicine, QWidget *parent)
        : QDialog(parent)
{
    setWindowTitle(medicine.name);
    setFixedSize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    setupUI(medicine);
    initStyles();
}

void MedicineDetailDialog::setupUI(const Medicine &medicine)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 头部信息
    QFrame *headerFrame = new QFrame();
    headerFrame->setObjectName("detailHeaderFrame");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    headerLayout->setSpacing(15);

    // 关闭按钮
    QPushButton *closeButton = new QPushButton("×");
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);

    headerLayout->addStretch();
    headerLayout->addWidget(closeButton);

    // 药品基本信息
    QFrame *infoFrame = new QFrame();
    infoFrame->setObjectName("medicineInfoFrame");
    QHBoxLayout *infoLayout = new QHBoxLayout(infoFrame);
    infoLayout->setContentsMargins(20, 20, 20, 20);
    infoLayout->setSpacing(15);

    // 药品图标
    QLabel *iconLabel = new QLabel();
    iconLabel->setObjectName("medicineIcon");
    iconLabel->setFixedSize(60, 60);
    iconLabel->setStyleSheet(QString("background-color: %1; border-radius: 30px;").arg(medicine.iconColor));

    // 药品名称和价格
    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *nameLabel = new QLabel(medicine.name);
    nameLabel->setObjectName("medicineName");

    QLabel *typeLabel = new QLabel(medicine.type);
    typeLabel->setObjectName("medicineType");
    if (medicine.isPrescription) {
        typeLabel->setStyleSheet("color: #E53E3E; background-color: #FED7D7; padding: 2px 8px; border-radius: 10px;");
    } else {
        typeLabel->setStyleSheet("color: #38A169; background-color: #C6F6D5; padding: 2px 8px; border-radius: 10px;");
    }

    QLabel *priceLabel = new QLabel(QString("¥%1").arg(medicine.price, 0, 'f', 2));
    priceLabel->setObjectName("medicinePrice");

    titleLayout->addWidget(nameLabel);
    titleLayout->addWidget(typeLabel);
    titleLayout->addWidget(priceLabel);

    infoLayout->addWidget(iconLabel);
    infoLayout->addLayout(titleLayout);
    infoLayout->addStretch();

    // 详细信息区域
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 10, 20, 20);
    contentLayout->setSpacing(15);

    // 药品信息
    contentLayout->addWidget(new QLabel("药品信息"));

    QFrame *infoSection = new QFrame();
    infoSection->setObjectName("infoSection");
    QVBoxLayout *infoSectionLayout = new QVBoxLayout(infoSection);
    infoSectionLayout->setSpacing(5);

    infoSectionLayout->addWidget(new QLabel("通用名称"));
    infoSectionLayout->addWidget(new QLabel(medicine.name));
    infoSectionLayout->addWidget(new QLabel("规格"));
    infoSectionLayout->addWidget(new QLabel(medicine.specifications));
    infoSectionLayout->addWidget(new QLabel("生产厂家"));
    infoSectionLayout->addWidget(new QLabel(medicine.manufacturer));

    contentLayout->addWidget(infoSection);

    // 功效主治
    contentLayout->addWidget(new QLabel("功效主治"));
    QLabel *effectsLabel = new QLabel(medicine.effects);
    effectsLabel->setWordWrap(true);
    effectsLabel->setObjectName("effectsLabel");
    contentLayout->addWidget(effectsLabel);

    // 用法用量
    contentLayout->addWidget(new QLabel("用法用量"));
    QLabel *dosageLabel = new QLabel(medicine.dosage);
    dosageLabel->setWordWrap(true);
    dosageLabel->setObjectName("dosageLabel");
    contentLayout->addWidget(dosageLabel);

    scrollArea->setWidget(contentWidget);

    // 底部按钮
    QFrame *buttonFrame = new QFrame();
    buttonFrame->setObjectName("buttonFrame");
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    buttonLayout->setContentsMargins(20, 15, 20, 15);

    if (medicine.isPrescription) {
        // 处方药只显示提示
        QLabel *prescriptionWarning = new QLabel("处方药需医生处方");
        prescriptionWarning->setObjectName("prescriptionWarning");
        buttonLayout->addWidget(prescriptionWarning);
    } else {
        // 非处方药显示立即购买按钮
        QPushButton *purchaseButton = new QPushButton("立即购买");
        purchaseButton->setObjectName("purchaseButton");
        connect(purchaseButton, &QPushButton::clicked, this, [this, medicine]() {
            emit purchaseRequested(medicine.name);
            close();
        });
        buttonLayout->addStretch();
        buttonLayout->addWidget(purchaseButton);
    }

    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(infoFrame);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(buttonFrame);
}

void MedicineDetailDialog::initStyles()
{
    setStyleSheet(R"(
        QDialog {
            background-color: white;
            border-radius: 8px;
        }
        #detailHeaderFrame {
            background-color: white;
            border-bottom: 1px solid #E2E8F0;
        }
        #closeButton {
            background-color: transparent;
            border: none;
            font-size: 20px;
            font-weight: bold;
            color: #A0AEC0;
        }
        #closeButton:hover {
            background-color: #F7FAFC;
            border-radius: 15px;
        }
        #medicineInfoFrame {
            background-color: #F7FAFC;
            border-bottom: 1px solid #E2E8F0;
        }
        #medicineName {
            font-size: 20px;
            font-weight: bold;
            color: #2D3748;
        }
        #medicinePrice {
            font-size: 18px;
            font-weight: bold;
            color: #3182CE;
        }
        #infoSection {
            background-color: #F7FAFC;
            border-radius: 8px;
            padding: 10px;
        }
        #effectsLabel, #dosageLabel {
            color: #4A5568;
            line-height: 1.6;
        }
        #buttonFrame {
            background-color: white;
            border-top: 1px solid #E2E8F0;
        }
        #purchaseButton {
            background-color: #38A169;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        #purchaseButton:hover {
            background-color: #2F855A;
        }
        #prescriptionWarning {
            color: #E53E3E;
            font-weight: bold;
            background-color: #FED7D7;
            padding: 8px 12px;
            border-radius: 6px;
        }
    )");
}

// 主搜索界面实现
MedicineSearchWidget::MedicineSearchWidget(Widget *api, int patientId, QWidget *parent)
    : QWidget(parent),
      m_api(api),searchInput(nullptr), m_patientId(patientId), categoryGroup(nullptr),
          allButton(nullptr), prescriptionButton(nullptr), nonPrescriptionButton(nullptr),
          medicineScrollArea(nullptr), medicineListLayout(nullptr),
          selectedCountLabel(nullptr), batchPurchaseButton(nullptr),
          currentCategory("全部"), warningFrame(nullptr)
{
    setObjectName("MedicineSearchWidget");
    loadMedicineData();
    initUI();
    initStyleSheets();
    updateMedicineList();
    if (m_api && m_patientId > 0) {
            // 绑定信号
            connect(m_api, &Widget::ensurePendingOrderReady, this, &MedicineSearchWidget::onEnsureOrderReady);
            connect(m_api, &Widget::orderItemAdded, this, &MedicineSearchWidget::onOrderItemAdded);
            connect(m_api, &Widget::medicinesLoaded, this, &MedicineSearchWidget::onMedicinesLoaded); // 可选

            // 确保有购物单
            m_api->sendEnsurePendingOrder(m_patientId);
            // 若要用数据库药品列表，下面这一句放开并去掉本地数据：
            // m_api->sendSearchMedicines("", "", -1);
        }
}

MedicineSearchWidget::~MedicineSearchWidget() {}

void MedicineSearchWidget::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 顶部标题栏
    QWidget* header = new QWidget();
    header->setObjectName("medicineHeader");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    QPushButton *backButton = new QPushButton("← 返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &MedicineSearchWidget::backRequested);

    QLabel *title = new QLabel("药品搜索");
    title->setObjectName("pageTitle");

    QLabel *userName = new QLabel("张患者");
    userName->setObjectName("userName");

    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(userName);

    mainLayout->addWidget(header);

    // 主内容区域
    QFrame *contentFrame = new QFrame();
    contentFrame->setObjectName("contentFrame");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setContentsMargins(30, 20, 30, 20);
    contentLayout->setSpacing(20);

    contentLayout->addWidget(createSearchPanel());
    contentLayout->addWidget(createMedicineListPanel());
    contentLayout->addWidget(createBottomPanel());

    mainLayout->addWidget(contentFrame);
}

QWidget* MedicineSearchWidget::createSearchPanel()
{
    QFrame *searchFrame = new QFrame();
    searchFrame->setObjectName("searchFrame");

    QVBoxLayout *layout = new QVBoxLayout(searchFrame);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 搜索输入框
    QHBoxLayout *inputLayout = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setObjectName("searchInput");
    searchInput->setPlaceholderText("搜索药品名称、成分或功效...");

    QPushButton *searchButton = new QPushButton("🔍 搜索");
    searchButton->setObjectName("searchButton");
    connect(searchButton, &QPushButton::clicked, this, &MedicineSearchWidget::onSearchClicked);

    inputLayout->addWidget(searchInput);
    inputLayout->addWidget(searchButton);

    // 分类按钮
    QHBoxLayout *categoryLayout = new QHBoxLayout();
    categoryGroup = new QButtonGroup(this);

    allButton = new QPushButton("📋 全部");
    allButton->setObjectName("categoryButton");
    allButton->setCheckable(true);
    allButton->setChecked(true);

    prescriptionButton = new QPushButton("💊 处方药");
    prescriptionButton->setObjectName("categoryButton");
    prescriptionButton->setCheckable(true);

    nonPrescriptionButton = new QPushButton("🏥 非处方药");
    nonPrescriptionButton->setObjectName("categoryButton");
    nonPrescriptionButton->setCheckable(true);

    categoryGroup->addButton(allButton, 0);
    categoryGroup->addButton(prescriptionButton, 1);
    categoryGroup->addButton(nonPrescriptionButton, 2);

    connect(categoryGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &MedicineSearchWidget::onCategoryChanged);

    categoryLayout->addWidget(allButton);
    categoryLayout->addWidget(prescriptionButton);
    categoryLayout->addWidget(nonPrescriptionButton);
    categoryLayout->addStretch();

    layout->addLayout(inputLayout);
    layout->addLayout(categoryLayout);

    return searchFrame;
}

QWidget* MedicineSearchWidget::createMedicineListPanel()
{
    QFrame *listFrame = new QFrame();
    listFrame->setObjectName("medicineListFrame");

    QVBoxLayout *layout = new QVBoxLayout(listFrame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 表头
    QFrame *headerFrame = new QFrame();
    headerFrame->setObjectName("tableHeader");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QLabel *selectLabel = new QLabel("选择");
    selectLabel->setFixedWidth(60);
    QLabel *infoLabel = new QLabel("药品信息");
    infoLabel->setFixedWidth(300);
    QLabel *typeLabel = new QLabel("类型");
    typeLabel->setFixedWidth(100);
    QLabel *priceLabel = new QLabel("价格");
    priceLabel->setFixedWidth(80);
    QLabel *actionLabel = new QLabel("操作");

    headerLayout->addWidget(selectLabel);
    headerLayout->addWidget(infoLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(typeLabel);
    headerLayout->addWidget(priceLabel);
    headerLayout->addWidget(actionLabel);

    // 药品列表滚动区域
    medicineScrollArea = new QScrollArea();
    medicineScrollArea->setWidgetResizable(true);
    medicineScrollArea->setFrameShape(QFrame::NoFrame);
    medicineScrollArea->setObjectName("medicineScrollArea");

    QWidget *container = new QWidget();
    medicineListLayout = new QVBoxLayout(container);
    medicineListLayout->setContentsMargins(0, 0, 0, 0);
    medicineListLayout->setSpacing(0);

    medicineScrollArea->setWidget(container);

    layout->addWidget(headerFrame);
    layout->addWidget(medicineScrollArea);

    return listFrame;
}

QWidget* MedicineSearchWidget::createBottomPanel()
{
    QVBoxLayout *bottomLayout = new QVBoxLayout();

    // 选中数量和批量购买
    QFrame *selectionFrame = new QFrame();
    selectionFrame->setObjectName("selectionFrame");
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionFrame);
    selectionLayout->setContentsMargins(20, 10, 20, 10);

    selectedCountLabel = new QLabel("已选择 0 个药品");
    selectedCountLabel->setObjectName("selectedCountLabel");

    batchPurchaseButton = new QPushButton("📦 立即购买");
    batchPurchaseButton->setObjectName("batchPurchaseButton");
    batchPurchaseButton->setEnabled(false);
    connect(batchPurchaseButton, &QPushButton::clicked, this, &MedicineSearchWidget::onBatchPurchaseClicked);

    selectionLayout->addWidget(selectedCountLabel);
    selectionLayout->addStretch();
    selectionLayout->addWidget(batchPurchaseButton);

    // 处方药提示
    warningFrame = new QFrame();
    warningFrame->setObjectName("warningFrame");
    QHBoxLayout *warningLayout = new QHBoxLayout(warningFrame);
    warningLayout->setContentsMargins(15, 10, 15, 10);

    QLabel *warningIcon = new QLabel("⚠️");
    QLabel *warningText = new QLabel("处方药需凭医生处方购买，请咨询医生后使用。非处方药可直接购买。");
    warningText->setObjectName("warningText");

    warningLayout->addWidget(warningIcon);
    warningLayout->addWidget(warningText);
    warningLayout->addStretch();

    bottomLayout->addWidget(selectionFrame);
    bottomLayout->addWidget(warningFrame);

    QWidget *bottomWidget = new QWidget();
    bottomWidget->setLayout(bottomLayout);
    return bottomWidget;
}

QWidget* MedicineSearchWidget::createMedicineCard(const Medicine &medicine)
{
    QFrame *card = new QFrame();
    card->setObjectName("medicineCard");

    QHBoxLayout *layout = new QHBoxLayout(card);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(15);

    // 选择框（仅非处方药可选）
    QCheckBox *checkBox = new QCheckBox();
    checkBox->setProperty("medName", medicine.name); // ★ 供批量购买读取名字
    checkBox->setObjectName("medicineCheckBox");
    checkBox->setFixedWidth(60);
    checkBox->setEnabled(!medicine.isPrescription);
    if (medicine.isPrescription) {
        checkBox->setStyleSheet("QCheckBox { color: #A0AEC0; }");
    }
    medicineCheckboxes.append(checkBox);
    connect(checkBox, &QCheckBox::toggled, this, &MedicineSearchWidget::updateSelectedCount);

    // 药品图标
    QLabel *iconLabel = new QLabel();
    iconLabel->setObjectName("medicineCardIcon");
    iconLabel->setFixedSize(50, 50);
    iconLabel->setStyleSheet(QString("background-color: %1; border-radius: 25px;").arg(medicine.iconColor));

    // 药品信息
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);

    QLabel *nameLabel = new QLabel(medicine.name);
    nameLabel->setObjectName("medicineCardName");

    QLabel *descLabel = new QLabel(medicine.description);
    descLabel->setObjectName("medicineCardDesc");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(descLabel);

    // 药品类型标签
    QLabel *typeLabel = new QLabel(medicine.type);
    typeLabel->setObjectName("medicineTypeLabel");
    typeLabel->setFixedWidth(100);
    if (medicine.isPrescription) {
        typeLabel->setStyleSheet("color: #E53E3E; background-color: #FED7D7; padding: 4px 8px; border-radius: 10px; text-align: center;");
    } else {
        typeLabel->setStyleSheet("color: #38A169; background-color: #C6F6D5; padding: 4px 8px; border-radius: 10px; text-align: center;");
    }

    // 价格
    QLabel *priceLabel = new QLabel(QString("¥%1").arg(medicine.price, 0, 'f', 2));
    priceLabel->setObjectName("medicineCardPrice");
    priceLabel->setFixedWidth(80);

    // 操作按钮
    QVBoxLayout *actionLayout = new QVBoxLayout();
    QPushButton *detailButton = new QPushButton("详情");
    detailButton->setObjectName("detailButton");
    connect(detailButton, &QPushButton::clicked, this, [this, medicine]() {
        onDetailClicked(medicine);
    });

    QPushButton *purchaseButton = new QPushButton("购买");
    purchaseButton->setObjectName("purchaseButton");
    purchaseButton->setEnabled(!medicine.isPrescription);
    if (medicine.isPrescription) {
        purchaseButton->setText("处方");
        purchaseButton->setStyleSheet("QPushButton { background-color: #E2E8F0; color: #A0AEC0; }");
    }
    connect(purchaseButton, &QPushButton::clicked, this, [this, medicine]() {
        if (!medicine.isPrescription) {
            onPurchaseClicked(medicine);
        }
    });

    actionLayout->addWidget(detailButton);
    actionLayout->addWidget(purchaseButton);

    layout->addWidget(checkBox);
    layout->addWidget(iconLabel);
    layout->addLayout(infoLayout, 1);
    layout->addWidget(typeLabel);
    layout->addWidget(priceLabel);
    layout->addLayout(actionLayout);

    return card;
}

void MedicineSearchWidget::loadMedicineData()
{
    allMedicines = {
            {
                    "阿莫西林胶囊",
                    "用于敏感菌所致的感染",
                    "处方药",
                    25.80,
                    "0.25g*24粒",
                    "某某制药有限公司",
                    "用于敏感菌所致的感染，如呼吸道感染、泌尿系统感染等。",
                    "口服。成人一次0.5g，每6～8小时1次，一日剂量不超过4g。",
                    "#FEB2B2",
                    true
            },
            {
                    "板蓝根颗粒",
                    "清热解毒，凉血利咽",
                    "非处方药",
                    18.50,
                    "10g*20袋",
                    "某某药业有限公司",
                    "清热解毒，凉血利咽。用于肺胃热盛所致的咽喉肿痛、口咽干燥。",
                    "开水冲服。一次1-2袋，一日3-4次。",
                    "#9AE6B4",
                    false
            },
            {
                    "硝苯地平控释片",
                    "用于高血压、冠心病治疗",
                    "处方药",
                    42.30,
                    "30mg*7片",
                    "某某制药股份有限公司",
                    "用于高血压、冠心病治疗",
                    "口服。成人一次0.5g，每6～8小时1次，一日剂量不超过4g。",
                    "#FEB2B2",
                    true
            },
            {
                    "维生素C片",
                    "补充维生素C，增强免疫力",
                    "非处方药",
                    15.90,
                    "100mg*100片",
                    "某某保健品有限公司",
                    "补充维生素C，增强免疫力",
                    "口服。成人一次1-2片，一日1-3次。",
                    "#9AE6B4",
                    false
            }
    };
}

void MedicineSearchWidget::onSearchClicked()
{
    updateMedicineList();
}
void MedicineSearchWidget::onEnsureOrderReady(int orderId) {
    m_orderId = orderId;
}

void MedicineSearchWidget::onCategoryChanged()
{
    int buttonId = categoryGroup->checkedId();
    switch (buttonId) {
        case 0: currentCategory = "全部"; break;
        case 1: currentCategory = "处方药"; break;
        case 2: currentCategory = "非处方药"; break;
        default: currentCategory = "全部"; break;
    }
    updateMedicineList();
}

void MedicineSearchWidget::onDetailClicked(const Medicine &medicine)
{
    MedicineDetailDialog dialog(medicine, this);
    connect(&dialog, &MedicineDetailDialog::purchaseRequested, this, [this](const QString &medicineName) {
        QMessageBox::information(this, "购买成功", QString("已成功购买 %1").arg(medicineName));
    });
    dialog.exec();
}

void MedicineSearchWidget::onOrderItemAdded(bool ok, const QString &msg, const QJsonObject &order)
{
    if (!ok) {
        QMessageBox::warning(this, "加入失败", msg.isEmpty()? "请稍后再试" : msg);
        return;
    }
    // 友好提示
    QMessageBox::information(this, "已加入购买单", "药品已加入购买单。可前往“线上支付”查看并支付。");
    // 清勾选
    for (QCheckBox *cb : medicineCheckboxes) cb->setChecked(false);
    updateSelectedCount();
}

// （可选）如果用后端药品列表，这里把 allMedicines 替换
void MedicineSearchWidget::onMedicinesLoaded(const QJsonArray &items)
{
    allMedicines.clear();
    for (const auto &v : items) {
        const auto o = v.toObject();
        Medicine m;
        m.name = o.value("name").toString();
        m.description = o.value("description").toString();
        m.type = o.value("type").toString();
        m.price = o.value("price").toDouble();
        m.specifications = o.value("specifications").toString();
        m.manufacturer = o.value("manufacturer").toString();
        m.effects = o.value("effects").toString();
        m.dosage = o.value("dosage").toString();
        m.iconColor = o.value("icon_color").toString();
        m.isPrescription = o.value("is_prescription").toInt() == 1;
        allMedicines.append(m);
    }
    updateMedicineList();
}
void MedicineSearchWidget::onPurchaseClicked(const Medicine &medicine)
{
    if (!m_api || m_orderId <= 0) {
        QMessageBox::warning(this, "提示", "购买单未就绪，请稍后再试");
        return;
    }
    // 用药名（唯一）加入购物单；数量先固定 1
    m_api->sendAddMedicineToOrderByName(m_orderId, medicine.name, 1);
}
// 批量购买：遍历勾选项加入购买单
void MedicineSearchWidget::onBatchPurchaseClicked()
{
    if (!m_api || m_orderId <= 0) {
        QMessageBox::warning(this, "提示", "购买单未就绪，请稍后再试");
        return;
    }
    int selectedCount = 0;
    for (QCheckBox *checkBox : medicineCheckboxes) {
        if (checkBox->isChecked()) {
            // 从复选框向上找这条卡片对应的名字 label
            // 这里因为你的布局写法较自由，最安全是：在 createMedicineCard 时把 name 放到 checkBox 的 property
            selectedCount++;
        }
    }
    // 简化：我们在 createMedicineCard 里给 checkBox 设置 "medName" 属性：
    //   checkBox->setProperty("medName", medicine.name);
    for (QCheckBox *cb : medicineCheckboxes) {
        if (cb->isChecked()) {
            const QString name = cb->property("medName").toString();
            if (!name.isEmpty()) m_api->sendAddMedicineToOrderByName(m_orderId, name, 1);
        }
    }
    // 不再本地清空勾选，等服务端确认再提示
}
void MedicineSearchWidget::updateMedicineList()
{
    // 清空现有列表
    QLayoutItem *item;
    while ((item = medicineListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    medicineCheckboxes.clear();

    // 获取过滤后的药品
    QList<Medicine> filteredMedicines = getFilteredMedicines();

    // 创建药品卡片
    for (const Medicine &medicine : filteredMedicines) {
        medicineListLayout->addWidget(createMedicineCard(medicine));
    }

    medicineListLayout->addStretch();
    updateSelectedCount();
}

void MedicineSearchWidget::updateSelectedCount()
{
    int selectedCount = 0;
    for (QCheckBox *checkBox : medicineCheckboxes) {
        if (checkBox->isChecked()) {
            selectedCount++;
        }
    }

    selectedCountLabel->setText(QString("已选择 %1 个药品").arg(selectedCount));
    batchPurchaseButton->setEnabled(selectedCount > 0);
}

QList<Medicine> MedicineSearchWidget::getFilteredMedicines()
{
    QList<Medicine> filtered;
    QString searchText = searchInput ? searchInput->text().toLower() : "";

    for (const Medicine &medicine : allMedicines) {
        // 分类过滤
        if (currentCategory != "全部") {
            if (currentCategory == "处方药" && !medicine.isPrescription) continue;
            if (currentCategory == "非处方药" && medicine.isPrescription) continue;
        }

        // 搜索过滤
        if (!searchText.isEmpty()) {
            QString medicineName = medicine.name.toLower();
            QString medicineDesc = medicine.description.toLower();
            QString medicineEffects = medicine.effects.toLower();

            if (!medicineName.contains(searchText) &&
                !medicineDesc.contains(searchText) &&
                !medicineEffects.contains(searchText)) {
                continue;
            }
        }

        filtered.append(medicine);
    }

    return filtered;
}

void MedicineSearchWidget::initStyleSheets()
{
    this->setStyleSheet(R"(
        #MedicineSearchWidget {
            background-color: #F0F4F8;
        }

        #medicineHeader {
            background-color: white;
            border-bottom: 1px solid #E2E8F0;
        }

        #pageTitle {
            font-size: 20px;
            font-weight: bold;
            color: #2D3748;
        }

        #backButton {
            font-size: 14px;
            color: #4A5568;
            border: none;
            background: transparent;
            padding: 8px 12px;
        }
        #backButton:hover {
            color: #2D3748;
            background-color: #F7FAFC;
            border-radius: 6px;
        }

        #userName {
            font-size: 14px;
            color: #3182CE;
            background-color: #EBF8FF;
            padding: 6px 12px;
            border-radius: 15px;
        }

        #contentFrame {
            background-color: transparent;
        }

        #searchFrame {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
            margin-bottom: 20px;
        }

        #searchInput {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 10px 15px;
            font-size: 14px;
            background-color: white;
        }
        #searchInput:focus {
            border-color: #3182CE;
            outline: none;
        }

        #searchButton {
            background-color: #3182CE;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
            min-width: 100px;
        }
        #searchButton:hover {
            background-color: #2B6CB0;
        }

        #categoryButton {
            background-color: #F7FAFC;
            color: #4A5568;
            border: 1px solid #E2E8F0;
            border-radius: 20px;
            padding: 8px 16px;
            font-size: 14px;
        }
        #categoryButton:hover {
            background-color: #EDF2F7;
        }
        #categoryButton:checked {
            background-color: #3182CE;
            color: white;
            border-color: #3182CE;
        }

        #medicineListFrame {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
        }

        #tableHeader {
            background-color: #F7FAFC;
            border-bottom: 1px solid #E2E8F0;
            font-weight: bold;
            color: #2D3748;
        }

        #medicineScrollArea {
            background-color: transparent;
        }

        #medicineCard {
            background-color: white;
            border-bottom: 1px solid #E2E8F0;
        }
        #medicineCard:hover {
            background-color: #F7FAFC;
        }

        #medicineCardName {
            font-size: 16px;
            font-weight: bold;
            color: #2D3748;
        }

        #medicineCardDesc {
            font-size: 14px;
            color: #718096;
        }

        #medicineCardPrice {
            font-size: 16px;
            font-weight: bold;
            color: #3182CE;
        }

        #detailButton {
            background-color: transparent;
            color: #3182CE;
            border: 1px solid #3182CE;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 12px;
        }
        #detailButton:hover {
            background-color: #EBF8FF;
        }

        #purchaseButton {
            background-color: #38A169;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 12px;
            font-weight: bold;
        }
        #purchaseButton:hover {
            background-color: #2F855A;
        }
        #purchaseButton:disabled {
            background-color: #E2E8F0;
            color: #A0AEC0;
        }

        #selectionFrame {
            background-color: #EDF2F7;
            border-radius: 8px;
            margin-top: 10px;
        }

        #selectedCountLabel {
            font-size: 14px;
            color: #4A5568;
        }

        #batchPurchaseButton {
            background-color: #3182CE;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        #batchPurchaseButton:hover {
            background-color: #2B6CB0;
        }
        #batchPurchaseButton:disabled {
            background-color: #CBD5E0;
            cursor: not-allowed;
        }

        #warningFrame {
            background-color: #FED7D7;
            border: 1px solid #FCA5A5;
            border-radius: 8px;
            margin-top: 10px;
        }

        #warningText {
            font-size: 14px;
            color: #C53030;
        }

        #medicineCheckBox {
            color: #4A5568;
        }
        #medicineCheckBox:checked {
            color: #3182CE;
        }
        #medicineCheckBox:disabled {
            color: #A0AEC0;
        }
    )");
}
