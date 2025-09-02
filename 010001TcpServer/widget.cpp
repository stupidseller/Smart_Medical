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
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkInterface>

const int MAXLINK = 99;//最大连接数

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // init sql
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("hospital_app.db");

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
    if (!query.exec("SELECT name FROM patients")) {
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

// bu yao dong shang mian de, yijing ok





void Widget::handleMessage(QTcpSocket *sock, const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();
    if (type == "login") {
        handleLogin(sock, obj);
    } else if (type == "register") {
        handleRegister(sock, obj);
    } else if (type == "get_patient_profile") {
        handleGetPatientProfile(sock, obj);
    } else if (type == "update_patient_profile") {
        handleUpdatePatientProfile(sock, obj);
        // shang mian bu yao dong
    } else if (type == "load_medicine_data") {
        handleLoadMedicineData(sock, obj);
    } else if (type == "loadOrderDetails") {
        handleLoadOrderDetails(sock, obj);
    } else if (type == "loadAvailableDoctors") {
        handleloadAvailableDoctors(sock, obj);
    } else if (type == "submitAppointmentRequest") {
        handlesubmitAppointmentRequest(sock, obj);
    } else if (type == "loadDoctorList") {
        handleloadDoctorList(sock, obj);
    } else if (type == "onPurchaseClicked") {
        handleonPurchaseClicked(sock, obj);
    } else if (type == "processPayment") {
        handleprocessPayment(sock, obj);
    } else {
        qDebug() << "unknown type:" << type << obj;
        sendError(sock, "error", QString("unknown type: %1").arg(type));
    }
}
// shang mian keyi jia, if / else if

void Widget::handleLoadMedicineData(QTcpSocket *sock, const QJsonObject &obj)
{

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[load_medicine_data] database not open";
        sendJson(sock, {
            {"type", "load_medicine_data_ok"},
            {"success", false},
            {"error",  "database not open"}
        });
        return;
    }

    // 只查 UI 需要的列（与前端字段对齐）
    static const char *kSql = R"SQL(
        SELECT
            name,
            description,
            type,
            is_prescription,
            price,
            specifications,
            manufacturer,
            effects,
            dosage,
            icon_color
        FROM medicines
        ORDER BY name ASC
    )SQL";

    QSqlQuery q(db);
    if (!q.prepare(kSql)) {
        const QString err = q.lastError().text();
        qWarning() << "[load_medicine_data] prepare failed:" << err;
        sendJson(sock, {
            {"type", "load_medicine_data_ok"},
            {"success", false},
            {"error",  QString("prepare failed: %1").arg(err)}
        });
        return;
    }

    if (!q.exec()) {
        const QString err = q.lastError().text();
        qWarning() << "[load_medicine_data] exec failed:" << err;
        sendJson(sock, {
            {"type", "load_medicine_data_ok"},
            {"success", false},
            {"error",  QString("exec failed: %1").arg(err)}
        });
        return;
    }

    QJsonArray arr;
    // arr.reserve(q.size() > 0 ? q.size() : 16);

    while (q.next()) {
        // 注意：不要用变量名"type"遮蔽 JSON 的 key，这里用 drugType
        const QString name         = q.value("name").toString();
        const QString description  = q.value("description").toString();
        const QString drugType     = q.value("type").toString();
        const bool    isPrescription = q.value("is_prescription").toInt() != 0;
        const double  price        = q.value("price").toDouble();
        const QString specs        = q.value("specifications").toString();
        const QString manufacturer = q.value("manufacturer").toString();
        const QString effects      = q.value("effects").toString();
        const QString dosage       = q.value("dosage").toString();
        const QString iconColor    = q.value("icon_color").toString();

        QJsonObject item{
            {"name",            name},
            {"description",     description},
            {"type",            drugType},
            {"is_prescription", isPrescription},
            {"price",           price},
            {"specifications",  specs},
            {"manufacturer",    manufacturer},
            {"effects",         effects},
            {"dosage",          dosage},
            {"icon_color",      iconColor}
        };
        arr.append(item);

        qDebug().noquote() << QString("药品: %1 | 类型: %2 | 处方: %3 | 价格: %4 | 规格: %5")
                              .arg(name, drugType, isPrescription ? "是" : "否")
                              .arg(price)
                              .arg(specs);
    }

    QJsonObject resp{
        {"type",     "load_medicine_data_ok"},
        {"success",  true},
        {"count",    static_cast<int>(arr.size())},
        {"medicines", arr}
    };
    sendJson(sock, resp);
}

void Widget::handleLoadOrderDetails(QTcpSocket *sock, const QJsonObject &obj)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[loadOrderDetails] database not open";
        sendJson(sock, {
            {"type", "loadOrderDetails"},
            {"success", false},
            {"error",  "database not open"}
        });
        return;
    }

    // 可选：按订单号筛选；如果 obj 里带了 "order_id"（int），就按该订单查单条
    const bool hasFilter = obj.contains("order_id");
    const int  orderId   = obj.value("order_id").toInt(); // 不带就为 0，不影响

    // 只查 UI 需要的列（与你 UI 字段一一对应）
    // 备注：%1 位置拼接 WHERE 子句
    static const char *kSqlTmpl = R"SQL(
