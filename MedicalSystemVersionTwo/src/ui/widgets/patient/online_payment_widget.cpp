#include "online_payment_widget.h"
#include "payment_success_dialog.h" // 包含支付成功弹窗
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QButtonGroup>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QPropertyAnimation> // 用于更精细的动画控制

OnlinePaymentWidget::OnlinePaymentWidget(QWidget *parent)
        : QWidget(parent),
          orderIdLabel(nullptr), createTimeLabel(nullptr), patientNameLabel(nullptr),
          departmentLabel(nullptr), doctorNameLabel(nullptr),
          registrationFeeLabel(nullptr), consultationFeeLabel(nullptr),
          medicineFeeLabel(nullptr), examinationFeeLabel(nullptr),
          discountLabel(nullptr), totalAmountLabel(nullptr),
          wechatPayRadio(nullptr), alipayRadio(nullptr), confirmPaymentButton(nullptr),
          loadingOverlay(nullptr), loadingSpinner(nullptr), loadingText(nullptr),
          paymentProcessTimer(nullptr), loadingTextDotCount(0)
{
    setObjectName("OnlinePaymentWidget");
    initUI();
    initStyleSheets();
    loadOrderDetails(); // 默认加载一个模拟订单
}

OnlinePaymentWidget::~OnlinePaymentWidget() {}

void OnlinePaymentWidget::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 整体无边距
    mainLayout->setSpacing(0);

    // 顶部全局标题栏
    QWidget* globalHeader = new QWidget();
    globalHeader->setObjectName("globalHeader");
    QHBoxLayout* globalHeaderLayout = new QHBoxLayout(globalHeader);
    QPushButton *backButton = new QPushButton("返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &OnlinePaymentWidget::backRequested);
    QLabel *title = new QLabel("线上支付");
    title->setObjectName("pageTitle");
    // ... 此处可以添加右侧的用户头像和名字 ...
    globalHeaderLayout->addWidget(backButton);
    globalHeaderLayout->addSpacing(20);
    globalHeaderLayout->addWidget(title);
    globalHeaderLayout->addStretch();
    mainLayout->addWidget(globalHeader);

    // 内容区域
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(40, 20, 40, 40);
    contentLayout->setSpacing(30);

    // 订单信息面板（暂时传入空OrderInfo，将在loadOrderDetails中填充）
    contentLayout->addWidget(createOrderInfoPanel(currentOrder), 1);
    contentLayout->addWidget(createPaymentMethodPanel(), 1);

    mainLayout->addLayout(contentLayout);

    // 初始化加载动画（默认为隐藏）
    loadingOverlay = new QWidget(this);
    loadingOverlay->setObjectName("loadingOverlay");
    loadingOverlay->hide(); // 默认隐藏
    QVBoxLayout *loadingLayout = new QVBoxLayout(loadingOverlay);
    loadingLayout->setAlignment(Qt::AlignCenter);

    loadingSpinner = new QLabel();
    loadingSpinner->setObjectName("loadingSpinner");
    loadingSpinner->setFixedSize(50, 50); // 旋转圆圈大小
    loadingLayout->addWidget(loadingSpinner, 0, Qt::AlignCenter);

    loadingText = new QLabel("支付处理中，请稍候...");
    loadingText->setObjectName("loadingText");
    loadingLayout->addWidget(loadingText, 0, Qt::AlignCenter);

    // 使loadingOverlay覆盖整个widget
    loadingOverlay->setGeometry(this->rect());
    loadingOverlay->raise(); // 确保在最顶层

    paymentProcessTimer = new QTimer(this);
    // 每500ms更新一次点点点
    connect(paymentProcessTimer, &QTimer::timeout, this, &OnlinePaymentWidget::updatePaymentStatusText);
}

// 模拟窗口大小变化时更新遮罩大小
void OnlinePaymentWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (loadingOverlay) {
        loadingOverlay->setGeometry(this->rect());
    }
}

