#include "online_payment_widget.h"
#include "payment_success_dialog.h" // 包含支付成功弹窗
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include "widget.h"
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QButtonGroup>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QTimer>
#include <QButtonGroup>
#include <QWidget>
#include <QJsonArray>
#include <QPropertyAnimation> // 用于更精细的动画控制
#include <QRegularExpression>

OnlinePaymentWidget::OnlinePaymentWidget(QWidget *parent)
    : OnlinePaymentWidget(nullptr, -1, parent) {}

OnlinePaymentWidget::OnlinePaymentWidget(Widget *api, int patientId, QWidget *parent)
    : QWidget(parent),
      m_api(api),
      m_patientId(patientId),
      m_orderId(-1)
{
    setObjectName("OnlinePaymentWidget");
    initUI();
    initStyleSheets();

    if (m_api && m_patientId > 0) {
        // 只接信号，不改 TCP 代码
        connect(m_api, &Widget::ensurePendingOrderReady,
                this,   &OnlinePaymentWidget::onEnsureOrderReady);
        connect(m_api, &Widget::orderDetailLoaded,
                this,   &OnlinePaymentWidget::onOrderDetailLoaded);
        connect(m_api, &Widget::paymentProcessed,
                this,   &OnlinePaymentWidget::onPaymentProcessed);

        // 启动时确保有待支付订单
        m_api->sendEnsurePendingOrder(m_patientId);
    } else {
        // 无后端：本地占位
        loadOrderDetails();
    }
}


OnlinePaymentWidget::~OnlinePaymentWidget() {}


void OnlinePaymentWidget::initUI() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 顶部
    QWidget* globalHeader = new QWidget();
    globalHeader->setObjectName("globalHeader");
    auto *globalHeaderLayout = new QHBoxLayout(globalHeader);
    globalHeaderLayout->setContentsMargins(20, 10, 20, 10);

    auto *backButton = new QPushButton(tr("返回首页"));
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &OnlinePaymentWidget::backRequested);

    auto *title = new QLabel(tr("线上支付"));
    title->setObjectName("pageTitle");

    globalHeaderLayout->addWidget(backButton);
    globalHeaderLayout->addSpacing(20);
    globalHeaderLayout->addWidget(title);
    globalHeaderLayout->addStretch();
    mainLayout->addWidget(globalHeader);

    // 内容区域
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(40, 20, 40, 40);
    contentLayout->setSpacing(30);

    // 左：订单信息（初始用空数据，后续 onOrderDetailLoaded 填充）
    contentLayout->addWidget(createOrderInfoPanel(currentOrder), 1);
    // 右：支付方式
    contentLayout->addWidget(createPaymentMethodPanel(), 1);

    mainLayout->addLayout(contentLayout);

    // loading 覆盖层
    loadingOverlay = new QWidget(this);
    loadingOverlay->setObjectName("loadingOverlay");
    loadingOverlay->hide();
    auto *loadingLayout = new QVBoxLayout(loadingOverlay);
    loadingLayout->setAlignment(Qt::AlignCenter);

    loadingSpinner = new QLabel();
    loadingSpinner->setObjectName("loadingSpinner");
    loadingSpinner->setFixedSize(50, 50);
    loadingLayout->addWidget(loadingSpinner, 0, Qt::AlignCenter);

    loadingText = new QLabel(tr("支付处理中，请稍候..."));
    loadingText->setObjectName("loadingText");
    loadingLayout->addWidget(loadingText, 0, Qt::AlignCenter);

    loadingOverlay->setGeometry(this->rect());
    loadingOverlay->raise();

    paymentProcessTimer = new QTimer(this);
    connect(paymentProcessTimer, &QTimer::timeout, this, &OnlinePaymentWidget::updatePaymentStatusText);
}

void OnlinePaymentWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (loadingOverlay) loadingOverlay->setGeometry(this->rect());
}
// xiamiangaide
void OnlinePaymentWidget::loadOrderDetails()
{
    // 交给 Api 去发网络请求
    emit requestLoadOrderDetails();
}
void OnlinePaymentWidget::onLoadOrderDetailsOk(const QJsonArray &orders)
{
    if (orders.isEmpty()) {
        qWarning() << "[OnlinePaymentWidget] no orders received";
        return;
    }

    // 这里示例取第一条订单（你也可以弹出列表让用户选）
    const QJsonObject o = orders.at(0).toObject();

    currentOrder.orderId     = o.value("order_code").toString();     // 例如 PO20230830123456
    currentOrder.createTime  = o.value("create_time").toString();
    currentOrder.patientName = o.value("patient_name").toString();
    currentOrder.department  = o.value("department").toString();
    currentOrder.doctorName  = o.value("doctor_name").toString();

    currentOrder.feeDetails.clear();
    const QJsonObject fee = o.value("fee").toObject();
    currentOrder.feeDetails["挂号费"] = fee.value("挂号费").toDouble();
    currentOrder.feeDetails["诊查费"] = fee.value("诊查费").toDouble();
    currentOrder.feeDetails["药品费"] = fee.value("药品费").toDouble();
    currentOrder.feeDetails["检查费"] = fee.value("检查费").toDouble();

    currentOrder.discount    = o.value("discount").toDouble();       // 已是负数
    currentOrder.totalAmount = o.value("total_amount").toDouble();   // 应付

    refreshUi();
}

void OnlinePaymentWidget::processPayment(int paymentMethod) {
    qDebug() << "[mock] pay by" << (paymentMethod==0 ? "wechat" : "alipay");
    QTimer::singleShot(3000, this, &OnlinePaymentWidget::onPaymentProcessFinished);
}
// --- UI 构建函数实现 ---
QWidget* OnlinePaymentWidget::createHeader() {
    // 这个在initUI()中已经创建，这里是占位符
    return nullptr;
}