SELECT
  o.order_id,
  ('PO' || strftime('%Y%m%d', o.created_at) || printf('%06d', o.order_id)) AS order_code,
  o.created_at                  AS create_time,
  p.name                        AS patient_name,
  dep.name                      AS department,
  d.name                        AS doctor_name,
  COALESCE(SUM(CASE WHEN oi.item_name = '挂号费'    THEN oi.amount END), 0) AS fee_reg,
  COALESCE(SUM(CASE WHEN oi.item_name = '诊查费'    THEN oi.amount END), 0) AS fee_consult,
  COALESCE(SUM(CASE WHEN oi.item_name LIKE '药品-%' THEN oi.amount END), 0) AS fee_drug,
  COALESCE(SUM(CASE WHEN oi.item_name LIKE '检查%'  THEN oi.amount END), 0) AS fee_exam,
  o.discount                                           AS discount_store,
  o.total_amount                                       AS total_amount_store,
  (o.total_amount - o.discount)                        AS payable
FROM orders o
JOIN patients       p   ON p.patient_id      = o.patient_id
LEFT JOIN departments dep ON dep.department_id = o.department_id
LEFT JOIN doctors    d   ON d.doctor_id      = o.doctor_id
LEFT JOIN order_items oi ON oi.order_id      = o.order_id
%1
GROUP BY
  o.order_id, o.created_at, p.name, dep.name, d.name, o.discount, o.total_amount
ORDER BY o.created_at DESC
)SQL";

    const QString whereClause = hasFilter ? QStringLiteral("WHERE o.order_id = :oid") : QString();
    const QString sql = QString::fromUtf8(kSqlTmpl).arg(whereClause);

    QSqlQuery q(db);
    if (!q.prepare(sql)) {
        const QString err = q.lastError().text();
        qWarning() << "[loadOrderDetails] prepare failed:" << err;
        sendJson(sock, {
            {"type", "loadOrderDetails"},
            {"success", false},
            {"error",  QString("prepare failed: %1").arg(err)}
        });
        return;
    }
    if (hasFilter) q.bindValue(":oid", orderId);

    if (!q.exec()) {
        const QString err = q.lastError().text();
        qWarning() << "[loadOrderDetails] exec failed:" << err;
        sendJson(sock, {
            {"type", "loadOrderDetails"},
            {"success", false},
            {"error",  QString("exec failed: %1").arg(err)}
        });
        return;
    }

    QJsonArray orders;
   // orders.reserve(q.size() > 0 ? q.size() : 8);

    while (q.next()) {
        const int     oid          = q.value("order_id").toInt();
        const QString orderCode    = q.value("order_code").toString();   // PO20230830xxxxxx
        const QString createTime   = q.value("create_time").toString();  // 建议用 ISO 字符串
        const QString patientName  = q.value("patient_name").toString();
        const QString department   = q.value("department").toString();
        const QString doctorName   = q.value("doctor_name").toString();

        const double feeReg        = q.value("fee_reg").toDouble();
        const double feeConsult    = q.value("fee_consult").toDouble();
        const double feeDrug       = q.value("fee_drug").toDouble();
        const double feeExam       = q.value("fee_exam").toDouble();

        const double discountStore = q.value("discount_store").toDouble();     // 库里为非负
        const double discountOut   = -discountStore;                           // UI 需要负号展示
        const double payable       = q.value("payable").toDouble();            // sum - discount

        QJsonObject fee{
            {"挂号费", feeReg},
            {"诊查费", feeConsult},
            {"药品费", feeDrug},
            {"检查费", feeExam}
        };

        QJsonObject item{
            {"order_id",      oid},
            {"order_code",    orderCode},
            {"create_time",   createTime},
            {"patient_name",  patientName},
            {"department",    department},
            {"doctor_name",   doctorName},
            {"fee",           fee},
            {"discount",      discountOut},      // 负值
            {"total_amount",  payable}           // 应付 = 合计 - 折扣
        };

        orders.append(item);

        qDebug().noquote() << QString("[Order] %1 %2 | %3/%4 | 应付:%5 折扣:%6")
                              .arg(orderCode, createTime, patientName, department)
                              .arg(payable).arg(discountOut);
    }

    sendJson(sock, {
        {"type",     "loadOrderDetails"},   // 注意：响应仍然用同一个 type，满足你在 Widget 里判断
        {"success",  true},
        {"count",    static_cast<int>(orders.size())},
        {"orders",   orders}
    });
}

void Widget::handleloadAvailableDoctors(QTcpSocket *sock, const QJsonObject &obj)
{
    Q_UNUSED(obj);

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[loadAvailableDoctors] database not open";
        sendJson(sock, {
            {"type", "loadAvailableDoctors"},
            {"success", false},
            {"error",  "database not open"}
        });
        return;
    }

    QSqlQuery q(db);
    static const char* sql = R"SQL(
        SELECT d.doctor_id,
               d.name,
               d.title,
               dep.name AS department,
               d.specialty
        FROM doctors d
        JOIN departments dep ON dep.department_id = d.department_id
        ORDER BY d.doctor_id
    )SQL";

    if (!q.prepare(sql) || !q.exec()) {
        qWarning() << "[doctors] sql error:" << q.lastError().text();
        sendJson(sock, {
            {"type", "loadAvailableDoctors"},
            {"success", false},
            {"error",  q.lastError().text()}
        });
        return;
    }

    QJsonArray doctors;
    // doctors.reserve(q.size() > 0 ? q.size() : 8);

    while (q.next()) {
        const int      doctorId   = q.value("doctor_id").toInt();
        const QString  name       = q.value("name").toString();
        const QString  pureTitle  = q.value("title").toString();
        const QString  department = q.value("department").toString();
        const QString  title      = pureTitle + " | " + department;   // 例："主任医师 | 心血管内科"
        const QString  specialty  = q.value("specialty").toString();  // 例："擅长: ..."

        QJsonObject item{
            {"doctor_id",  doctorId},
            {"name",       name},
            {"title",      title},
            {"department", department},
            {"specialty",  specialty},
            // 预留：前端若已有时段结构，这里给一个空数组占位（后续可由另一接口填充）
            {"time_slots", QJsonArray{}}
        };
        doctors.append(item);

        qDebug().noquote() << QString("[Doctor] #%1 %2 %3 %4")
                              .arg(doctorId)
                              .arg(name, title, specialty);
    }

    sendJson(sock, {
        {"type",    "loadAvailableDoctors"},   // 按你的要求：响应同样用这个 type
        {"success", true},
        {"count",   static_cast<int>(doctors.size())},
        {"doctors", doctors}
    });
}