// --- 后端交互函数 (伪代码实现) ---
void OnlinePaymentWidget::loadOrderDetails() {
    //
    // 后端交互函数：加载待支付订单的详细信息
    // API端点建议: GET /api/patient/orders/pending
    //

    // --- 前端伪代码 ---
    // 模拟订单数据
    currentOrder.orderId = "PO20230830123456";
    currentOrder.createTime = "2025-08-30 21:57:50";
    currentOrder.patientName = "张患者";
    currentOrder.department = "心血管内科";
    currentOrder.doctorName = "李医生";
    currentOrder.feeDetails["挂号费"] = 25.00;
    currentOrder.feeDetails["诊查费"] = 50.00;
    currentOrder.feeDetails["药品费"] = 126.50;
    currentOrder.feeDetails["检查费"] = 180.00;
    currentOrder.discount = -15.00;
    currentOrder.totalAmount = 366.50; // 25+50+126.5+180-15 = 366.5

    orderIdLabel->setText(currentOrder.orderId);
    createTimeLabel->setText(currentOrder.createTime);
    patientNameLabel->setText(currentOrder.patientName);
    departmentLabel->setText(currentOrder.department);
    doctorNameLabel->setText(currentOrder.doctorName);

    registrationFeeLabel->setText(QString("¥%1").arg(currentOrder.feeDetails.value("挂号费", 0.00), 0, 'f', 2));
    consultationFeeLabel->setText(QString("¥%1").arg(currentOrder.feeDetails.value("诊查费", 0.00), 0, 'f', 2));
    medicineFeeLabel->setText(QString("¥%1").arg(currentOrder.feeDetails.value("药品费", 0.00), 0, 'f', 2));
    examinationFeeLabel->setText(QString("¥%1").arg(currentOrder.feeDetails.value("检查费", 0.00), 0, 'f', 2));
    discountLabel->setText(QString("¥%1").arg(currentOrder.discount, 0, 'f', 2));
    totalAmountLabel->setText(QString("¥%1").arg(currentOrder.totalAmount, 0, 'f', 2));

    confirmPaymentButton->setText(QString("确认支付 ¥%1").arg(currentOrder.totalAmount, 0, 'f', 2));
}

void OnlinePaymentWidget::processPayment(int paymentMethod) {
    //
    // 后端交互函数：向后端发送支付请求
    // API端点建议: POST /api/patient/payment
    // 请求体 (JSON):
    // {
    //   "orderId": "PO20230830123456",
    //   "paymentMethod": "WeChatPay" // 或 "Alipay"
    // }
    //

    // --- 前端伪代码 ---
    qDebug() << "正在处理支付请求，支付方式：" << (paymentMethod == 0 ? "微信支付" : "支付宝");
    // 模拟支付成功
    QTimer::singleShot(3000, this, &OnlinePaymentWidget::onPaymentProcessFinished);
}

// --- UI 构建函数实现 ---
QWidget* OnlinePaymentWidget::createHeader() {
    // 这个在initUI()中已经创建，这里是占位符
    return nullptr;
}