QWidget* OnlinePaymentWidget::createOrderInfoPanel(const OrderInfo &order) {
    auto *panel = new QFrame();
    panel->setObjectName("infoPanel");
    auto *mainLayout = new QVBoxLayout(panel);
    mainLayout->setContentsMargins(25,25,25,25);
    mainLayout->setSpacing(20);

    // 标题
    auto *orderTitle = new QLabel(tr("订单信息"));
    orderTitle->setObjectName("panelTitle");
    mainLayout->addWidget(orderTitle);

    // 基本信息
    auto *orderForm = new QFormLayout();
    orderForm->setContentsMargins(0,0,0,0);
    orderForm->setSpacing(10);
    orderForm->setLabelAlignment(Qt::AlignLeft);
    orderForm->addRow(tr("订单号"),    orderIdLabel     = new QLabel(order.orderId));
    orderForm->addRow(tr("创建时间"),  createTimeLabel  = new QLabel(order.createTime));
    orderForm->addRow(tr("患者姓名"),  patientNameLabel = new QLabel(order.patientName));
    orderForm->addRow(tr("就诊科室"),  departmentLabel  = new QLabel(order.department));
    orderForm->addRow(tr("医生"),      doctorNameLabel  = new QLabel(order.doctorName));
    mainLayout->addLayout(orderForm);

    // 订单条目
    auto *itemsTitle = new QLabel(tr("订单条目"));
    itemsTitle->setObjectName("panelTitle");
    mainLayout->addWidget(itemsTitle);

    auto *itemsArea = new QScrollArea();
    itemsArea->setWidgetResizable(true);
    itemsArea->setFrameShape(QFrame::NoFrame);

    auto *itemsBox = new QWidget();
    itemsListLayout = new QVBoxLayout(itemsBox);
    itemsListLayout->setContentsMargins(0,0,0,0);
    itemsListLayout->setSpacing(6);
    itemsListLayout->addWidget(new QLabel(tr("（正在加载...）")));

    itemsArea->setWidget(itemsBox);
    mainLayout->addWidget(itemsArea);

    // 费用明细
    auto *feeTitle = new QLabel(tr("费用明细"));
    feeTitle->setObjectName("panelTitle");
    mainLayout->addWidget(feeTitle);

    auto *feeForm = new QFormLayout();
    feeForm->setContentsMargins(0,0,0,0);
    feeForm->setSpacing(10);
    feeForm->setLabelAlignment(Qt::AlignLeft);
    feeForm->addRow(tr("挂号费"),  registrationFeeLabel = new QLabel(QString("¥%1").arg(order.feeDetails.value("挂号费",0.0),0,'f',2)));
    feeForm->addRow(tr("诊查费"),  consultationFeeLabel = new QLabel(QString("¥%1").arg(order.feeDetails.value("诊查费",0.0),0,'f',2)));
    feeForm->addRow(tr("药品费"),  medicineFeeLabel     = new QLabel(QString("¥%1").arg(order.feeDetails.value("药品费",0.0),0,'f',2)));
    feeForm->addRow(tr("检查费"),  examinationFeeLabel  = new QLabel(QString("¥%1").arg(order.feeDetails.value("检查费",0.0),0,'f',2)));
    feeForm->addRow(tr("优惠折扣"), discountLabel       = new QLabel(QString("¥%1").arg(order.discount,0,'f',2)));
    mainLayout->addLayout(feeForm);

    // 总计（用 QWidget 当容器）
    auto *totalAmountBox = new QWidget();
    totalAmountBox->setObjectName("totalAmountBox");
    auto *totalLayout = new QHBoxLayout(totalAmountBox);
    totalLayout->addStretch();
    auto *totalText = new QLabel(tr("应付总额"));
    totalText->setObjectName("totalText");
    totalAmountLabel = new QLabel(QString("¥%1").arg(order.totalAmount,0,'f',2));
    totalAmountLabel->setObjectName("totalAmountValue");
    totalLayout->addWidget(totalText);
    totalLayout->addWidget(totalAmountLabel);
    totalLayout->addStretch();

    mainLayout->addStretch();
    mainLayout->addWidget(totalAmountBox);

    return panel;
}
QWidget* OnlinePaymentWidget::createPaymentMethodPanel() {
    auto *panel = new QFrame();
    panel->setObjectName("paymentPanel");
    auto *mainLayout = new QVBoxLayout(panel);
    mainLayout->setContentsMargins(25,25,25,25);
    mainLayout->setSpacing(20);

    auto *payTitle = new QLabel(tr("选择支付方式"));
    payTitle->setObjectName("panelTitle");
    mainLayout->addWidget(payTitle);

    auto *payGroup = new QButtonGroup(this);
    payGroup->setExclusive(true);

    // 微信
    auto *wechatEntry = new QWidget();
    wechatEntry->setObjectName("paymentEntry");
    auto *wechatLayout = new QHBoxLayout(wechatEntry);
    auto *wechatIcon = new QLabel();
    wechatIcon->setObjectName("wechatIcon");
    wechatIcon->setFixedSize(32,32);
    auto *wechatTextLayout = new QVBoxLayout();
    auto *wechatTitle = new QLabel(tr("微信支付"));
    wechatTitle->setObjectName("paymentMethodTitle");
    auto *wechatHint = new QLabel(tr("推荐已安装微信的用户使用"));
    wechatHint->setObjectName("paymentMethodHint");
    wechatTextLayout->addWidget(wechatTitle);
    wechatTextLayout->addWidget(wechatHint);
    wechatPayRadio = new QRadioButton();
    wechatPayRadio->setChecked(true);
    wechatLayout->addWidget(wechatIcon);
    wechatLayout->addLayout(wechatTextLayout);
    wechatLayout->addStretch();
    wechatLayout->addWidget(wechatPayRadio);
    mainLayout->addWidget(wechatEntry);
    payGroup->addButton(wechatPayRadio, 0);

    // 支付宝
    auto *alipayEntry = new QWidget();
    alipayEntry->setObjectName("paymentEntry");
    auto *alipayLayout = new QHBoxLayout(alipayEntry);
    auto *alipayIcon = new QLabel();
    alipayIcon->setObjectName("alipayIcon");
    alipayIcon->setFixedSize(32,32);
    auto *alipayTextLayout = new QVBoxLayout();
    auto *alipayTitle = new QLabel(tr("支付宝支付"));
    alipayTitle->setObjectName("paymentMethodTitle");
    auto *alipayHint = new QLabel(tr("推荐已安装支付宝的用户使用"));
    alipayHint->setObjectName("paymentMethodHint");
    alipayTextLayout->addWidget(alipayTitle);
    alipayTextLayout->addWidget(alipayHint);
    alipayRadio = new QRadioButton();
    alipayLayout->addWidget(alipayIcon);
    alipayLayout->addLayout(alipayTextLayout);
    alipayLayout->addStretch();
    alipayLayout->addWidget(alipayRadio);
    mainLayout->addWidget(alipayEntry);
    payGroup->addButton(alipayRadio, 1);

    mainLayout->addStretch();

    // 确认支付
    confirmPaymentButton = new QPushButton(tr("确认支付 ¥366.50"));
    confirmPaymentButton->setObjectName("confirmPaymentButton");
    mainLayout->addWidget(confirmPaymentButton);
    connect(confirmPaymentButton, &QPushButton::clicked,
            this, &OnlinePaymentWidget::onConfirmPaymentClicked);

    return panel;
}

