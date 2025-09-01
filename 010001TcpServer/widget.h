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
    void handleGetPatientProfile(QTcpSocket *sock, const QJsonObject &obj);
    void handleUpdatePatientProfile(QTcpSocket *sock, const QJsonObject &obj);
    // bu yao dong shang mian de yijing ok

    void handleLoadMedicineData(QTcpSocket *sock, const QJsonObject &obj);
    void handleLoadOrderDetails(QTcpSocket *sock, const QJsonObject &obj);
    void handleloadAvailableDoctors(QTcpSocket *sock, const QJsonObject &obj);

    // --- reply helpers ---
    void sendJson(QTcpSocket *sock, const QJsonObject &obj);
    void sendError(QTcpSocket *sock, const QString &type, const QString &msg);
};

#endif // WIDGET_H
