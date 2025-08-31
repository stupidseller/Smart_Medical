#include "widget.h"
#include "ui_widget.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QIntValidator>
#include <QSqlRecord>
#include <QJsonObject>
#include <QMap>
#include <QJsonParseError>
const int MAXLINK = 99;//最大连接数

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // init sql
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("hospital_management.db");

    if( !db.open() ) {
        QMessageBox::critical(this, "Error", "Open database fail");
        return;
    }
    // 2) 必须在 open() 之后设置 PRAGMA
        {
            QSqlQuery q;
            // WAL 模式可选：如果你在命令行已设置过，这里可以不再设置
            if (!q.exec("PRAGMA journal_mode=WAL;"))
                qWarning() << "[PRAGMA] journal_mode WAL failed:" << q.lastError().text();

            if (!q.exec("PRAGMA foreign_keys=ON;"))
                qWarning() << "[PRAGMA] foreign_keys ON failed:" << q.lastError().text();
        }

    QSqlQuery query;
        if (!query.exec("SELECT user_id, phone FROM users")) {
            QMessageBox::critical(this, "SQL Error", "数据库查询失败");
            return;
        }
    init();
}

Widget::~Widget()
{
    delete ui;
}

//初始化函数,根据个人的编程习惯处理即可
void Widget::init()
{
    //1 创建一个TcpServer对象
    myTcpServer = new QTcpServer(this);
    //设置端口可输入的范围,端口范围的选择是在合理的范围内,需要学生理解基本的端口概念
    ui->serverPort->setValidator(new QIntValidator(1, 65535, this));
    ui->clientPort->setValidator(new QIntValidator(1, 65535, this));
}

//启动服务器使之监听响应端口,接受来自客户端的连接请求
void Widget::on_startServerBtn_clicked()
{
    qDebug() << "on_startServerBtn_clicked()";
    QString ipAddress;//字符串的地址形式,因为用户的输入应该都是字符串

    //方法1.手动获>取用户手工输入的ip地址
    ipAddress = ui->serverIP->text();
    if(ipAddress.isEmpty()){//如果没有手动设置ip地址,则自动获取
        //方法2.遍历服务器所有ip
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        qDebug() << ipAddressesList;
        // 默认情况下使用本地的非localhost127.0.0.1的本地IPV4地址
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                ui->serverIP->setText(ipAddress);
                break;
            }
        }
    }