void Widget::handlesubmitAppointmentRequest(QTcpSocket *sock, const QJsonObject &obj)
{
    // 1) 参数校验（与 UI 对齐）
    const int patientId = obj.value("patient_id").toInt(-1);
    const int doctorId  = obj.value("doctor_id").toInt(-1);
    const int slotId    = obj.value("slot_id").toInt(-1);
    const QString desc  = obj.value("disease_description").toString();

    if (patientId < 0 || doctorId < 0 || slotId < 0) {
        sendJson(sock, {
            {"type",    "submitAppointmentRequest"},
            {"success", false},
            {"error",   "invalid arguments: require patient_id, doctor_id, slot_id"}
        });
        return;
    }

    // 2) 打开数据库
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[submitAppointmentRequest] database not open";
        sendJson(sock, {
            {"type",    "submitAppointmentRequest"},
            {"success", false},
            {"error",   "database not open"}
        });
        return;
    }

    // 3) 插入预约（只写 UI 需要的列；created_at 可由表默认值填写）
    static const char *kSql = R"SQL(
        INSERT INTO appointments
            (patient_id, doctor_id, slot_id, disease_description)
        VALUES
            (:patient_id, :doctor_id, :slot_id, :disease_description)
    )SQL";

    QSqlQuery q(db);

    // 可选：事务，便于后续扩展扣减号源等
    if (!db.transaction()) {
        qWarning() << "[submitAppointmentRequest] begin tx failed:" << db.lastError().text();
        // 不中断，继续尝试执行（有的驱动不需要显式事务）
    }

    if (!q.prepare(kSql)) {
        const QString err = q.lastError().text();
        qWarning() << "[submitAppointmentRequest] prepare failed:" << err;
        sendJson(sock, {
            {"type",    "submitAppointmentRequest"},
            {"success", false},
            {"error",   QString("prepare failed: %1").arg(err)}
        });
        if (db.isOpen()) db.rollback();
        return;
    }

    q.bindValue(":patient_id", patientId);
    q.bindValue(":doctor_id",  doctorId);
    q.bindValue(":slot_id",    slotId);
    q.bindValue(":disease_description", desc);

    if (!q.exec()) {
        const QString err = q.lastError().text();
        qWarning() << "[submitAppointmentRequest] exec failed:" << err;
        sendJson(sock, {
            {"type",    "submitAppointmentRequest"},
            {"success", false},
            {"error",   QString("exec failed: %1").arg(err)}
        });
        if (db.isOpen()) db.rollback();
        return;
    }

    const QVariant newId = q.lastInsertId();   // SQLite/MySQL 可用；部分驱动可能返回无效
    if (!db.commit()) {
        qWarning() << "[submitAppointmentRequest] commit failed:" << db.lastError().text();
        // 即便提交失败，也给出错误
        sendJson(sock, {
            {"type",    "submitAppointmentRequest"},
            {"success", false},
            {"error",   QString("commit failed: %1").arg(db.lastError().text())}
        });
        return;
    }

    // 4) 响应（type 仍然是 submitAppointmentRequest，满足你在 Widget 里判断）
    QJsonObject resp{
        {"type",          "submitAppointmentRequest"},
        {"success",       true},
        {"appointment_id", newId.isValid() ? newId.toLongLong() : -1},
        {"patient_id",    patientId},
        {"doctor_id",     doctorId},
        {"slot_id",       slotId}
        // 需要的话可回显 desc；通常不必
        // {"disease_description", desc}
    };
    sendJson(sock, resp);
}

void Widget::handleloadDoctorList(QTcpSocket *sock, const QJsonObject &obj)
    {
        Q_UNUSED(obj);

        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isValid() || !db.isOpen()) {
            qWarning() << "[loadDoctorList] database not open";
            sendJson(sock, {
                {"type", "loadDoctorList"},
                {"success", false},
                {"error",  "database not open"}
            });
            return;
        }

        // 只查 UI 需要的列（与前端字段对齐）
        static const char* SQL_DOCTORS_FOR_UI =
            "SELECT "
            "  d.doctor_id, "
            "  d.name, "
            "  d.title, "
            "  dep.name AS department_name, "
            "  COALESCE(d.specialty, '')   AS specialty, "
            "  COALESCE(d.experience, '')  AS experience "
            "FROM doctors d "
            "JOIN departments dep ON dep.department_id = d.department_id "
            "ORDER BY d.doctor_id;";

        QSqlQuery q(db);
        if (!q.prepare(SQL_DOCTORS_FOR_UI) || !q.exec()) {
            qWarning() << "[loadDoctorList] sql error:" << q.lastError().text();
            sendJson(sock, {
                {"type", "loadDoctorList"},
                {"success", false},
                {"error",  q.lastError().text()}
            });
            return;
        }

        QJsonArray doctors;
        // doctors.reserve(q.size() > 0 ? q.size() : 8);

        while (q.next()) {
            const int      doctorId  = q.value("doctor_id").toInt();
            const QString  name      = q.value("name").toString();
            const QString  title     = q.value("title").toString();
            const QString  dept      = q.value("department_name").toString();
            const QString  specialty = q.value("specialty").toString();   // 例如："擅长：..."
            const QString  exp       = q.value("experience").toString();  // 例如："25年"

            // 仅返回表里有的字段；若 UI 还需 schedule/education/awards，可另建表或另一个接口
            QJsonObject item{
                {"doctor_id",  doctorId},
                {"name",       name},
                {"title",      title},
                {"department", dept},
                {"specialty",  specialty},
                {"experience", exp}
            };
            doctors.append(item);

            qDebug().noquote() << QString("[Doctor] #%1 %2 %3 %4 %5")
                                  .arg(doctorId)
                                  .arg(name, title, dept, exp);
        }

        // 响应仍然用 "loadDoctorList"（与你的 Widget 判断一致）
        sendJson(sock, {
            {"type",    "loadDoctorList"},
            {"success", true},
            {"count",   static_cast<int>(doctors.size())},
            {"doctors", doctors}
        });
    }

