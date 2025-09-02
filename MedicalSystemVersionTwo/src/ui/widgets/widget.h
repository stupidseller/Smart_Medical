#ifndef WIDGET_H
#define WIDGET_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>

class Widget : public QObject
{
    Q_OBJECT

public:
    explicit Widget(QObject *parent = nullptr);
    ~Widget();

    // 连接
    void on_connectServerBtn_clicked();
    void sendLoginData(const QString &username,
                       const QString &password,
                       const QString &roleText);
    void sendRegisterData(const QString &username,
                          const QString &email,
                          const QString &password,
                          const QString &userType,
                          const QString &department);   
    void sendGetPatientProfile(int patientId);
    void sendUpdatePatientProfile(const QJsonObject &profilePatch); // 包含 patient_id
    // shang mian bu yao dong , yijing ok

    void loadMedicineData();
signals:
    void loginSucceeded(const QString &msg);
    void loginSucceededDetail(int id, const QString &name, const QString &role);
    void loginFailed(const QString &msg);
    void registerSucceeded(const QString &msg);
    void registerFailed(const QString &msg);
    void patientProfileLoaded(const QJsonObject &profile);
    void patientProfileSaved(bool ok, const QString &msg);
 // shang mian ok
    void loadMedicineDataOk(const QJsonArray &medicines);
    void loadOrderDetailsOk(const QJsonArray &orders);
    void loadAvailableDoctorsOk(const QJsonArray &doctors);
    void submitAppointmentRequestOk(const QJsonObject &resp);
    void loadDoctorListOk(const QJsonArray &doctors);
    void onPurchaseClickedOk(const QJsonObject &resp);
    void processPaymentOk(const QJsonObject &resp);
    //
    void addToCartOk(const QJsonObject &order);
private slots:
    void slotConnected();
    void slotReadyRead();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError error);
    // shang mian ok

public slots:
    void loadAvailableDoctors(); //
    void addToCart(const QJsonArray &cart, int orderId = 0);   // ★ 新增
    void submitAppointmentRequest(int patientId, int doctorId, int slotId, const QString &desc);
    void loadDoctorList();
    void onPurchaseClicked(int patientId, const QJsonArray &cart, int orderId = 0);
    void processPayment(int orderId, const QString &method, double amount,
                            const QString &status = QString("success"),
                            const QString &txref  = QString());
    void loadOrderDetails(int orderId = -1);
private:
    void init();
    void ensureConnected();
    void sendJson(const QJsonObject &obj);

private:
    QJsonObject m_cartOrder;
    QTcpSocket *myTcpClient = nullptr;
    bool connectStatus = false;

    QString serverIp   = "192.168.253.145"; // 根据实际情况
    quint16 serverPort = 9999;
};
#endif // WIDGET_H