void OnlinePaymentWidget::onOrderDetailLoaded(const QJsonObject &order)
{
    if (order.isEmpty()) return;
    m_orderId = order.value("order_id").toInt(m_orderId);

    if (orderIdLabel)     orderIdLabel->setText(QString::number(m_orderId));
    if (createTimeLabel)  createTimeLabel->setText(order.value("created_at").toString());
    if (patientNameLabel) patientNameLabel->setText(tr("当前用户"));

    const double discount = order.value("discount").toDouble();
    const double total    = order.value("total_amount").toDouble();
    if (discountLabel)        discountLabel->setText(QString("¥%1").arg(discount,0,'f',2));
    if (totalAmountLabel)     totalAmountLabel->setText(QString("¥%1").arg(total,0,'f',2));
    if (confirmPaymentButton) confirmPaymentButton->setText(QString("确认支付 ¥%1").arg(total,0,'f',2));

    // 列表
    while (itemsListLayout && itemsListLayout->count() > 0) {
        QLayoutItem *it = itemsListLayout->takeAt(0);
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }
    const auto items = order.value("items").toArray();
    for (const auto &v : items) {
        const auto o = v.toObject();
        const QString line = QString("%1   ¥%2")
                .arg(o.value("item_name").toString())
                .arg(o.value("amount").toDouble(), 0, 'f', 2);
        itemsListLayout->addWidget(new QLabel(line));
    }
    if (items.isEmpty()) {
        itemsListLayout->addWidget(new QLabel(tr("（空空如也，去“药品搜索”加入购买单吧）")));
    }
}

// --- 槽函数实现 ---
void OnlinePaymentWidget::onConfirmPaymentClicked() {
    loadingOverlay->show();
    confirmPaymentButton->setEnabled(false);
    loadingText->setText(tr("支付处理中..."));
    loadingTextDotCount = 0;
    paymentProcessTimer->start(500);

    if (!m_api || m_orderId <= 0) {
        QMessageBox::warning(this, tr("提示"), tr("没有待支付订单"));
        loadingOverlay->hide();
        confirmPaymentButton->setEnabled(true);
        paymentProcessTimer->stop();
        return;
    }

    // 解析金额：去掉 ¥ 和千分位/空白
    QString amt = totalAmountLabel ? totalAmountLabel->text() : QString();
    amt.remove(QChar(0x00A5));             // '¥'
    amt.remove(QRegularExpression("[,\\s]"));
    const double amount = amt.toDouble();

    const QString method = wechatPayRadio && wechatPayRadio->isChecked()
                           ? QStringLiteral("wechat")
                           : QStringLiteral("alipay");
    m_api->sendCreatePayment(m_orderId, method, amount);
}


void OnlinePaymentWidget::updatePaymentStatusText() {
    loadingTextDotCount = (loadingTextDotCount + 1) % 4;
    QString dots(loadingTextDotCount, '.');
    loadingText->setText(tr("支付处理中，请稍候%1").arg(dots));
}

void OnlinePaymentWidget::onEnsureOrderReady(int orderId) {
    m_orderId = orderId;
    if (m_api) m_api->sendGetOrderDetail(orderId);
}

