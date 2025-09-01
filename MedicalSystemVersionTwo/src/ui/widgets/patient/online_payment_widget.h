#ifndef ONLINE_PAYMENT_WIDGET_H
#define ONLINE_PAYMENT_WIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>

// 前置声明
class QLabel;
class QRadioButton;
class QPushButton;
class QTimer;
class QPropertyAnimation; // 用于模拟加载动画

// 订单详情结构体 (模拟后端数据)
struct OrderInfo {
    QString orderId;
    QString createTime;
    QString patientName;
    QString department;
    QString doctorName;
    QMap<QString, double> feeDetails; // 费用明细，例如 {"挂号费", 25.00}
    double discount;
    double totalAmount;
};

class OnlinePaymentWidget : public QWidget
{
Q_OBJECT

public:
    explicit OnlinePaymentWidget(QWidget *parent = nullptr);
    ~OnlinePaymentWidget();

signals:
    void backRequested(); // 返回首页
    void paymentCompleted(); // 支付完成（成功或失败）

private slots:
    void onConfirmPaymentClicked(); // 确认支付按钮点击
    void onPaymentProcessFinished(); // 支付处理完成（模拟3秒后）
    void updatePaymentStatusText(); // 更新支付处理中的文字
protected:
    // 重写 QWidget 的 resizeEvent 事件，以便在窗口大小变化时更新加载动画遮罩的大小
    void resizeEvent(QResizeEvent *event) override;
private:
    void initUI();
    void initStyleSheets();
    QWidget* createHeader();
    QWidget* createOrderInfoPanel(const OrderInfo &order);
    QWidget* createPaymentMethodPanel();

    // --- 后端交互 (伪代码) ---
    void loadOrderDetails(); // 加载订单详情
    void processPayment(int paymentMethod); // 处理支付请求

    // UI 控件成员
    QLabel *orderIdLabel;
    QLabel *createTimeLabel;
    QLabel *patientNameLabel;
    QLabel *departmentLabel;
    QLabel *doctorNameLabel;

    QLabel *registrationFeeLabel;
    QLabel *consultationFeeLabel;
    QLabel *medicineFeeLabel;
    QLabel *examinationFeeLabel;
    QLabel *discountLabel;
    QLabel *totalAmountLabel;

    QRadioButton *wechatPayRadio;
    QRadioButton *alipayRadio;
    QPushButton *confirmPaymentButton;

    // 加载动画相关
    QWidget *loadingOverlay; // 半透明遮罩
    QLabel *loadingSpinner; // 旋转动画
    QLabel *loadingText;    // “支付处理中”文字
    QTimer *paymentProcessTimer; // 模拟支付处理时间
    int loadingTextDotCount; // 用于动态显示点点点

    OrderInfo currentOrder; // 当前订单数据
};

#endif // ONLINE_PAYMENT_WIDGET_H