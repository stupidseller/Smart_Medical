#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QMap>
#include <QJsonObject>

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

    // --- dispatcher & handlers ---
    void handleMessage(QTcpSocket *sock, const QJsonObject &obj);
    void handleLogin(QTcpSocket *sock, const QJsonObject &obj);
    void handleRegister(QTcpSocket *sock, const QJsonObject &obj);

    // 新增：把cpp里已有实现的函数声明补齐
    void handleGetPatientProfile(QTcpSocket *sock, const QJsonObject &obj);
    void handleUpdatePatientProfile(QTcpSocket *sock, const QJsonObject &obj);

    void handleListDepartments(QTcpSocket *sock, const QJsonObject &obj);
    void handleListDoctors(QTcpSocket *sock, const QJsonObject &obj);
    void handleListAvailableSlots(QTcpSocket *sock, const QJsonObject &obj);
    void handleBookAppointment(QTcpSocket *sock, const QJsonObject &obj);
    void handleSearchMedicines(QTcpSocket *sock, const QJsonObject &obj);
    void handleEnsurePendingOrder(QTcpSocket *sock, const QJsonObject &obj);
    void handleAddMedicineToOrder(QTcpSocket *sock, const QJsonObject &obj);
    void handleGetOrderDetail(QTcpSocket *sock, const QJsonObject &obj);
    void handleCreatePayment(QTcpSocket *sock, const QJsonObject &obj);

    // --- reply helpers ---
    void sendJson(QTcpSocket *sock, const QJsonObject &obj);
    void sendError(QTcpSocket *sock, const QString &type, const QString &msg);
};

#endif // WIDGET_H
