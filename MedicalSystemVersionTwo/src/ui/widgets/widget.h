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

    // 给 LoginDialog 连接的槽
    void sendLoginData(const QString &username,
                       const QString &password,
                       const QString &roleText);

    void sendRegisterData(const QString &username,
                          const QString &email,
                          const QString &password,
                          const QString &userType,
                          const QString &department);
    // === 发请求 ===
    void sendListDepartments();
    void sendListDoctors(int departmentId=-1, const QString &keyword=QString(),
                         int isOnline=-1, int limit=20, int offset=0,
                         const QString &orderBy="doctor_id", const QString &sort="ASC");
    void sendListAvailableSlots(int doctorId=-1, const QString &start=QString(),
                                const QString &end=QString());
    void sendBookAppointment(int patientId, int doctorId, int slotId, const QString &desc);

    void sendGetPatientProfile(int patientId);
    void sendUpdatePatientProfile(const QJsonObject &profilePatch); // 包含 patient_id

    void sendGetDoctorContacts(int patientId);
    void sendGetChatHistory(int patientId, int doctorId);
    void sendSendMessage(int patientId, int doctorId, const QString &content);

    void sendGetHealthQuestions();
    void sendSubmitHealthAssessment(int patientId, const QJsonArray &answers); // [{question_id, option_id, score}]

    void sendSearchMedicines(const QString &keyword, const QString &typeFilter, int isPrescription=-1);

    void sendGetOrderDetail(int orderId);
    void sendCreatePayment(int orderId, const QString &method, double amount);

    void sendListPatientAppointments(int patientId);
    void sendCancelAppointment(int appointmentId);
    void sendEnsurePendingOrder(int patientId);
    void sendAddMedicineToOrder(int orderId, int medicineId, int qty);
    void sendAddMedicineToOrderByName(int orderId, const QString &medicineName, int qty); // 方便用 name
signals:
    void ensurePendingOrderReady(int orderId);
    void orderItemAdded(bool ok, const QString &msg, const QJsonObject &order);
    void loginSucceeded(const QString &msg);
    void loginSucceededDetail(int id, const QString &name, const QString &role);
    void loginFailed(const QString &msg);
    void registerSucceeded(const QString &msg);
    void registerFailed(const QString &msg);

    // 科室/医生/号源/预约
    void departmentsListed(const QJsonArray &items);
    void doctorsListed(int total, const QJsonArray &items);
    void availableSlotsListed(const QJsonArray &items);
    void appointmentBooked(bool ok, const QJsonObject &appointment, const QString &msg);

    // 患者档案
    void patientProfileLoaded(const QJsonObject &profile);
    void patientProfileSaved(bool ok, const QString &msg);

    // 即时沟通
    void doctorContactsLoaded(const QJsonArray &items);
    void chatHistoryLoaded(int doctorId, const QJsonArray &messages);
    void messageSent(bool ok, const QString &msg);

    // 健康评估
    void healthQuestionsLoaded(const QJsonArray &questions, const QJsonArray &options);
    void healthAssessmentDone(const QJsonObject &result);

    // 药品
    void medicinesLoaded(const QJsonArray &items);

    // 支付/订单
    void orderDetailLoaded(const QJsonObject &order);
    void paymentProcessed(bool ok, const QString &msg, const QJsonObject &orderIfAny);

    // 预约列表/取消
    void patientAppointmentsLoaded(const QJsonArray &items);
    void appointmentCanceled(bool ok, const QString &msg);

private slots:
    void slotConnected();
    void slotReadyRead();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError error);

private:
    void init();
    void ensureConnected();
    void sendJson(const QJsonObject &obj);

private:
    QTcpSocket *myTcpClient = nullptr;
    bool connectStatus = false;

    QString serverIp   = "192.168.253.140"; // 根据实际情况
    quint16 serverPort = 9999;
};
#endif // WIDGET_H