// ======= 修正后的 handleonPurchaseClicked，已改为成员函数 + 去掉 makeError =======
void Widget::handleonPurchaseClicked(QTcpSocket *sock, const QJsonObject &obj)
{
    const int patientId = obj.value("patient_id").toInt(-1);
    const int reqOid    = obj.value("order_id").toInt(0);
    const QJsonArray cart = obj.value("cart").toArray();

    if (patientId < 0) { sendError(sock, "onPurchaseClicked", "invalid patient_id"); return; }
    if (cart.isEmpty()) { sendError(sock, "onPurchaseClicked", "cart is empty"); return; }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[onPurchaseClicked] database not open";
        sendError(sock, "onPurchaseClicked", "database not open");
        return;
    }

    if (!db.transaction()) {
        qWarning() << "[onPurchaseClicked] begin transaction failed:" << db.lastError().text();
    }

    int oid = 0;

    if (reqOid > 0) {
        QSqlQuery chk(db);
        const char *sql_chk = R"SQL(
            SELECT order_id FROM orders
            WHERE order_id = :oid AND patient_id = :p AND status = 'created'
            LIMIT 1
        )SQL";
        if (!chk.prepare(sql_chk)) {
            const QString err = chk.lastError().text();
            qWarning() << "[onPurchaseClicked] chk prepare failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("sql prepare failed(chk): %1").arg(err));
            return;
        }
        chk.bindValue(":oid", reqOid);
        chk.bindValue(":p",   patientId);
        if (!chk.exec()) {
            const QString err = chk.lastError().text();
            qWarning() << "[onPurchaseClicked] chk exec failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("sql exec failed(chk): %1").arg(err));
            return;
        }
        if (chk.next()) {
            oid = reqOid;
        }
    }

    if (oid == 0) {
        QSqlQuery qi(db);
        const char *sql_new = R"SQL(
            INSERT INTO orders (patient_id, status, total_amount, discount, created_at)
            VALUES (:p, 'created', 0, 0, CURRENT_TIMESTAMP)
        )SQL";
        if (!qi.prepare(sql_new)) {
            const QString err = qi.lastError().text();
            qWarning() << "[onPurchaseClicked] new order prepare failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("prepare failed(new order): %1").arg(err));
            return;
        }
        qi.bindValue(":p", patientId);
        if (!qi.exec()) {
            const QString err = qi.lastError().text();
            qWarning() << "[onPurchaseClicked] new order exec failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("exec failed(new order): %1").arg(err));
            return;
        }
        QVariant v = qi.lastInsertId();
        oid = v.isValid() ? v.toInt() : 0;
        if (oid <= 0) {
            db.rollback();
            sendError(sock, "onPurchaseClicked", "cannot get new order_id");
            return;
        }
    }

    QSqlQuery qFindMed(db);
    if (!qFindMed.prepare(R"SQL(
        SELECT name, price FROM medicines WHERE medicine_id = :mid LIMIT 1
    )SQL")) {
        const QString err = qFindMed.lastError().text();
        qWarning() << "[onPurchaseClicked] find med prepare failed:" << err;
        db.rollback();
        sendError(sock, "onPurchaseClicked", QString("prepare failed(find med): %1").arg(err));
        return;
    }

    QSqlQuery qInsItem(db);
    if (!qInsItem.prepare(R"SQL(
        INSERT INTO order_items
            (order_id, item_type, item_name, medicine_id, quantity, unit_price, amount)
        VALUES
            (:oid, 'drug', :iname, :mid, :qty, :uprice, :amount)
    )SQL")) {
        const QString err = qInsItem.lastError().text();
        qWarning() << "[onPurchaseClicked] insert item prepare failed:" << err;
        db.rollback();
        sendError(sock, "onPurchaseClicked", QString("prepare failed(insert item): %1").arg(err));
        return;
    }

    QJsonArray itemsOut;
    double total = 0.0;

    for (const QJsonValue &v : cart) {
        const QJsonObject it = v.toObject();
        const int mid = it.value("medicine_id").toInt(-1);
        const int qty = it.value("qty").toInt(0);
        double unitPrice = it.value("unit_price").toDouble(-1.0);

        if (mid < 0 || qty <= 0) {
            db.rollback();
            sendError(sock, "onPurchaseClicked", "invalid cart item (medicine_id/qty)");
            return;
        }

        qFindMed.bindValue(":mid", mid);
        if (!qFindMed.exec()) {
            const QString err = qFindMed.lastError().text();
            qWarning() << "[onPurchaseClicked] find med exec failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("exec failed(find med): %1").arg(err));
            return;
        }
        if (!qFindMed.next()) {
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("medicine not found: id=%1").arg(mid));
            return;
        }
        const QString medName = qFindMed.value("name").toString();
        const double  dbPrice = qFindMed.value("price").toDouble();
        if (unitPrice < 0.0) unitPrice = dbPrice;

        const double amount = unitPrice * static_cast<double>(qty);
        total += amount;

        const QString itemName = QStringLiteral("药品-%1").arg(medName);
        qInsItem.bindValue(":oid",    oid);
        qInsItem.bindValue(":iname",  itemName);
        qInsItem.bindValue(":mid",    mid);
        qInsItem.bindValue(":qty",    qty);
        qInsItem.bindValue(":uprice", unitPrice);
        qInsItem.bindValue(":amount", amount);
        if (!qInsItem.exec()) {
            const QString err = qInsItem.lastError().text();
            qWarning() << "[onPurchaseClicked] insert item exec failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("exec failed(insert item): %1").arg(err));
            return;
        }

        itemsOut.append(QJsonObject{
            {"medicine_id", mid},
            {"name",        medName},
            {"qty",         qty},
            {"unit_price",  unitPrice},
            {"amount",      amount}
        });
    }

    {
        QSqlQuery qUpd(db);
        if (!qUpd.prepare(R"SQL(
            UPDATE orders SET total_amount = COALESCE(total_amount,0) + :delta WHERE order_id = :oid
        )SQL")) {
            const QString err = qUpd.lastError().text();
            qWarning() << "[onPurchaseClicked] update order prepare failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("prepare failed(update order): %1").arg(err));
            return;
        }
        qUpd.bindValue(":delta", total);
        qUpd.bindValue(":oid",   oid);
        if (!qUpd.exec()) {
            const QString err = qUpd.lastError().text();
            qWarning() << "[onPurchaseClicked] update order exec failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("exec failed(update order): %1").arg(err));
            return;
        }
    }

    QString orderCode; double totalAmount=0.0; double discount=0.0;
    {
        QSqlQuery qSum(db);
        const char *sqlSum = R"SQL(
            SELECT
              o.total_amount,
              o.discount,
              ('PO' || strftime('%Y%m%d', o.created_at) || printf('%06d', o.order_id)) AS order_code
            FROM orders o WHERE o.order_id = :oid LIMIT 1
        )SQL";
        if (!qSum.prepare(sqlSum)) {
            const QString err = qSum.lastError().text();
            qWarning() << "[onPurchaseClicked] sum prepare failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("prepare failed(sum): %1").arg(err));
            return;
        }
        qSum.bindValue(":oid", oid);
        if (!qSum.exec() || !qSum.next()) {
            const QString err = qSum.lastError().text();
            qWarning() << "[onPurchaseClicked] sum exec/next failed:" << err;
            db.rollback();
            sendError(sock, "onPurchaseClicked", QString("exec failed(sum): %1").arg(err));
            return;
        }
        totalAmount = qSum.value("total_amount").toDouble();
        discount    = qSum.value("discount").toDouble();
        orderCode   = qSum.value("order_code").toString();
    }

    if (!db.commit()) {
        const QString err = db.lastError().text();
        qWarning() << "[onPurchaseClicked] commit failed:" << err;
        sendError(sock, "onPurchaseClicked", QString("commit failed: %1").arg(err));
        return;
    }

    sendJson(sock, {
        {"type",          "onPurchaseClicked"},
        {"success",       true},
        {"order_id",      oid},
        {"order_code",    orderCode},
        {"total_amount",  totalAmount},
        {"discount",      discount},
        {"count",         static_cast<int>(itemsOut.size())},
        {"items",         itemsOut}
    });
}