void OnlinePaymentWidget::onPaymentProcessed(bool ok, const QString &msg, const QJsonObject &)
{
    loadingOverlay->hide();
    confirmPaymentButton->setEnabled(true);
    paymentProcessTimer->stop();

    if (!ok) {
        QMessageBox::warning(this, tr("支付失败"),
                             msg.isEmpty() ? tr("请稍后再试") : msg);
        return;
    }
    QMessageBox::information(this, tr("支付成功"), tr("支付成功，购买单已清空。"));
    emit paymentCompleted();
    emit backRequested();
}
void OnlinePaymentWidget::onPaymentProcessFinished() {
    loadingOverlay->hide();
    confirmPaymentButton->setEnabled(true);
    paymentProcessTimer->stop();

    // 本地占位成功弹窗（有后端时基本用不到）
    PaymentSuccessDialog successDialog(
        totalAmountLabel ? totalAmountLabel->text().remove(QChar(0x00A5)).toDouble() : 0.0,
        this
    );
    connect(&successDialog, &PaymentSuccessDialog::backToDashboardClicked,
            this, &OnlinePaymentWidget::backRequested);
    successDialog.exec();
    emit paymentCompleted();
}
void OnlinePaymentWidget::initStyleSheets() {
    this->setStyleSheet(R"(
        #OnlinePaymentWidget { background-color: #F0F4F8; }
        #globalHeader { background-color: white; padding: 10px 40px; }
        #pageTitle { font-size: 20px; font-weight: bold; color: #2D3748; }
        #backButton { font-size: 14px; color: #4A5568; border: none; background: transparent; }

        /* 左右两个面板的基础样式 */
        #infoPanel, #paymentPanel {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
        }
        #panelTitle {
            font-size: 18px;
            font-weight: bold;
            color: #2D3748;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 1px solid #EDF2F7;
        }

        /* 订单信息部分的样式 */
        QFormLayout > QLabel { font-size: 14px; color: #4A5568; }
        #totalAmountBox {
            background-color: #F7FAFC;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 15px;
            margin-top: 20px;
        }
        #totalText { font-size: 16px; font-weight: bold; color: #2D3748; margin-right: 15px; }
        #totalAmountValue { font-size: 20px; font-weight: bold; color: #E53E3E; } /* 红色 */

        /* 支付方式部分的样式 */
        #paymentEntry {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 15px 20px;
            margin-bottom: 10px;
        }
        #paymentEntry:hover {
            border-color: #3182CE; /* 鼠标悬停蓝色边框 */
        }
        QRadioButton::indicator { /* 隐藏默认radio按钮指示器 */
            width: 18px;
            height: 18px;
            border-radius: 9px;
            border: 2px solid #CBD5E0;
            background: white;
            margin-right: 8px;
        }
        QRadioButton::indicator:checked {
            border: 6px solid #3182CE;   /* 视觉上是个实心圆 */
        }
        QRadioButton::indicator:unchecked {
            border: 2px solid #CBD5E0;
        }
        QRadioButton { /* 占用空间 */
            width: 20px;
            height: 20px;
            margin-left: 10px;
        }
        QRadioButton::checked { /* 自定义选中状态 */
            border: 6px solid #3182CE; /* 蓝色实心圆 */
            border-radius: 10px;
            background-color: white;
            min-width: 14px; /* 确保可见 */
            min-height: 14px;
        }
        QRadioButton::!checked { /* 自定义未选中状态 */
            border: 2px solid #CBD5E0; /* 灰色空心圆 */
            border-radius: 10px;
            background-color: white;
            min-width: 18px; /* 确保可见 */
            min-height: 18px;
        }
        #wechatIcon {
            background-image: url(:/icons/wechat_pay_icon.png); /* 假设有图标资源 */
            background-repeat: no-repeat;
            background-position: center;
            background-size: contain;
            /* 模拟图标 */
            background-color: #4CAF50; /* 绿色 */
            border-radius: 4px;
        }
        #alipayIcon {
            background-image: url(:/icons/alipay_icon.png); /* 假设有图标资源 */
            background-repeat: no-repeat;
            background-position: center;
            background-size: contain;
            /* 模拟图标 */
            background-color: #2196F3; /* 蓝色 */
            border-radius: 4px;
        }
        #paymentMethodTitle { font-size: 16px; font-weight: bold; color: #2D3748; }
        #paymentMethodHint { font-size: 12px; color: #718096; }

        #confirmPaymentButton {
            background-color: #48BB78;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 15px 25px;
            font-size: 18px;
            font-weight: bold;
            margin-top: 20px;
        }
        #confirmPaymentButton:hover { background-color: #38A169; }
        #confirmPaymentButton:disabled { background-color: #A0AEC0; } /* 禁用状态 */

        /* 加载动画覆盖层 */
        #loadingOverlay {
            background-color: rgba(255, 255, 255, 0.8); /* 半透明白色 */
            border-radius: 12px; /* 与面板一致 */
        }
        #loadingSpinner {
            /* 模拟旋转动画，实际可能需要更复杂的QPropertyAnimation或第三方库 */
            background-color: #3182CE; /* 蓝色圆点 */
            border-radius: 25px; /* 变为圆形 */
            /* 简单的旋转效果可以靠QPropertyAnimation实现 */
            /* 对于QSS，只能模拟一个静态圆，如果需要旋转，需要代码 */
        }
        #loadingText {
            font-size: 16px;
            color: #4A5568;
            margin-top: 15px;
        }
    )");
}
