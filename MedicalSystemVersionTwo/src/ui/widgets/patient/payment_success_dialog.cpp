#include "payment_success_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <QPainter> // For custom painting if needed, but for simple icon, QLabel is enough

PaymentSuccessDialog::PaymentSuccessDialog(double paymentAmount, QWidget *parent)
        : QDialog(parent)
{
    setObjectName("PaymentSuccessDialog");
    setModal(true); // 设置为模态对话框
    setFixedSize(400, 300); // 固定大小
    setWindowTitle("支付成功"); // 隐藏标题栏更好看
    // setWindowFlags(Qt::FramelessWindowHint); // 如果需要隐藏标题栏，这里取消注释

    initUI(paymentAmount);
    initStyleSheets();
}

PaymentSuccessDialog::~PaymentSuccessDialog() {}

void PaymentSuccessDialog::initUI(double paymentAmount) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 顶部成功的图标
    QLabel *iconLabel = new QLabel();
    // 使用占位符，实际中可以加载SVG或PNG图标
    // 为了复刻图片，我们直接用QSS画圆和对勾
    iconLabel->setObjectName("successIcon");
    iconLabel->setFixedSize(60, 60); // 图标大小
    iconLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignCenter);

    // 支付成功文本
    QLabel *statusLabel = new QLabel("支付成功!");
    statusLabel->setObjectName("statusLabel");
    mainLayout->addWidget(statusLabel, 0, Qt::AlignCenter);

    // 支付金额信息
    QLabel *infoLabel = new QLabel(QString("您的订单已支付成功，支付金额: ¥%1").arg(paymentAmount, 0, 'f', 2));
    infoLabel->setObjectName("infoLabel");
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel, 0, Qt::AlignCenter);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    buttonLayout->setAlignment(Qt::AlignCenter);

    QPushButton *viewOrderButton = new QPushButton("查看订单");
    viewOrderButton->setObjectName("viewOrderButton");
    connect(viewOrderButton, &QPushButton::clicked, this, &PaymentSuccessDialog::onViewOrder);

    QPushButton *backToDashboardButton = new QPushButton("返回首页");
    backToDashboardButton->setObjectName("backToDashboardButton");
    connect(backToDashboardButton, &QPushButton::clicked, this, &PaymentSuccessDialog::onBackToDashboard);

    buttonLayout->addWidget(viewOrderButton);
    buttonLayout->addWidget(backToDashboardButton);
    mainLayout->addLayout(buttonLayout);
}

void PaymentSuccessDialog::onViewOrder() {
    emit viewOrderClicked();
    accept(); // 关闭对话框
}

void PaymentSuccessDialog::onBackToDashboard() {
    emit backToDashboardClicked();
    accept(); // 关闭对话框
}

void PaymentSuccessDialog::initStyleSheets() {
    this->setStyleSheet(R"(
        #PaymentSuccessDialog {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
        }
        #successIcon {
            background-color: #48BB78; /* 绿色背景 */
            border-radius: 30px; /* 圆形 */
            /* 使用QPainter绘制对勾，或者直接用带对勾的SVG/PNG图片 */
            /* 为了简单复刻，我们用一个背景色和边框，实际对勾需要图片或重绘 */
            /* 假定背景色就代表了图标 */
        }
        #statusLabel {
            font-size: 20px;
            font-weight: bold;
            color: #2D3748;
        }
        #infoLabel {
            font-size: 14px;
            color: #4A5568;
            margin-bottom: 10px;
        }
        #viewOrderButton, #backToDashboardButton {
            padding: 10px 20px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
        }
        #viewOrderButton {
            background-color: #48BB78; /* 绿色 */
            color: white;
            border: none;
        }
        #viewOrderButton:hover {
            background-color: #38A169;
        }
        #backToDashboardButton {
            background-color: #F7FAFC; /* 浅灰色 */
            color: #4A5568;
            border: 1px solid #E2E8F0;
        }
        #backToDashboardButton:hover {
            background-color: #EDF2F7;
        }
    )");
}