void Widget::handleprocessPayment(QTcpSocket *sock, const QJsonObject &obj)
{
    // 0) 读取参数 + 校验
    const int     orderId = obj.value("order_id").toInt(-1);
    QString       method  = obj.value("method").toString().trimmed();      // 例：WeChat/Alipay/Cash
    const double  amount  = obj.value("amount").toDouble(-1.0);
    QString       status  = obj.value("status").toString("success");       // success/pending/failed
    const QString txref   = obj.value("transaction_ref").toString();

    if (orderId <= 0) { sendError(sock, "processPayment", "invalid order_id"); return; }
    if (method.isEmpty()) { method = "Unknown"; }          // 不强制失败
    if (amount <= 0.0) { sendError(sock, "processPayment", "invalid amount"); return; }
    if (status != "success" && status != "pending" && status != "failed") status = "success";

    // 1) 打开数据库
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[processPayment] database not open";
        sendError(sock, "processPayment", "database not open");
        return;
    }

    // 2) 事务
    if (!db.transaction()) {
        qWarning() << "[processPayment] begin tx failed:" << db.lastError().text();
        // 不中断，继续
    }

    // 3) 检查订单存在
    {
        QSqlQuery chk(db);
        static const char* sql_chk =
            "SELECT order_id FROM orders WHERE order_id=:id LIMIT 1";
        if (!chk.prepare(sql_chk)) { db.rollback(); sendError(sock, "processPayment", "数据库错误(prepare chk)"); return; }
        chk.bindValue(":id", orderId);
        if (!chk.exec() || !chk.next()) { db.rollback(); sendError(sock, "processPayment", "订单不存在"); return; }
    }

    // 4) 插入 payment（触发器可更新订单状态；若无触发器，后面我们会自行汇总）
    int paymentId = -1;
    {
        QSqlQuery ins(db);
        static const char* sql_ins =
            "INSERT INTO payments(order_id, method, amount, status, transaction_ref) "
            "VALUES(:o,:m,:a,:s,:t)";
        if (!ins.prepare(sql_ins))  { db.rollback(); sendError(sock, "processPayment", "数据库错误(prepare ins)"); return; }
        ins.bindValue(":o", orderId);
        ins.bindValue(":m", method);
        ins.bindValue(":a", amount);
        ins.bindValue(":s", status);
        ins.bindValue(":t", txref.isEmpty()? QVariant(QVariant::String) : QVariant(txref));

        if (!ins.exec()) { db.rollback(); sendError(sock, "processPayment", "支付写入失败"); return; }

        // lastInsertId 更通用；如需 SQLite 兼容也可备用查询 last_insert_rowid()
        const QVariant lid = ins.lastInsertId();
        if (lid.isValid()) {
            paymentId = lid.toInt();
        } else {
            QSqlQuery rid(db);
            if (rid.exec("SELECT last_insert_rowid()") && rid.next())
                paymentId = rid.value(0).toInt();
        }
    }

    // 5) 汇总订单金额与已付
    double total = 0.0, paid = 0.0; QString orderStatus;
    {
        QSqlQuery qp(db);
        static const char* sql_paid =
            "SELECT COALESCE(SUM(amount),0) FROM payments WHERE order_id=:o AND status='success'";
        qp.prepare(sql_paid); qp.bindValue(":o", orderId);
        if (qp.exec() && qp.next()) paid = qp.value(0).toDouble();

        QSqlQuery qo(db);
        static const char* sql_ord =
            "SELECT status, total_amount FROM orders WHERE order_id=:o LIMIT 1";
        qo.prepare(sql_ord); qo.bindValue(":o", orderId);
        if (qo.exec() && qo.next()) {
            orderStatus = qo.value("status").toString();
            total       = qo.value("total_amount").toDouble();
        }
    }
    const double due = (total - paid) > 0.0 ? (total - paid) : 0.0;

    QJsonArray items;
    {
        QSqlQuery qi(db);
        static const char* sql_items =
            "SELECT medicine_id, item_name, quantity, unit_price, amount "
            "FROM order_items WHERE order_id=:o ORDER BY rowid";
        if (qi.prepare(sql_items)) {
            qi.bindValue(":o", orderId);
            if (qi.exec()) {
                while (qi.next()) {
                    items.append(QJsonObject{
                        {"medicine_id", qi.value("medicine_id").toInt()},
                        {"name",        qi.value("item_name").toString()},
                        {"qty",         qi.value("quantity").toInt()},
                        {"unit_price",  qi.value("unit_price").toDouble()},
                        {"amount",      qi.value("amount").toDouble()}
                    });
                }
            }
        }
    }

    if (!db.commit()) {
        const QString err = db.lastError().text();
        qWarning() << "[processPayment] commit failed:" << err;
        sendError(sock, "processPayment", QString("commit failed: %1").arg(err));
        return;
    }

    sendJson(sock, QJsonObject{
        {"type","processPayment"},
        {"success", status=="success"},
        {"message", status=="success" ? "支付已记录"
                                      : (status=="pending" ? "支付待确认" : "支付失败")},
        {"payment", QJsonObject{
            {"payment_id", paymentId},
            {"order_id",   orderId},
            {"method",     method},
            {"amount",     amount},
            {"status",     status},
            {"transaction_ref", txref}
        }},
        {"order", QJsonObject{
            {"order_id",     orderId},
            {"status",       orderStatus},
            {"total_amount", total},
            {"paid_amount",  paid},
            {"due_amount",   due}
        }},
        {"items", items}
    });
}