//    qDebug() << ipAddress;

    //获取端口
    QString port = ui->serverPort->text();
    if(port.isEmpty()){//如果没有输入端口号,则默认使用一个端口号
        port = "9999";
        ui->serverPort->setText(port);
    }

    //在指定的IP和端口绑定并启动监听
    bool result = myTcpServer->listen(QHostAddress(ipAddress),(qint16)(port.toInt()));
    if(!result){
        //QMessageBox::warning(this,"错误","不能绑定启动");
        ui->tips->setPlainText(QString("服务器在[%1:%2]上绑定失败").arg(ipAddress).arg(port));
        return;
    }else{
       ui->tips->setPlainText(QString("服务器在[%1:%2]上绑定成功").arg(ipAddress).arg(port));
       ui->startServerBtn->setDisabled(true);//绑定启动成功后设置按钮为禁用状态
    }

    //设置最大连接数,可以通过常量设定
    myTcpServer->setMaxPendingConnections(MAXLINK);
    //接受用户的连接,通过信号和槽的方式实现,不是传统的while循环监听
    //如果有新客户端接入时(这就是信号和槽的好处)
    connect(myTcpServer, &QTcpServer::newConnection, this, &Widget::slotNewConnection);
    //Qt4的信号和槽的处理方式,确保出错可以得到处理
    connect(myTcpServer,SIGNAL(acceptError(QAbstractSocket::SocketError)),this,SLOT(slotAcceptError(QAbstractSocket::SocketError)));
}
//有一个新的客户端连入时
void Widget::slotNewConnection()
{
    qDebug() << "slotNewConnection()";
    //获取新创建连接的客户端对象
    myTcpClient = myTcpServer->nextPendingConnection();
    //将新连入的客户端加入到客户端列表中
    clients.append(myTcpClient);
    //客户端有消息发送过来时,读取消息并显示
    connect(myTcpClient, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    //客户端断开时,主要处理提示信息和更新客户端列表
    connect(myTcpClient,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
    //客户端接入,主要处理提示信息并更新客户端列表
    connect(myTcpClient,SIGNAL(connected()),this,SLOT(slotConnected()));
    QString msg = QString("客户端[%1:%2]已经接入").
            arg(myTcpClient->peerAddress().toString()).
            arg(myTcpClient->peerPort());
    ui->tips->append(msg);
}
//客户端连接成功准备读取信息
void Widget::slotReadyRead()
{
    qDebug() << "slotReadyRead()";

    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock) return;

    // 追加到该socket的缓冲
    QByteArray &buf = recvBuf[sock];
    buf.append(sock->readAll());

    // 一次处理多条行消息
        while (true) {
            int nl = buf.indexOf('\n');
            if (nl < 0) break;                 // 没有完整一行，等下次
            QByteArray line = buf.left(nl);    // 取一行（不含换行）
            buf.remove(0, nl + 1);             // 丢弃这一行（含换行）

            if (line.trimmed().isEmpty()) continue;

            // 解析JSON
            QJsonParseError perr{};
            QJsonDocument doc = QJsonDocument::fromJson(line, &perr);
            if (perr.error != QJsonParseError::NoError || !doc.isObject()) {
                qDebug() << "recv invalid json:" << line;
                // 不知道具体类型，统一报错
                sendJson(sock, QJsonObject{
                    {"type","error"},
                    {"success",false},
                    {"message","invalid json"}
                });
                continue;
               }

               handleMessage(sock, doc.object());
          }
}
//客户端连接错误时的处理方式
void Widget::slotAcceptError(QAbstractSocket::SocketError error)
{
    qDebug() << "slotAcceptError(QAbstractSocket::SocketError error)" << error;
}



//客户端断开连接时
void Widget::slotDisconnected()
{
    qDebug() << "slotDisconnected()";
    //获取信号的发送者QObject对象,需要强转为客户端对象
    QTcpSocket * currentClient = qobject_cast<QTcpSocket *>(this->sender());
    QString msg = QString("客户端[%1:%2]退出").arg(currentClient->peerAddress().toString()).arg(currentClient->peerPort());
    ui->tips->append(msg);

    //退出时删除客户端,如果和当前发送信号的客户端的ip和端口完全相同,则从客户端队列中删除
    for(int i=0;i < clients.length();i++){
        if(clients.at(i)->peerAddress() == currentClient->peerAddress() &&
            clients.at(i)->peerPort() == currentClient->peerPort()){
            clients.removeAt(i);//移除对应的客户端
            break;
        }
    }
}
//给客户端发送信息
void Widget::on_clientSendBtn_clicked()
{
    qDebug() << "on_clientSendBtn_clicked()";
    //获取客户端的ip和port
    QString clientIp = ui->clientIp->text();
    QString clientPort = ui->clientPort->text();
    //遍历客户端列表,匹配是否存在该客户端
    qDebug() << clients.size();
    for(int i=0;i < clients.size();i++){
        //如果ip和port都是在客户端列表存在
        if(clients.at(i)->peerAddress().toString()==clientIp &&
            clients.at(i)->peerPort() == clientPort.toInt()){
            QString msg = ui->message->toPlainText();//获取文本框内容
            clients.at(i)->write(msg.toUtf8());//将信息写入客户端,避免乱码的产生使用了utf8
            ui->tips->append(msg);
            break;
        }
    }
    ui->message->clear();//点击发送按钮后清空文本框区域
}

void Widget::slotConnected()
{
    QTcpSocket * currentClient = qobject_cast<QTcpSocket *>(this->sender());
    QString msg = QString("客户端[%1:%2]已连接").arg(currentClient->peerAddress().toString()).arg(currentClient->peerPort());
    ui->tips->append(msg);
}



void Widget::handleMessage(QTcpSocket *sock, const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();
    if (type == "login") {
        handleLogin(sock, obj);
    } else if (type == "register") {
        handleRegister(sock, obj);
    } else if (type == "list_departments")handleListDepartments(sock, obj);
    else if (type == "list_doctors")    handleListDoctors(sock, obj);
    else if (type == "list_available_slots") handleListAvailableSlots(sock, obj);
    else if (type == "book_appointment")     handleBookAppointment(sock, obj);
    else {
        qDebug() << "unknown type:" << type << obj;
        sendError(sock, "error", QString("unknown type: %1").arg(type));
    }
}



void Widget::handleLogin(QTcpSocket *sock, const QJsonObject &obj)
{
    const QString username = obj.value("username").toString().trimmed();
    const QString password = obj.value("password").toString();
    const QString role     = obj.value("role").toString().trimmed(); // 'patient' or 'doctor'

    if (username.isEmpty() || password.isEmpty() ||
        (role != "patient" && role != "doctor")) {
        sendJson(sock, {{"type","login_result"},{"success":false},{"message","参数不完整/角色非法"}});
        return;
    }

    QSqlQuery q;
    q.prepare(R"SQL(
        SELECT account_id, password, role, patient_id, doctor_id
        FROM accounts
        WHERE username=:u AND role=:r AND is_active=1
        LIMIT 1
    )SQL");
    q.bindValue(":u", username);
    q.bindValue(":r", role);

    if (!q.exec()) {
        qWarning() << "[login] sql error:" << q.lastError().text();
        sendJson(sock, {{"type","login_result"},{"success":false},{"message","数据库错误"}});
        return;
    }
    if (!q.next()) {
        sendJson(sock, {{"type","login_result"},{"success":false},{"message","账号不存在或已停用"}});
        return;
    }

    const QString storedPwd = q.value("password").toString();
    if (storedPwd != password) {
        sendJson(sock, {{"type","login_result"},{"success":false},{"message","密码错误"}});
        return;
    }

    QJsonObject payload{
        {"type","login_result"},
        {"success",true},
        {"message","登录成功"},
        {"role", role}
    };

    if (role == "patient") {
        const int pid = q.value("patient_id").toInt();
        QSqlQuery qp; qp.prepare("SELECT name, username FROM patients WHERE patient_id=:id"); qp.bindValue(":id", pid);
        if (qp.exec() && qp.next()) {
            payload.insert("patient_id", pid);
            payload.insert("name", qp.value("name").toString());
            payload.insert("nickname", qp.value("username").toString());
        }
    } else { // doctor
        const int did = q.value("doctor_id").toInt();
        QSqlQuery qd; qd.prepare(R"SQL(
            SELECT d.name, d.title, dep.name AS department
            FROM doctors d JOIN departments dep ON dep.department_id=d.department_id
            WHERE d.doctor_id=:id
        )SQL"); qd.bindValue(":id", did);
        if (qd.exec() && qd.next()) {
            payload.insert("doctor_id", did);
            payload.insert("name", qd.value("name").toString());
            payload.insert("title", qd.value("title").toString());
            payload.insert("department", qd.value("department").toString());
        }
    }

    sendJson(sock, payload);
}

void Widget::handleListDepartments(QTcpSocket *sock, const QJsonObject &)
{
    QSqlQuery q("SELECT department_id, name FROM departments ORDER BY department_id");
    if (!q.isActive()) {
        sendJson(sock, {{"type","list_departments_result"},{"success",false},{"message","数据库错误"}});
        return;
    }
    QJsonArray items;
    while (q.next()) {
        items.append(QJsonObject{
            {"department_id", q.value(0).toInt()},
            {"name",         q.value(1).toString()}
        });
    }
    sendJson(sock, {{"type","list_departments_result"},{"success",true},{"items", items}});
}

void Widget::handleListDoctors(QTcpSocket *sock, const QJsonObject &obj)
{
    const int depId = obj.value("department_id").toInt(-1);
    const QString keyword = obj.value("keyword").toString().trimmed();
    const int isOnline = obj.value("is_online").toInt(-1); // -1=不限, 0/1 指定
    int limit = obj.value("limit").toInt(20);
    int offset = obj.value("offset").toInt(0);

    QString orderBy = obj.value("order_by").toString().trimmed(); // name/title/fee
    QString sort    = obj.value("sort").toString().trimmed().toUpper(); // ASC/DESC
    if (limit<=0 || limit>100) limit=20;
    if (offset<0) offset=0;
    if (!QStringList{"name","title","consultation_fee","doctor_id"}.contains(orderBy)) orderBy="doctor_id";
    if (sort!="ASC" && sort!="DESC") sort="ASC";

    QString where = " WHERE 1=1 ";
    if (depId>0) where += " AND d.department_id=:dep ";
    if (isOnline==0 || isOnline==1) where += " AND d.is_online=:on ";
    if (!keyword.isEmpty()) where += " AND (d.name LIKE :kw OR d.specialty LIKE :kw OR dep.name LIKE :kw) ";

    // count
    QSqlQuery cnt;
    cnt.prepare("SELECT COUNT(*) FROM doctors d JOIN departments dep ON dep.department_id=d.department_id " + where);
    if (depId>0) cnt.bindValue(":dep", depId);
    if (isOnline==0 || isOnline==1) cnt.bindValue(":on", isOnline);
    if (!keyword.isEmpty()) cnt.bindValue(":kw", "%"+keyword+"%");
    if (!cnt.exec()) {
        qWarning() << "[list_doctors] count error:" << cnt.lastError().text();
        sendJson(sock, {{"type","list_doctors_result"},{"success",false},{"message","数据库错误"}});
        return;
    }
    cnt.next();
    const int total = cnt.value(0).toInt();

    // list
    QSqlQuery q;
    const QString sql =
        "SELECT d.doctor_id, d.name, d.title, dep.name AS department, d.specialty, d.experience, d.consultation_fee, d.is_online "
        "FROM doctors d JOIN departments dep ON dep.department_id=d.department_id " + where +
        QString(" ORDER BY %1 %2 LIMIT :limit OFFSET :offset").arg(orderBy, sort);
    q.prepare(sql);
    if (depId>0) q.bindValue(":dep", depId);
    if (isOnline==0 || isOnline==1) q.bindValue(":on", isOnline);
    if (!keyword.isEmpty()) q.bindValue(":kw", "%"+keyword+"%");
    q.bindValue(":limit", limit);
    q.bindValue(":offset", offset);

    if (!q.exec()) {
        qWarning() << "[list_doctors] list error:" << q.lastError().text();
        sendJson(sock, {{"type","list_doctors_result"},{"success",false},{"message","数据库错误"}});
        return;
    }

    QJsonArray items;
    while (q.next()) {
        items.append(QJsonObject{
            {"doctor_id", q.value("doctor_id").toInt()},
            {"name",      q.value("name").toString()},
            {"title",     q.value("title").toString()},
            {"department",q.value("department").toString()},
            {"specialty", q.value("specialty").toString()},
            {"experience",q.value("experience").toString()},
            {"consultation_fee", q.value("consultation_fee")},
            {"is_online", q.value("is_online").toInt()}
        });
    }

    sendJson(sock, {{"type","list_doctors_result"},{"success",true},{"total", total},{"items", items}});
}

void Widget::handleListAvailableSlots(QTcpSocket *sock, const QJsonObject &obj)
{
    const int doctorId = obj.value("doctor_id").toInt(-1);
    const QString start = obj.value("start").toString().trimmed(); // 'YYYY-MM-DD' 或 'YYYY-MM-DD HH:MM:SS'
    const QString end   = obj.value("end").toString().trimmed();

    QString where = " WHERE 1=1 ";
    if (doctorId>0) where += " AND doctor_id=:did ";
    if (!start.isEmpty()) where += " AND slot_start >= :st ";
    if (!end.isEmpty())   where += " AND slot_start <  :ed ";

    QSqlQuery q;
    q.prepare("SELECT slot_id, doctor_id, doctor_name, title, department, slot_start, slot_end, fee "
              "FROM v_available_slots " + where +
              " ORDER BY slot_start ASC");
    if (doctorId>0) q.bindValue(":did", doctorId);
    if (!start.isEmpty()) q.bindValue(":st", start);
    if (!end.isEmpty())   q.bindValue(":ed", end);

    if (!q.exec()) {
        qWarning() << "[list_slots] sql error:" << q.lastError().text();
        sendJson(sock, {{"type","list_available_slots_result"},{"success",false},{"message","数据库错误"}});
        return;
    }

    QJsonArray items;
    while (q.next()) {
        items.append(QJsonObject{
            {"slot_id", q.value("slot_id").toInt()},
            {"doctor_id", q.value("doctor_id").toInt()},
            {"doctor_name", q.value("doctor_name").toString()},
            {"title", q.value("title").toString()},
            {"department", q.value("department").toString()},
            {"slot_start", q.value("slot_start").toString()},
            {"slot_end",   q.value("slot_end").toString()},
            {"fee", q.value("fee")}
        });
    }
    sendJson(sock, {{"type","list_available_slots_result"},{"success",true},{"items", items}});
}

void Widget::handleBookAppointment(QTcpSocket *sock, const QJsonObject &obj)
{
    const int patientId = obj.value("patient_id").toInt();
    const int doctorId  = obj.value("doctor_id").toInt();
    const int slotId    = obj.value("slot_id").toInt();
    const QString desc  = obj.value("disease_description").toString().trimmed();

    if (patientId<=0 || doctorId<=0 || slotId<=0) {
        sendJson(sock, {{"type","book_appointment_result"},{"success",false},{"message","参数不完整"}});
        return;
    }

    QSqlQuery ins;
    ins.prepare(R"SQL(
        INSERT INTO appointments(patient_id, doctor_id, slot_id, disease_description)
        VALUES(:p,:d,:s,:desc)
    )SQL");
    ins.bindValue(":p", patientId);
    ins.bindValue(":d", doctorId);
    ins.bindValue(":s", slotId);
    ins.bindValue(":desc", desc);

    if (!ins.exec()) {
        // 你的触发器会抛中文错误，如 "该时间段不可用或已被预约"
        const QString dbmsg = ins.lastError().databaseText();
        qWarning() << "[book] err:" << dbmsg;
        sendJson(sock, {{"type","book_appointment_result"},{"success",false},{"message", dbmsg.isEmpty() ? "预约失败" : dbmsg}});
        return;
    }

    // 取回预约信息（包括回填的fee）
    QSqlQuery q("SELECT last_insert_rowid()");
    q.next(); const int apptId = q.value(0).toInt();

    QSqlQuery one;
    one.prepare(R"SQL(
        SELECT appointment_id, patient_id, doctor_id, slot_id, status, fee, created_at
        FROM appointments WHERE appointment_id=:id
    )SQL");
    one.bindValue(":id", apptId);
    one.exec(); one.next();

    sendJson(sock, QJsonObject{
        {"type","book_appointment_result"},
        {"success",true},
        {"message","预约成功"},
        {"appointment", QJsonObject{
            {"appointment_id", one.value("appointment_id").toInt()},
            {"patient_id",     one.value("patient_id").toInt()},
            {"doctor_id",      one.value("doctor_id").toInt()},
            {"slot_id",        one.value("slot_id").toInt()},
            {"status",         one.value("status").toString()},
            {"fee",            one.value("fee")},
            {"created_at",     one.value("created_at").toString()}
        }}
    });
}



void Widget::sendJson(QTcpSocket *sock, const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    payload.append('\n'); // 按行协议
    sock->write(payload);
    sock->flush();
}

void Widget::sendError(QTcpSocket *sock, const QString &type, const QString &msg)
{
    sendJson(sock, QJsonObject{
        {"type", type},
        {"success", false},
        {"message", msg}
    });
}
