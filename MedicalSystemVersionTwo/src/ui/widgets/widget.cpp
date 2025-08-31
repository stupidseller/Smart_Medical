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
{   // 只创建一次，不要在这里重复 new
    connectStatus = false;

    if (!myTcpClient) {
        myTcpClient = new QTcpSocket(this);
    }

    connect(myTcpClient, &QTcpSocket::connected,
            this,        &Widget::slotConnected);
    connect(myTcpClient, &QTcpSocket::readyRead,
            this,        &Widget::slotReadyRead);
    connect(myTcpClient, &QTcpSocket::disconnected,
            this,        &Widget::slotDisconnected);


    connect(myTcpClient, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this,        &Widget::slotError);}

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

void Widget::sendListDepartments() {
    sendJson(QJsonObject{{"type","list_departments"}});
}
void Widget::sendListDoctors(int dep, const QString &kw, int online, int limit, int offset,
                             const QString &orderBy, const QString &sort) {
    QJsonObject obj{{"type","list_doctors"},{"limit",limit},{"offset",offset},{"order_by",orderBy},{"sort",sort}};
    if (dep>0) obj.insert("department_id", dep);
    if (!kw.trimmed().isEmpty()) obj.insert("keyword", kw.trimmed());
    if (online==0 || online==1) obj.insert("is_online", online);
    sendJson(obj);
}
void Widget::sendListAvailableSlots(int doctorId, const QString &start, const QString &end) {
    QJsonObject obj{{"type","list_available_slots"}};
    if (doctorId>0) obj.insert("doctor_id", doctorId);
    if (!start.isEmpty()) obj.insert("start", start);
    if (!end.isEmpty())   obj.insert("end", end);
    sendJson(obj);
}
void Widget::sendBookAppointment(int pid, int did, int slotId, const QString &desc) {
    sendJson(QJsonObject{{"type","book_appointment"},{"patient_id",pid},{"doctor_id",did},
                         {"slot_id",slotId},{"disease_description",desc}});
}
void Widget::sendGetPatientProfile(int pid){
    sendJson(QJsonObject{{"type","get_patient_profile"},{"patient_id",pid}});
}
void Widget::sendUpdatePatientProfile(const QJsonObject &patch){
    QJsonObject obj = patch; obj.insert("type","update_patient_profile"); sendJson(obj);
}
void Widget::sendGetDoctorContacts(int pid){ sendJson({{"type","get_doctor_contacts"},{"patient_id",pid}}); }
void Widget::sendGetChatHistory(int pid,int did){ sendJson({{"type","get_chat_history"},{"patient_id",pid},{"doctor_id",did}}); }
void Widget::sendSendMessage(int pid,int did,const QString &c){ sendJson({{"type","send_message"},{"patient_id",pid},{"doctor_id",did},{"content",c}}); }

void Widget::sendGetHealthQuestions(){ sendJson({{"type","get_health_questions"}}); }
void Widget::sendSubmitHealthAssessment(int pid, const QJsonArray &answers){
    sendJson(QJsonObject{{"type","submit_health_assessment"},{"patient_id",pid},{"answers",answers}});
}

void Widget::sendSearchMedicines(const QString &kw, const QString &type, int rx){
    QJsonObject obj{{"type","search_medicines"}};
    if (!kw.trimmed().isEmpty()) obj.insert("keyword", kw.trimmed());
    if (!type.isEmpty()) obj.insert("type", type);                // "处方药"/"非处方药"
    if (rx==0 || rx==1) obj.insert("is_prescription", rx);
    sendJson(obj);
}

void Widget::sendGetOrderDetail(int orderId){ sendJson({{"type","get_order_detail"},{"order_id",orderId}}); }
void Widget::sendCreatePayment(int orderId, const QString &method, double amount){
    sendJson({{"type","create_payment"},{"order_id",orderId},{"method",method},{"amount",amount}});
}

void Widget::sendListPatientAppointments(int pid){ sendJson({{"type","list_patient_appointments"},{"patient_id",pid}}); }
void Widget::sendCancelAppointment(int apptId){ sendJson({{"type","cancel_appointment"},{"appointment_id",apptId}}); }

void Widget::slotConnected()
{
    connectStatus = true;
    qDebug() << "Connected to" << myTcpClient->peerAddress().toString()
             << ":" << myTcpClient->peerPort();}
//读取服务器发送的信息
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

        } else if (type=="list_departments_result") {
            emit departmentsListed(obj.value("items").toArray());

        } else if (type=="list_doctors_result") {
            emit doctorsListed(obj.value("total").toInt(), obj.value("items").toArray());

        } else if (type=="list_available_slots_result") {
            emit availableSlotsListed(obj.value("items").toArray());

        } else if (type=="book_appointment_result") {
            emit appointmentBooked(obj.value("success").toBool(),
                                   obj.value("appointment").toObject(),
                                   obj.value("message").toString());

        } else if (type=="patient_profile_result") {
            emit patientProfileLoaded(obj.value("profile").toObject());

        } else if (type=="update_patient_profile_result") {
            emit patientProfileSaved(obj.value("success").toBool(),
                                     obj.value("message").toString());

        } else if (type=="doctor_contacts_result") {
            emit doctorContactsLoaded(obj.value("items").toArray());

        } else if (type=="chat_history_result") {
            emit chatHistoryLoaded(obj.value("doctor_id").toInt(),
                                   obj.value("messages").toArray());

        } else if (type=="send_message_result") {
            emit messageSent(obj.value("success").toBool(),
                             obj.value("message").toString());

        } else if (type=="health_questions_result") {
            emit healthQuestionsLoaded(obj.value("questions").toArray(),
                                       obj.value("options").toArray());

        } else if (type=="health_assessment_result") {
            emit healthAssessmentDone(obj.value("result").toObject());

        } else if (type=="search_medicines_result") {
            emit medicinesLoaded(obj.value("items").toArray());

        } else if (type=="order_detail_result") {
            emit orderDetailLoaded(obj.value("order").toObject());

        } else if (type=="payment_result") {
            emit paymentProcessed(obj.value("success").toBool(),
                                  obj.value("message").toString(),
                                  obj.value("order").toObject());

        } else if (type=="patient_appointments_result") {
            emit patientAppointmentsLoaded(obj.value("items").toArray());

        } else if (type=="cancel_appointment_result") {
            emit appointmentCanceled(obj.value("success").toBool(),
                                     obj.value("message").toString());
        } else if (type == "register_result") {
            const bool ok = obj.value("success").toBool();
            const QString msg = obj.value("message").toString();
            if (ok) emit registerSucceeded(msg.isEmpty() ? "注册成功" : msg);
            else    emit registerFailed   (msg.isEmpty() ? "注册失败" : msg);
        } else {
            qDebug() << "unknown msg type:" << type << obj;
        }
    }
}
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