QWidget* OnlinePaymentWidget::createOrderInfoPanel(const OrderInfo &order) {
    QFrame *panel = new QFrame();
    panel->setObjectName("infoPanel");
    QVBoxLayout *mainLayout = new QVBoxLayout(panel);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    // 订单信息
    QLabel *orderTitle = new QLabel("订单信息");
    orderTitle->setObjectName("panelTitle");
    mainLayout->addWidget(orderTitle);

    QFormLayout *orderForm = new QFormLayout();
    orderForm->setContentsMargins(0, 0, 0, 0);
    orderForm->setSpacing(10);
    orderForm->setLabelAlignment(Qt::AlignLeft);
    orderForm->addRow("订单号", orderIdLabel = new QLabel(order.orderId));
    orderForm->addRow("创建时间", createTimeLabel = new QLabel(order.createTime));
    orderForm->addRow("患者姓名", patientNameLabel = new QLabel(order.patientName));
    orderForm->addRow("就诊科室", departmentLabel = new QLabel(order.department));
    orderForm->addRow("医生", doctorNameLabel = new QLabel(order.doctorName));
    mainLayout->addLayout(orderForm);

    // 费用明细
    QLabel *feeTitle = new QLabel("费用明细");
    feeTitle->setObjectName("panelTitle");
    mainLayout->addWidget(feeTitle);

    QFormLayout *feeForm = new QFormLayout();
    feeForm->setContentsMargins(0, 0, 0, 0);
    feeForm->setSpacing(10);
    feeForm->setLabelAlignment(Qt::AlignLeft);
    feeForm->addRow("挂号费", registrationFeeLabel = new QLabel(QString("¥%1").arg(order.feeDetails.value("挂号费", 0.00), 0, 'f', 2)));
    feeForm->addRow("诊查费", consultationFeeLabel = new QLabel(QString("¥%1").arg(order.feeDetails.value("诊查费", 0.00), 0, 'f', 2)));
    feeForm->addRow("药品费", medicineFeeLabel = new QLabel(QString("¥%1").arg(order.feeDetails.value("药品费", 0.00), 0, 'f', 2)));
    feeForm->addRow("检查费", examinationFeeLabel = new QLabel(QString("¥%1").arg(order.feeDetails.value("检查费", 0.00), 0, 'f', 2)));
    feeForm->addRow("优惠折扣", discountLabel = new QLabel(QString("¥%1").arg(order.discount, 0, 'f', 2)));
    mainLayout->addLayout(feeForm);

    // 总计
    QLabel *totalAmountBox = new QLabel();
    totalAmountBox->setObjectName("totalAmountBox");
    QHBoxLayout *totalLayout = new QHBoxLayout(totalAmountBox);
    totalLayout->addStretch();
    QLabel *totalText = new QLabel("应付总额");
    totalText->setObjectName("totalText");
    totalAmountLabel = new QLabel(QString("¥%1").arg(order.totalAmount, 0, 'f', 2));
    totalAmountLabel->setObjectName("totalAmountValue");
    totalLayout->addWidget(totalText);
    totalLayout->addWidget(totalAmountLabel);
    totalLayout->addStretch(); // 使其居中

    mainLayout->addStretch(); // 推到顶部
    mainLayout->addWidget(totalAmountBox);

    return panel;
}

