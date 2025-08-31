#ifndef WIDGET_H
#define WIDGET_H

#include <QObject>
#include <QWidget>
#include <QTcpServer>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>
#include <QIntValidator>
#include <QJsonObject>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //槽函数区域
private slots:
    void on_startServerBtn_clicked();//启动服务器按钮
    void slotNewConnection();//新的链接
    void slotReadyRead();
    void slotAcceptError(QAbstractSocket::SocketError error);
    void slotDisconnected();
    void on_clientSendBtn_clicked();
    void slotConnected();

private:
    // 每个socket一段接收缓冲（按行协议更稳）
        QMap<QTcpSocket*, QByteArray> recvBuf;

        // 业务分发
        void processIncoming(QTcpSocket *sock);
        void handleMessage(QTcpSocket *sock, const QJsonObject &obj);
        void handleLogin(QTcpSocket *sock, const QJsonObject &obj);
        void handleRegister(QTcpSocket *sock, const QJsonObject &obj);
        // --- 业务分发（新增） ---
        void handleListDepartments(QTcpSocket *sock, const QJsonObject &obj);
        void handleListDoctors(QTcpSocket *sock, const QJsonObject &obj);
        void handleListAvailableSlots(QTcpSocket *sock, const QJsonObject &obj);
        void handleBookAppointment(QTcpSocket *sock, const QJsonObject &obj);

        // 回包工具
        void sendJson(QTcpSocket *sock, const QJsonObject &obj);
        void sendError(QTcpSocket *sock, const QString &type, const QString &msg);

    Ui::Widget *ui;
    QTcpServer * myTcpServer = nullptr;//服务器端
    QTcpSocket *myTcpClient = nullptr;//新接入的客户端
    QList<QTcpSocket *> clients;//客户端列表   STL再封装  //数组，STL   【vector，list，map，set】

    void init();//初始化函数
};
#endif // WIDGET_H
