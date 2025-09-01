#include "widget.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QAbstractSocket>

Widget::Widget(QObject *parent)
    : QObject(parent)
{
    init();
}

Widget::~Widget(){}


void Widget::init()
{
    connectStatus = false;
    if (!myTcpClient) myTcpClient = new QTcpSocket(this);

    connect(myTcpClient, &QTcpSocket::connected,
            this,        &Widget::slotConnected);
    connect(myTcpClient, &QTcpSocket::readyRead,
            this,        &Widget::slotReadyRead);
    connect(myTcpClient, &QTcpSocket::disconnected,
            this,        &Widget::slotDisconnected);

    connect(myTcpClient, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this,        &Widget::slotError);

}
void Widget::on_connectServerBtn_clicked()
{
    if (connectStatus) return;
    // 如果 serverIp 是域名或 IPv4 字符串，这个重载最稳
    myTcpClient->connectToHost(serverIp, serverPort);
}

void Widget::ensureConnected()
{
    if (!connectStatus) {
        on_connectServerBtn_clicked();
        // 可选：阻塞等待 1 秒（若你允许轻微阻塞）
        // myTcpClient->waitForConnected(1000);
    }
}

void Widget::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    payload.append('\n');

    ensureConnected();
    if (myTcpClient->state() == QAbstractSocket::ConnectedState) {
        myTcpClient->write(payload);
        myTcpClient->flush();
    } else {
        qDebug() << "[sendJson] not connected, drop:" << payload;
        // 如需排队可在此扩展
    }
}
// kaishi gongju lei
void Widget::sendLoginData(const QString &username,
                           const QString &password,
                           const QString &roleText)
{
    QString role = roleText;
    if (role.contains("患")) role = "patient";
    else if (role.contains("医")) role = "doctor";

    QJsonObject obj{
        {"type", "login"},
        {"username", username},
        {"password", password},
        {"role", role}
    };
    QJsonDocument doc(obj);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    payload.append('\n');

    ensureConnected();

    if (myTcpClient->state() == QAbstractSocket::ConnectedState) {
        myTcpClient->write(payload);
        myTcpClient->flush();
    } else {
        qDebug() << "[login] not connected";
        // TODO: 需要的话，这里可以把 payload 暂存到队列，等 connected 再发
    }
}

void Widget::sendRegisterData(const QString &username,
                              const QString &email,
                              const QString &password,
                              const QString &userType,
                              const QString &department)
{
    QJsonObject obj{
        {"type", "register"},
        {"username", username},
        {"email", email},
        {"password", password},
        {"user_type", userType}
    };
    if (userType == "doctor"){
        obj.insert("department", department);
    }

    QJsonDocument doc(obj);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    payload.append('\n');

    ensureConnected();

    if (myTcpClient->state() == QAbstractSocket::ConnectedState) {
        myTcpClient->write(payload);
        myTcpClient->flush();
    } else {
        qDebug() << "[register] not connected yet, drop this try";
        // TODO: 同上，可排队
    }
}
void Widget::sendGetPatientProfile(int pid){
    sendJson(QJsonObject{{"type","get_patient_profile"},{"patient_id",pid}});
}
void Widget::sendUpdatePatientProfile(const QJsonObject &patch){
    QJsonObject obj = patch; obj.insert("type","update_patient_profile"); sendJson(obj);
}
// shang mian yijing ok