// bu yao dong xia mian de, yijing ok
void Widget::handleGetPatientProfile(QTcpSocket *sock, const QJsonObject &obj)
{
    const int pid = obj.value("patient_id").toInt(-1);
    if (pid <= 0) {
        sendJson(sock, {{"type","patient_profile_result"},{"success",false},{"message","参数错误：缺少 patient_id"}});
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);

    const char *sql = R"SQL(
        SELECT patient_id, username, name, gender, birth_date, id_number,
               blood_type, mobile, email, emergency_contact, emergency_phone,
               insurance_type, insurance_card, allergies, history, created_at
        FROM patients WHERE patient_id=:id LIMIT 1
    )SQL";
    if (!q.prepare(sql)) {
        qWarning() << "[get_profile] prepare failed:" << q.lastError().text();
        sendJson(sock, {{"type","patient_profile_result"},{"success",false},{"message","数据库错误(prepare)"}});
        return;
    }
    q.bindValue(":id", pid);
    if (!q.exec()) {
        qWarning() << "[get_profile] exec error:" << q.lastError().text();
        sendJson(sock, {{"type","patient_profile_result"},{"success",false},{"message","数据库错误"}});
        return;
    }
    if (!q.next()) {
        sendJson(sock, {{"type","patient_profile_result"},{"success",false},{"message","未找到该患者"}});
        return;
    }

    QJsonObject profile{
        {"patient_id",        q.value("patient_id").toInt()},
        {"username",          q.value("username").toString()},
        {"name",              q.value("name").toString()},
        {"gender",            q.value("gender").toString()},
        {"birth_date",        q.value("birth_date").toString()},        // "YYYY-MM-DD"
        {"id_number",         q.value("id_number").toString()},
        {"blood_type",        q.value("blood_type").toString()},        // 'A'/'B'/'AB'/'O'/'其他'
        {"mobile",            q.value("mobile").toString()},
        {"email",             q.value("email").toString()},
        {"emergency_contact", q.value("emergency_contact").toString()},
        {"emergency_phone",   q.value("emergency_phone").toString()},
        {"insurance_type",    q.value("insurance_type").toString()},
        {"insurance_card",    q.value("insurance_card").toString()},
        {"allergies",         q.value("allergies").toString()},
        {"history",           q.value("history").toString()},
        {"created_at",        q.value("created_at").toString()}
    };

    sendJson(sock, {
        {"type","patient_profile_result"},
        {"success",true},
        {"profile",profile}
    });
}

