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
    //
    void loadDoctorContactsForPatient(int patientId);
        void loadDoctorContactsForDoctor(int doctorId);

        // 历史
        void loadChatHistoryByConversation(int conversationId, int limit=200);
        void loadChatHistoryByPeer(int patientId, int doctorId, int limit=200);

        // 发送
        void sendChatMessageByConversation(int conversationId, const QString &senderType, const QString &content);
        void sendChatMessageByPeer(int patientId, int doctorId, const QString &senderType, const QString &content);
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
    void loadCurrentDoctorDataByAccount(int accountId);
        void loadCurrentDoctorDataByDoctorId(int doctorId);
        void sendUpdateDoctorProfile(const QJsonObject &patch);
    void loadMedicineData();
    void loadPatientList();
    void loadMedicalRecord(int patientId);
    void saveMedicalRecord(const QJsonObject &record); // 需含 patient_id (可选 record_id)
    void loadMedicalOrders(int patientId, int orderId = 0);
    void saveMedicalOrders(const QJsonObject &order);  // 需含 patient_id, doctor_id, items[]
    void loadAttendanceToday(int doctorId);
    void clockEvent(int doctorId, const QString &kind /*"in"/"out"*/);
    void submitLeave(int doctorId, const QString &leaveType,
                     const QString &startDate, const QString &endDate, const QString &reason);
    void loadLeaveRecords(int doctorId);
signals:
    // aaaa
    void loadPatientListOk(const QJsonArray &patients);
    void loadPatientListFailed(const QString &msg);

    void medicalRecordLoaded(const QJsonObject &record, bool found);
    void medicalRecordSaved(bool ok, int recordId, const QString &msg);

    void medicalOrdersLoaded(const QJsonObject &order, const QJsonArray &items, bool found);
    void medicalOrdersSaved(bool ok, int orderId, const QString &msg);

    void attendanceTodayLoaded(const QJsonObject &payload);
    void clockEventDone(const QJsonObject &payload);

    void leaveSubmitted(const QJsonObject &payload);
    void leaveRecordsLoaded(const QJsonArray &records);
    // aaaa
    //
    void doctorContactsLoadedOk(const QJsonArray &contacts, const QString &side);
        void doctorContactsLoadedFailed(const QString &msg);

        void chatHistoryLoadedOk(const QJsonArray &messages, int conversationId);
        void chatHistoryLoadedFailed(const QString &msg);

        void messageSentOk(const QJsonObject &message, int conversationId);
        void messageSentFailed(const QString &msg);
    //
    void loginSucceeded(const QString &msg);
    void loginSucceededDetail(int id, const QString &name, const QString &role);
    void loginFailed(const QString &msg);
    void registerSucceeded(const QString &msg);
    void registerFailed(const QString &msg);
    void patientProfileLoaded(const QJsonObject &profile);
    void patientProfileSaved(bool ok, const QString &msg);
 // shang mian ok
    void currentDataDoctorOk(const QJsonObject &doctor);
        void currentDataDoctorFailed(const QString &msg);
        void updateDoctorProfileDone(bool ok, const QString &msg);
    // xia mian ok
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

    QString serverIp   = "192.168.253.147"; // 根据实际情况
    quint16 serverPort = 9999;
};
#endif // WIDGET_H