void Widget::loadMedicineData()
{
    // 客户端请求：type 必须与服务端分发字符串一致
    sendJson({
        {"type", "load_medicine_data"}
    });
}
void Widget::loadOrderDetails(int orderId)
{
    QJsonObject req{
        {"type", "loadOrderDetails"}
    };
    if (orderId >= 0) req.insert("order_id", orderId);
    sendJson(req);
}
void Widget::loadAvailableDoctors()
{
    sendJson({{"type", "loadAvailableDoctors"}});
}
void Widget::submitAppointmentRequest(int patientId, int doctorId, int slotId, const QString &desc)
{
    sendJson({
        {"type", "submitAppointmentRequest"},
        {"patient_id", patientId},
        {"doctor_id",  doctorId},
        {"slot_id",    slotId},
        {"disease_description", desc}
    });
}
void Widget::loadDoctorList()
{
    sendJson({{"type", "loadDoctorList"}});
}
void Widget::onPurchaseClicked(int patientId, const QJsonArray &cart, int orderId)
{
    QJsonObject req{
        {"type",       "onPurchaseClicked"},
        {"patient_id", patientId},
        {"cart",       cart}
    };
    if (orderId > 0) req.insert("order_id", orderId);
    sendJson(req);
}
void Widget::processPayment(int orderId, const QString &method, double amount,
                         const QString &status, const QString &txref)
{
    QJsonObject req{
        {"type",    "processPayment"},
        {"order_id", orderId},
        {"method",   method},
        {"amount",   amount},
        {"status",   status}
    };
    if (!txref.isEmpty()) req.insert("transaction_ref", txref);
    sendJson(req);
}
// xia mian zhi yunxu xiugai slotReadyRead
// xiamian zhege if elseif
void Widget::slotReadyRead()
{
    while (myTcpClient->canReadLine()) {
        const QByteArray line = myTcpClient->readLine().trimmed();
        if (line.isEmpty()) continue;

        QJsonParseError err{};
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "recv (non-json):" << line;
            continue;
        }

        QJsonObject obj = doc.object();
        const QString type = obj.value("type").toString();
        // shangmian bu yao xiugai
        if (type=="login_result") {
            const bool ok = obj.value("success").toBool();
            const QString msg = obj.value("message").toString();
            if (ok) {
                const QString role = obj.value("role").toString();
                int id = role=="patient" ? obj.value("patient_id").toInt()
                                         : obj.value("doctor_id").toInt();
                const QString name = obj.value("name").toString();
                emit loginSucceeded(msg.isEmpty()?"登录成功":msg);
                emit loginSucceededDetail(id, name, role);
            } else emit loginFailed(msg.isEmpty()?"登录失败":msg);

        }  else if (type=="patient_profile_result") {
            emit patientProfileLoaded(obj.value("profile").toObject());

        } else if (type == "processPayment") {
            emit processPaymentOk(obj);
            continue;
        } else if (type == "onPurchaseClicked") {
            emit onPurchaseClickedOk(obj);
            continue;
        } else if (type == "loadDoctorList") {
            const bool ok = obj.value("success").toBool();
            if (!ok) {
                qWarning() << "[Api] loadDoctorList failed:" << obj.value("error").toString();
                continue;
            }
            emit loadDoctorListOk(obj.value("doctors").toArray());
            continue;
        } else if (type == "loadOrderDetails") {
            const bool ok = obj.value("success").toBool();
            if (!ok) {
                qWarning() << "[Api] loadOrderDetails failed:" << obj.value("error").toString();
                continue;
            }
            emit loadOrderDetailsOk(obj.value("orders").toArray());
            continue;
        } else if (type == "submitAppointmentRequest") {
            emit submitAppointmentRequestOk(obj);
            continue;
        } else if (type=="update_patient_profile_result") {
            emit patientProfileSaved(obj.value("success").toBool(),
                                     obj.value("message").toString());

        } else if (type == "register_result") {
            const bool ok = obj.value("success").toBool();
            const QString msg = obj.value("message").toString();
            if (ok) emit registerSucceeded(msg.isEmpty() ? "注册成功" : msg);
            else    emit registerFailed   (msg.isEmpty() ? "注册失败" : msg);
            // shang mian bu yao xiugai, yijing ok;
        } else if (type == "loadAvailableDoctors") { //
            const bool ok = obj.value("success").toBool();
            if (!ok) {
                qWarning() << "[Api] loadAvailableDoctors failed:" << obj.value("error").toString();
                continue;
            }
            emit loadAvailableDoctorsOk(obj.value("doctors").toArray());
            continue;
        } else if (type == "load_medicine_data_ok") {
            const bool ok = obj.value("success").toBool();
            if (!ok) {
                qWarning() << "[Api] load_medicine_data failed:" << obj.value("error").toString();
                // 这里可根据需要发错误信号
                continue;
            }
            const QJsonArray arr = obj.value("medicines").toArray();
            emit loadMedicineDataOk(arr);
            continue;
        } else {
            qDebug() << "unknown msg type:" << type << obj;
        }
    }
}


// tingl





void Widget::slotConnected()
{
    connectStatus = true;
    qDebug() << "Connected to" << myTcpClient->peerAddress().toString()
             << ":" << myTcpClient->peerPort();}
void Widget::slotDisconnected()
{
    qDebug() << "Disconnected";
    connectStatus = false;
}
void Widget::slotError(QAbstractSocket::SocketError error)
{
    qDebug() << "socket error:" << error << myTcpClient->errorString();
    connectStatus = false;
}
//end slots