void Widget::handleUpdatePatientProfile(QTcpSocket *sock, const QJsonObject &obj)
{
    const int pid = obj.value("patient_id").toInt(-1);
    if (pid <= 0) {
        sendJson(sock, {{"type","update_patient_profile_result"},{"success",false},{"message","参数错误：缺少 patient_id"}});
        return;
    }

    // 允许更新的字段：JSON key -> DB 列名（相同名直接写）
    const QMap<QString, QString> allow {
        {"gender","gender"},
        {"birth_date","birth_date"},                 // "YYYY-MM-DD"
        {"blood_type","blood_type"},                 // A/B/AB/O/其他
        {"mobile","mobile"},
        {"emergency_contact","emergency_contact"},
        {"emergency_phone","emergency_phone"},
        {"insurance_type","insurance_type"},
        {"allergies","allergies"},
        {"history","history"}
    };

    // 收集需要更新的列
    QStringList sets;
    QMap<QString, QVariant> binds;

    for (auto it = allow.constBegin(); it != allow.constEnd(); ++it) {
        if (obj.contains(it.key())) {
            const auto v = obj.value(it.key());
            // 简单校验（可按需加严）
            if (it.key()=="gender") {
                const QString g = v.toString().trimmed();
                if (!QStringList({"男","女","其他"}).contains(g)) continue; // 非法值忽略
                binds[":"+it.key()] = g;
            } else if (it.key()=="blood_type") {
                const QString bt = v.toString().trimmed();
                if (!QStringList({"A","B","AB","O","其他"}).contains(bt)) continue;
                binds[":"+it.key()] = bt;
            } else if (it.key()=="birth_date") {
                binds[":"+it.key()] = v.toString().trimmed(); // 交由后端保持字符串格式
            } else {
                binds[":"+it.key()] = v.toVariant();
            }
            sets << QString("%1=:%2").arg(it.value(), it.key());
        }
    }

    if (sets.isEmpty()) {
        sendJson(sock, {{"type","update_patient_profile_result"},{"success",false},{"message","无可更新字段"}});
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qWarning() << "[update_profile] begin tx failed:" << db.lastError().text();
        sendJson(sock, {{"type","update_patient_profile_result"},{"success",false},{"message","数据库错误(事务)"}});
        return;
    }

    QSqlQuery q(db);
    const QString sql = "UPDATE patients SET " + sets.join(", ") + " WHERE patient_id=:pid";
    if (!q.prepare(sql)) {
        qWarning() << "[update_profile] prepare failed:" << q.lastError().text() << " sql=" << sql;
        db.rollback();
        sendJson(sock, {{"type","update_patient_profile_result"},{"success",false},{"message","数据库错误(prepare)"}});
        return;
    }

    // 绑定
    for (auto it = binds.constBegin(); it != binds.constEnd(); ++it) q.bindValue(it.key(), it.value());
    q.bindValue(":pid", pid);

    if (!q.exec()) {
        qWarning() << "[update_profile] exec error:" << q.lastError().text();
        db.rollback();
        sendJson(sock, {{"type","update_patient_profile_result"},{"success",false},{"message","更新失败"}});
        return;
    }

    if (!db.commit()) {
        qWarning() << "[update_profile] commit failed:" << db.lastError().text();
        db.rollback();
        sendJson(sock, {{"type","update_patient_profile_result"},{"success",false},{"message","数据库错误(commit)"}});
        return;
    }

    sendJson(sock, {{"type","update_patient_profile_result"},{"success",true},{"message","已保存"}});
}

