#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QMap>
#include <QJsonObject>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <algorithm>   // 你用了 std::max
#include <QVariant>    // 你用到了 QVariant(...) 绑定可空值（可选，但建议）

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_startServerBtn_clicked();
    void slotNewConnection();
    void slotReadyRead();
    void slotAcceptError(QAbstractSocket::SocketError error);
    void slotDisconnected();
    void on_clientSendBtn_clicked();
    void slotConnected();

private:
    // --- state ---
    Ui::Widget *ui;
    QTcpServer *myTcpServer = nullptr;
    QTcpSocket *myTcpClient = nullptr;
    QList<QTcpSocket *> clients;
    QMap<QTcpSocket*, QByteArray> recvBuf;

    void init();
    //
    void handledoctorContactsLoaded(QTcpSocket *sock, const QJsonObject &obj);
        void handlechatHistoryLoaded(QTcpSocket *sock, const QJsonObject &obj);
        void handlemessageSent(QTcpSocket *sock, const QJsonObject &obj);
    // --- dispatcher & handlers ---
    void handleMessage(QTcpSocket *sock, const QJsonObject &obj);
    void handleLogin(QTcpSocket *sock, const QJsonObject &obj);
    void handleRegister(QTcpSocket *sock, const QJsonObject &obj);
    void handleGetPatientProfile(QTcpSocket *sock, const QJsonObject &obj);
    void handleUpdatePatientProfile(QTcpSocket *sock, const QJsonObject &obj);
    // bu yao dong shang mian de yijing ok
    void handlecurrentDataDoctor(QTcpSocket *sock, const QJsonObject &obj);
    void handleUpdateDoctorProfile(QTcpSocket *sock, const QJsonObject &obj);

    void handleLoadMedicineData(QTcpSocket *sock, const QJsonObject &obj);
    void handleLoadOrderDetails(QTcpSocket *sock, const QJsonObject &obj);
    void handleloadAvailableDoctors(QTcpSocket *sock, const QJsonObject &obj);
    void handlesubmitAppointmentRequest(QTcpSocket *sock, const QJsonObject &obj);
    void handleloadDoctorList(QTcpSocket *sock, const QJsonObject &obj);
    void handleonPurchaseClicked(QTcpSocket *sock, const QJsonObject &obj);
    void handleprocessPayment(QTcpSocket *sock, const QJsonObject &obj);
    // --- reply helpers ---
    void sendJson(QTcpSocket *sock, const QJsonObject &obj);
    void sendError(QTcpSocket *sock, const QString &type, const QString &msg);
    // xiamian gaoneng
    // ==== 通用：加载/修改(创建) 模板 ====
    // 患者列表
    void handleLoadPatientList(QTcpSocket *sock, const QJsonObject &obj);

    // 病历
    void handleLoadMedicalRecord(QTcpSocket *sock, const QJsonObject &obj);
    void handleSaveMedicalRecord(QTcpSocket *sock, const QJsonObject &obj);

    // 医嘱
    void handleLoadMedicalOrders(QTcpSocket *sock, const QJsonObject &obj);
    void handleSaveMedicalOrders(QTcpSocket *sock, const QJsonObject &obj);

    // 考勤 / 请假
    void handleLoadAttendanceToday(QTcpSocket *sock, const QJsonObject &obj);
    void handleClockEvent(QTcpSocket *sock, const QJsonObject &obj);
    void handleSubmitLeave(QTcpSocket *sock, const QJsonObject &obj);
    void handleLoadLeaveRecords(QTcpSocket *sock, const QJsonObject &obj);
};

#endif // WIDGET_H