QWidget* OnlinePaymentWidget::createPaymentMethodPanel() {
    QFrame *panel = new QFrame();
    panel->setObjectName("paymentPanel");
    QVBoxLayout *mainLayout = new QVBoxLayout(panel);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    QLabel *payTitle = new QLabel("选择支付方式");
    payTitle->setObjectName("panelTitle");
    mainLayout->addWidget(payTitle);

    QButtonGroup *payGroup = new QButtonGroup(this);
    payGroup->setExclusive(true); // 确保只有一个被选中

    // 微信支付
    QWidget *wechatEntry = new QWidget();
    wechatEntry->setObjectName("paymentEntry");
    QHBoxLayout *wechatLayout = new QHBoxLayout(wechatEntry);
    QLabel *wechatIcon = new QLabel(); // 使用QSS设置背景图
    wechatIcon->setObjectName("wechatIcon");
    wechatIcon->setFixedSize(32, 32);
    QVBoxLayout *wechatTextLayout = new QVBoxLayout();
    QLabel *wechatTitle = new QLabel("微信支付");
    wechatTitle->setObjectName("paymentMethodTitle");
    QLabel *wechatHint = new QLabel("推荐已安装微信的用户使用");
    wechatHint->setObjectName("paymentMethodHint");
    wechatTextLayout->addWidget(wechatTitle);
    wechatTextLayout->addWidget(wechatHint);
    wechatPayRadio = new QRadioButton();
    wechatPayRadio->setChecked(true); // 默认选中微信
    wechatLayout->addWidget(wechatIcon);
    wechatLayout->addLayout(wechatTextLayout);
    wechatLayout->addStretch();
    wechatLayout->addWidget(wechatPayRadio);
    mainLayout->addWidget(wechatEntry);
    payGroup->addButton(wechatPayRadio, 0); // 0代表微信

    // 支付宝支付
    QWidget *alipayEntry = new QWidget();
    alipayEntry->setObjectName("paymentEntry");
    QHBoxLayout *alipayLayout = new QHBoxLayout(alipayEntry);
    QLabel *alipayIcon = new QLabel(); // 使用QSS设置背景图
    alipayIcon->setObjectName("alipayIcon");
    alipayIcon->setFixedSize(32, 32);
    QVBoxLayout *alipayTextLayout = new QVBoxLayout();
    QLabel *alipayTitle = new QLabel("支付宝支付");
    alipayTitle->setObjectName("paymentMethodTitle");
    QLabel *alipayHint = new QLabel("推荐已安装支付宝的用户使用");
    alipayHint->setObjectName("paymentMethodHint");
    alipayTextLayout->addWidget(alipayTitle);
    alipayTextLayout->addWidget(alipayHint);
    alipayRadio = new QRadioButton();
    alipayLayout->addWidget(alipayIcon);
    alipayLayout->addLayout(alipayTextLayout);
    alipayLayout->addStretch();
    alipayLayout->addWidget(alipayRadio);
    mainLayout->addWidget(alipayEntry);
    payGroup->addButton(alipayRadio, 1); // 1代表支付宝

    mainLayout->addStretch(); // 推到顶部

    // 确认支付按钮
    confirmPaymentButton = new QPushButton("确认支付 ¥366.50");
    confirmPaymentButton->setObjectName("confirmPaymentButton");
    mainLayout->addWidget(confirmPaymentButton);
    connect(confirmPaymentButton, &QPushButton::clicked, this, &OnlinePaymentWidget::onConfirmPaymentClicked);

    return panel;
}


// --- 槽函数实现 ---
void OnlinePaymentWidget::onConfirmPaymentClicked() {
    // 显示加载动画
    loadingOverlay->show();
    confirmPaymentButton->setEnabled(false); // 禁用按钮防止重复点击
    loadingText->setText("支付处理中，请稍候."); // 初始化文本
    loadingTextDotCount = 0;
    paymentProcessTimer->start(500); // 启动定时器，每500ms更新一次点

    int selectedMethod = -1;
    if (wechatPayRadio->isChecked()) {
        selectedMethod = 0; // 微信
    } else if (alipayRadio->isChecked()) {
        selectedMethod = 1; // 支付宝
    }

    if (selectedMethod != -1) {
        processPayment(selectedMethod); // 调用模拟支付处理
    } else {
        QMessageBox::warning(this, "提示", "请选择一种支付方式！");
        loadingOverlay->hide();
        confirmPaymentButton->setEnabled(true);
        paymentProcessTimer->stop();
    }
}

void OnlinePaymentWidget::updatePaymentStatusText() {
    loadingTextDotCount = (loadingTextDotCount + 1) % 4; // 0, 1, 2, 3
    QString dots(loadingTextDotCount, '.');
    loadingText->setText(QString("支付处理中，请稍候%1").arg(dots));
}


void OnlinePaymentWidget::onPaymentProcessFinished() {
    loadingOverlay->hide(); // 隐藏加载动画
    confirmPaymentButton->setEnabled(true); // 重新启用按钮
    paymentProcessTimer->stop(); // 停止点点点更新

    // 显示支付成功对话框
    PaymentSuccessDialog successDialog(currentOrder.totalAmount, this);
    connect(&successDialog, &PaymentSuccessDialog::backToDashboardClicked, this, &OnlinePaymentWidget::backRequested);
    // connect(&successDialog, &PaymentSuccessDialog::viewOrderClicked, ...); // 如果有查看订单页面的话

    successDialog.exec(); // 模态显示对话框
    emit paymentCompleted(); // 发出支付完成信号，可能用于刷新父窗口状态
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
            width: 0px;
            height: 0px;
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