void Widget::handleLogin(QTcpSocket *sock, const QJsonObject &obj)
{
    const QString username = obj.value("username").toString().trimmed();
    const QString password = obj.value("password").toString();
    const QString role     = obj.value("role").toString().trimmed(); // 前端字段是 role

    if (username.isEmpty() || password.isEmpty() ||
        (role != "patient" && role != "doctor" && role != "admin")) {
        sendJson(sock, {{"type","login_result"},{"success",false},{"message","参数不完整/角色非法"}});
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << "[login] driver =" << db.driverName()
             << " username =" << username
             << " role =" << role;

    QSqlQuery q(db);                       // 显式绑定到当前连接
    const char *sql =
        "SELECT account_id, password_hash, role, patient_id, doctor_id "
        "FROM accounts "
        "WHERE username=:u AND role=:r AND is_active=1 "
        "LIMIT 1";

    // 1) 先检查 prepare 是否成功
    if (!q.prepare(sql)) {
        qWarning() << "[login] prepare failed:" << q.lastError().text()
                   << " query=" << sql;
        sendJson(sock, {{"type","login_result"},{"success",false},{"message","数据库错误(prepare)"}});
        return;
    }

    // 2) 绑定命名参数（不要和 ? 混用）
    q.bindValue(":u", username);
    q.bindValue(":r", role);

    // 打印已绑定的键，确认确实绑定了 2 个
    const auto bvs = q.boundValues();
    qDebug() << "[login] bound keys =" << bvs.keys();

    // 3) 执行
    if (!q.exec()) {
        qWarning() << "[login] exec error:" << q.lastError().text()
                   << " lastQuery=" << q.lastQuery()
                   << " boundKeys=" << bvs.keys();
        sendJson(sock, {{"type","login_result"},{"success",false},{"message","数据库错误(exec)"}});
        return;
    }

    if (!q.next()) {
        sendJson(sock, {{"type","login_result"},{"success",false},{"message","账号不存在或已停用"}});
        return;
    }

    const QString stored = q.value("password_hash").toString(); // 你现在用明文就直接比
    if (stored != password) {
        sendJson(sock, {{"type","login_result"},{"success",false},{"message","密码错误"}});
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
        QSqlQuery qp(db);
        qp.prepare("SELECT name, username FROM patients WHERE patient_id=:id");
        qp.bindValue(":id", pid);
        if (qp.exec() && qp.next()) {
            payload.insert("patient_id", pid);
            payload.insert("name", qp.value("name").toString());
            payload.insert("nickname", qp.value("username").toString());
        }
    } else if (role == "doctor") {
        const int did = q.value("doctor_id").toInt();
        QSqlQuery qd(db);
        qd.prepare(
            "SELECT d.name, d.title, dep.name AS department "
            "FROM doctors d JOIN departments dep ON dep.department_id=d.department_id "
            "WHERE d.doctor_id=:id"
        );
        qd.bindValue(":id", did);
        if (qd.exec() && qd.next()) {
            payload.insert("doctor_id", did);
            payload.insert("name", qd.value("name").toString());
            payload.insert("title", qd.value("title").toString());
            payload.insert("department", qd.value("department").toString());
        }
    }

    sendJson(sock, payload);
}

void Widget::handleRegister(QTcpSocket *sock, const QJsonObject &obj)
{
    qDebug() << "[register] raw json =" << obj;

    const QString username = obj.value("username").toString().trimmed();
    const QString password = obj.value("password").toString();              // 明文直接存到 password_hash
    const QString role     = obj.value("user_type").toString().trimmed();   // 前端传 user_type
    const QString name     = obj.value("name").toString().trimmed();
    const QString phone    = obj.value("phone").toString().trimmed();       // patients.mobile
    const QString email    = obj.value("email").toString().trimmed();

    if (username.isEmpty() || password.isEmpty() || role != "patient") {
        sendJson(sock, {{"type","register_result"},{"success",false},{"message","仅支持患者注册"}});
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << "[register] driver =" << db.driverName() << " username =" << username;

    // 账户重名检查（accounts.username 唯一）
    {
        QSqlQuery q(db);
        const char *sql = "SELECT 1 FROM accounts WHERE username=:u LIMIT 1";
        if (!q.prepare(sql)) {
            qWarning() << "[register] prepare(check account) failed:" << q.lastError().text() << " query=" << sql;
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误(prepare)"}});
            return;
        }
        q.bindValue(":u", username);
        if (!q.exec()) {
            qWarning() << "[register] exec(check account) error:" << q.lastError().text();
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误"}});
            return;
        }
        if (q.next()) {
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","用户名已存在"}});
            return;
        }
    }

    // 患者档案重名检查（patients.username 唯一）
    {
        QSqlQuery q(db);
        const char *sql = "SELECT 1 FROM patients WHERE username=:u LIMIT 1";
        if (!q.prepare(sql)) {
            qWarning() << "[register] prepare(check patient) failed:" << q.lastError().text() << " query=" << sql;
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误(prepare)"}});
            return;
        }
        q.bindValue(":u", username);
        if (!q.exec()) {
            qWarning() << "[register] exec(check patient) error:" << q.lastError().text();
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误"}});
            return;
        }
        if (q.next()) {
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","该用户已存在患者档案，请联系管理员处理"}});
            return;
        }
    }

    if (!db.transaction()) {
        qWarning() << "[register] begin transaction failed:" << db.lastError().text();
        sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误"}});
        return;
    }

    int pid = -1;

    // 1) 插入 patients（列名：username, name, mobile, email）
    {
        QSqlQuery ins(db);
        const char *sql =
            "INSERT INTO patients(username, name, mobile, email) "
            "VALUES(:u, :n, :m, :e)";
        if (!ins.prepare(sql)) {
            qWarning() << "[register] prepare(insert patient) failed:" << ins.lastError().text() << " query=" << sql;
            db.rollback();
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误(prepare)"}});
            return;
        }
        ins.bindValue(":u", username);
        ins.bindValue(":n", name.isEmpty() ? username : name);
        ins.bindValue(":m", phone);
        ins.bindValue(":e", email);

        if (!ins.exec()) {
            const QString err = ins.lastError().text();
            qWarning() << "[register] insert patient error:" << err;
            db.rollback();
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","创建患者失败: " + err}});
            return;
        }
        pid = ins.lastInsertId().toInt();   // SQLite 可直接取
        qDebug() << "[register] new patient_id =" << pid;
    }

    // 2) 插入 accounts（列名：password_hash）
    {
        QSqlQuery ins(db);
        const char *sql =
            "INSERT INTO accounts(username, password_hash, role, patient_id, is_active) "
            "VALUES(:u, :p, 'patient', :pid, 1)";
        if (!ins.prepare(sql)) {
            qWarning() << "[register] prepare(insert account) failed:" << ins.lastError().text() << " query=" << sql;
            db.rollback();
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误(prepare)"}});
            return;
        }
        ins.bindValue(":u", username);
        ins.bindValue(":p", password);  // 明文直接写到 password_hash
        ins.bindValue(":pid", pid);

        if (!ins.exec()) {
            const QString err = ins.lastError().text();
            qWarning() << "[register] insert account error:" << err;
            db.rollback();
            sendJson(sock, {{"type","register_result"},{"success",false},{"message","创建账户失败: " + err}});
            return;
        }
    }

    if (!db.commit()) {
        const QString err = db.lastError().text();
        qWarning() << "[register] commit failed:" << err;
        db.rollback();
        sendJson(sock, {{"type","register_result"},{"success",false},{"message","数据库错误: " + err}});
        return;
    }

    // ✅ 别忘了给前端回成功包
    sendJson(sock, {
        {"type","register_result"},
        {"success",true},
        {"message","注册成功"},
        {"role","patient"},
        {"patient_id", pid},
        {"name", name.isEmpty() ? username : name}
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
