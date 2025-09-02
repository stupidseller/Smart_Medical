#ifndef ONLINE_PAYMENT_WIDGET_H
#define ONLINE_PAYMENT_WIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QJsonObject>

class QLabel;
class QRadioButton;
class QPushButton;
class QTimer;
class QVBoxLayout;

// ★ 前置声明“API”类（你的服务器端窗口类）
class Widget;

struct OrderInfo {
    QString orderId;
    QString createTime;
    QString patientName;
    QString department;
    QString doctorName;
    QMap<QString,double> feeDetails;
    double discount = 0.0;
    double totalAmount = 0.0;
};

class OnlinePaymentWidget : public QWidget
{
    Q_OBJECT                         // ★ 必须有！
public:
    explicit OnlinePaymentWidget(QWidget *parent = nullptr);
    explicit OnlinePaymentWidget(Widget *api, int patientId, QWidget *parent = nullptr);
    ~OnlinePaymentWidget();
    // xiamian xinde
    void loadOrderDetails();
    void doProcessPayment(int orderId, const QString &method, double amount,
                              const QString &status = QString("success"),
                              const QString &txref  = QString());
signals:
    void backRequested();
    void paymentCompleted();
    //
    void requestLoadOrderDetails();
    void processPayment(int orderId, const QString &method, double amount,
                           const QString &status, const QString &txref);

public slots:
    void onLoadOrderDetailsOk(const QJsonArray &orders); //
    void onProcessPaymentOk(const QJsonObject &resp);
private slots:
    void onConfirmPaymentClicked();
    void onPaymentProcessFinished();
    void updatePaymentStatusText();
    void onEnsureOrderReady(int orderId);
    void onOrderDetailLoaded(const QJsonObject &order);
    void onPaymentProcessed(bool ok, const QString &msg, const QJsonObject &payload);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void initUI();
    void initStyleSheets();
    QWidget* createHeader();
    QWidget* createOrderInfoPanel(const OrderInfo &order);
    QWidget* createPaymentMethodPanel();
public:
    OrderInfo currentOrder;
    void setPaymentMethod(int way);
    void refreshUi();
private:
    // —— UI 成员 ——
    QLabel *orderIdLabel = nullptr;
    QLabel *createTimeLabel = nullptr;
    QLabel *patientNameLabel = nullptr;
    QLabel *departmentLabel = nullptr;
    QLabel *doctorNameLabel = nullptr;

    QLabel *registrationFeeLabel = nullptr;
    QLabel *consultationFeeLabel = nullptr;
    QLabel *medicineFeeLabel = nullptr;
    QLabel *examinationFeeLabel = nullptr;
    QLabel *discountLabel = nullptr;
    QLabel *totalAmountLabel = nullptr;

    QRadioButton *wechatPayRadio = nullptr;
    QRadioButton *alipayRadio = nullptr;
    QPushButton *confirmPaymentButton = nullptr;

    QWidget *loadingOverlay = nullptr;
    QLabel *loadingSpinner = nullptr;
    QLabel *loadingText = nullptr;
    QTimer *paymentProcessTimer = nullptr;
    int loadingTextDotCount = 0;

    QVBoxLayout *itemsListLayout = nullptr;


    // —— 后端相关 ——
    Widget *m_api = nullptr;
    int m_patientId = -1;
    int m_orderId = -1;
};

#endif // ONLINE_PAYMENT_WIDGET_H
