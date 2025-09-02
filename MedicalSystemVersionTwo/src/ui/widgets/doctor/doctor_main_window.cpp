#include "doctor_main_window.h"
#include "attendance_widget.h"
#include "medical_record_dialog.h"
#include "patient_management_widget.h"
#include "medical_orders_dialog.h"
#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QTimer>
#include <QSvgWidget>
#include <QDebug>
#include "doctor_profile_widget.h"
#include <QStackedWidget>
#include <QMessageBox>

// 新增：用到 QJsonObject/QJsonArray 的 lambda 需要这个
#include <QJsonObject>
#include <QJsonArray>

// === 新增：UI 版本里的类/头 ===
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QCursor>
#include <QPoint>

// 如果你项目里文件名不同，请把下面两个 include 替换为实际头文件名
#include "doctor_communication_widget.h"
#include "prescription_widget.h"

// --- 嵌入式SVG图标数据（保持你的主代码版本，白色描边，不动） ---
static const char* userIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* usersIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"></path><circle cx="9" cy="7" r="4"></circle><path d="M23 21v-2a4 4 0 0 0-3-3.87"></path><path d="M16 3.13a4 4 0 0 1 0 7.75"></path></svg>)";
static const char* calendarIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line></svg>)";
static const char* messageIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z"></path></svg>)";
static const char* fileTextIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="16" y1="13" x2="8" y2="13"></line><line x1="16" y1="17" x2="8" y2="17"></line><polyline points="10 9 9 9 8 9"></polyline></svg>)";
static const char* clipboardIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"></path><rect x="8" y="2" width="8" height="4" rx="1" ry="1"></rect></svg>)";
static const  char* editIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path><path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path></svg>)";

DoctorMainWindow::DoctorMainWindow(Widget* api, int doctorId, const QString& name, QWidget *parent)
    : QMainWindow(parent), api_(api), doctorId_(doctorId), name_(name)
{
    setWindowTitle(QString("医生工作台 - %1 (#%2)").arg(name_).arg(doctorId_));
    initUI();
    applyStyles();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DoctorMainWindow::updateClock);
    timer->start(1000);
    updateClock();

    userMenu = new QMenu(this);
    userMenu->addAction("个人中心", this, &DoctorMainWindow::showProfilePage);
    userMenu->addSeparator();
    QAction *logoutAction = userMenu->addAction("退出登录");
    connect(logoutAction, &QAction::triggered, this, &DoctorMainWindow::onLogoutClicked);

    // ★ userNameLabel 现为成员，直接判空使用
    if (userNameLabel) {
        userNameLabel->setCursor(Qt::PointingHandCursor);
        userNameLabel->installEventFilter(this);
    }
}
DoctorMainWindow::~DoctorMainWindow() = default;
void DoctorMainWindow::initUI() {
    this->setObjectName("doctorMainWindow");
    this->resize(1024, 768);

    centralStack = new QStackedWidget(this);
    setCentralWidget(centralStack);

    // --- 仪表盘页面 ---
    dashboardPage = new QWidget();
    QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboardPage);
    dashboardLayout->setContentsMargins(40, 20, 40, 20);
    dashboardLayout->setSpacing(25);

    dashboardLayout->addWidget(createHeaderWidget());
    dashboardLayout->addStretch();
    dashboardLayout->addWidget(createGridWidget());
    dashboardLayout->addStretch();
    dashboardLayout->addWidget(createFooterWidget());

    centralStack->addWidget(dashboardPage);

    profilePage = nullptr;
    attendancePage = nullptr;
    patientManagementPage = nullptr;

    // ★ 统一成员名
    communicationWidget = nullptr;
    prescriptionPage = nullptr;
}

QWidget* DoctorMainWindow::createHeaderWidget() {
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(headerWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *titleLabel = new QLabel("智慧医院医生工作平台");
    titleLabel->setObjectName("headerTitleLabel");

    QWidget *timeDateWidget = new QWidget();
    QVBoxLayout *timeDateLayout = new QVBoxLayout(timeDateWidget);
    timeDateLayout->setContentsMargins(0,0,0,0);
    timeDateLayout->setSpacing(5);
    timeLabel = new QLabel();
    timeLabel->setObjectName("timeLabel");
    timeLabel->setAlignment(Qt::AlignCenter);
    dateLabel = new QLabel();
    dateLabel->setObjectName("dateLabel");
    dateLabel->setAlignment(Qt::AlignCenter);
    timeDateLayout->addWidget(timeLabel);
    timeDateLayout->addWidget(dateLabel);

    QWidget *userWidget = new QWidget();
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(0,0,0,0);
    userLayout->setSpacing(10);
    auto *userIconWidget = new QSvgWidget();
    userIconWidget->load(QByteArray(userIcon));
    userIconWidget->setFixedSize(24, 24);

    // ★ 成员赋值
    userNameLabel = new QLabel(name_.isEmpty() ? "医生" : name_);
    userNameLabel->setObjectName("userNameLabel");
    userLayout->addWidget(userIconWidget);
    userLayout->addWidget(userNameLabel);

    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(timeDateWidget);
    layout->addStretch();
    layout->addWidget(userWidget);
    return headerWidget;
}

QWidget* DoctorMainWindow::createGridWidget() {
    QWidget *gridContainer = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(30);

    auto *btnPersonalInfo  = qobject_cast<QPushButton*>(createDashboardButton(userIcon,     "个人信息", "查看和编辑医生个人资料", "btnPersonalInfo"));
    auto *btnPatientInfo   = qobject_cast<QPushButton*>(createDashboardButton(usersIcon,    "患者信息", "查看和管理患者资料与病历", "btnPatientInfo"));
    auto *btnAttendance    = qobject_cast<QPushButton*>(createDashboardButton(calendarIcon, "考勤管理", "打卡签到、排班安排", "btnAttendance"));
    auto *btnCommunication = qobject_cast<QPushButton*>(createDashboardButton(messageIcon,  "医患沟通", "与患者在线交流、回答咨询", "btnCommunication"));
    auto *btnRecords       = qobject_cast<QPushButton*>(createDashboardButton(fileTextIcon, "病历管理", "创建、查看和编辑患者病历", "btnRecords"));
    auto *btnOrders        = qobject_cast<QPushButton*>(createDashboardButton(clipboardIcon,"医嘱管理", "开具、查看和管理患者医嘱", "btnOrders"));
    auto *btnPrescription  = qobject_cast<QPushButton*>(createDashboardButton(editIcon,     "处方管理", "开具、审核和管理电子处方", "btnPrescription"));

    gridLayout->addWidget(btnPersonalInfo,  0, 0);
    gridLayout->addWidget(btnPatientInfo,   0, 1);
    gridLayout->addWidget(btnAttendance,    0, 2);
    gridLayout->addWidget(btnCommunication, 0, 3);
    gridLayout->addWidget(btnRecords,       1, 0);
    gridLayout->addWidget(btnOrders,        1, 1);
    gridLayout->addWidget(btnPrescription,  1, 2);

    // === 只连你的页面切换槽（保留） ===
    connect(btnPersonalInfo, &QPushButton::clicked, this, &DoctorMainWindow::showProfilePage);
    connect(btnAttendance,   &QPushButton::clicked, this, &DoctorMainWindow::showAttendancePage);
    connect(btnPatientInfo,  &QPushButton::clicked, this, &DoctorMainWindow::showPatientManagementPage);

    // === 新增：对 UI 版新增页面的连接（不影响原有逻辑） ===
    connect(btnCommunication, &QPushButton::clicked, this, &DoctorMainWindow::showCommunicationPage);
    connect(btnPrescription,  &QPushButton::clicked, this, &DoctorMainWindow::showPrescriptionPage);

    return gridContainer;
}

QPushButton* DoctorMainWindow::createDashboardButton(const QString &svgIconData,
                                                     const QString &title,
                                                     const QString &subtitle,
                                                     const QString &objectName) {
    auto *button = new QPushButton();
    button->setObjectName(objectName);
    button->setMinimumSize(220, 150);
    button->setCursor(Qt::PointingHandCursor);

    auto *layout = new QVBoxLayout(button);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);

    auto *icon = new QSvgWidget();
    icon->load(QByteArray(svgIconData.toUtf8()));
    icon->setFixedSize(48, 48);
    icon->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto *titleLabel = new QLabel(title);
    titleLabel->setObjectName("buttonTitle");
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto *subtitleLabel = new QLabel(subtitle);
    subtitleLabel->setObjectName("buttonSubtitle");
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addStretch();
    layout->addWidget(icon, 0, Qt::AlignCenter);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);
    layout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    layout->addStretch();
    return button;
}

QWidget* DoctorMainWindow::createFooterWidget() {
    QWidget *footerWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(footerWidget);
    QLabel *footerLabel = new QLabel("智慧医疗医生工作平台");
    footerLabel->setObjectName("footerLabel");
    footerLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(footerLabel);
    return footerWidget;
}

void DoctorMainWindow::updateClock() {
    const auto current = QDateTime::currentDateTime();
    if (timeLabel) timeLabel->setText(current.toString("hh:mm:ss"));
    if (dateLabel) dateLabel->setText(current.toString("yyyy.MM.dd dddd"));
}

// === 以下三个保持你的主逻辑（包含 api_ 信号/槽） ===
void DoctorMainWindow::showProfilePage() {
    if (!profilePage) {
        profilePage = new DoctorProfileWidget();
        connect(profilePage, &DoctorProfileWidget::backRequested, this, &DoctorMainWindow::showDashboardPage);
        // Widget → Profile
        connect(api_, &Widget::currentDataDoctorOk,     profilePage, &DoctorProfileWidget::setDoctorProfile);
        connect(api_, &Widget::currentDataDoctorFailed, profilePage, &DoctorProfileWidget::showError);
        // Profile → Widget
        connect(profilePage, &DoctorProfileWidget::saveRequested, api_, &Widget::sendUpdateDoctorProfile);

        // ⭐ 新增：保存回执 → 控件提示
        connect(api_, &Widget::updateDoctorProfileDone,
                profilePage, &DoctorProfileWidget::onSaveResult);

        // ⭐ 新增：保存成功后，主动重新拉我的资料，保证展示是服务器最新
        connect(api_, &Widget::updateDoctorProfileDone, this, [this](bool ok, const QString &){
            if (ok) api_->loadCurrentDoctorDataByDoctorId(doctorId_);
        });

        centralStack->addWidget(profilePage);
        api_->loadCurrentDoctorDataByDoctorId(doctorId_);  // 拉取一次
    }
    centralStack->setCurrentWidget(profilePage);
}

void DoctorMainWindow::showPatientManagementPage() {
    if (!patientManagementPage) {
        patientManagementPage = new PatientManagementWidget();
        connect(patientManagementPage, &PatientManagementWidget::backRequested,
                this, &DoctorMainWindow::showDashboardPage);

        // 列表数据
        connect(api_, &Widget::loadPatientListOk,
                patientManagementPage, &PatientManagementWidget::setPatientList);

        // 条目 -> 打开对话框/拉取数据
        connect(patientManagementPage, &PatientManagementWidget::openProfileRequested,
                api_, &Widget::sendGetPatientProfile);

        connect(patientManagementPage, &PatientManagementWidget::openMedicalRecordRequested,
                this, [this](int pid, const QString &name){ openMedicalRecordDialog(pid, name); });

        connect(patientManagementPage, &PatientManagementWidget::openMedicalOrdersRequested,
                this, [this](int pid, const QString &name){ openMedicalOrdersDialog(pid, name); });

        connect(patientManagementPage, &PatientManagementWidget::refreshItemRequested,
                this, [this](int pid){
                    api_->sendGetPatientProfile(pid);
                    api_->loadMedicalRecord(pid);
                    api_->loadMedicalOrders(pid);
                });

        // ★ 修复：以下三个是 PatientManagementWidget 的 private 成员，不能直接 connect
        // connect(api_, &Widget::patientProfileLoaded, patientManagementPage, &PatientManagementWidget::updateListItemProfile);
        // connect(api_, &Widget::medicalRecordLoaded,  patientManagementPage, &PatientManagementWidget::updateListItemRecordFlag);
        // connect(api_, &Widget::medicalOrdersLoaded,  patientManagementPage, &PatientManagementWidget::updateListItemOrdersFlag);

        // 可保留：用于右侧详情卡
        connect(patientManagementPage, &PatientManagementWidget::requestLoadPatientProfile,
                api_, &Widget::sendGetPatientProfile);
        connect(api_, &Widget::patientProfileLoaded,
                patientManagementPage, &PatientManagementWidget::setPatientProfile);

        centralStack->addWidget(patientManagementPage);
    }
    centralStack->setCurrentWidget(patientManagementPage);
    api_->loadPatientList();
}
// ★ 补上声明对应的定义（最小实现即可通过编译）
void DoctorMainWindow::openMedicalRecordDialog(int patientId, const QString &patientName) {
    auto *dlg = new MedicalRecordDialog(patientId, patientName, this);

    // 典型接线（如项目已有可按需补全/对齐）
    connect(dlg, &MedicalRecordDialog::requestLoadRecord,
            this, [this](int pid){ api_->loadMedicalRecord(pid); });
    connect(dlg, &MedicalRecordDialog::requestSaveRecord,
            this, [this](const QJsonObject &obj){ api_->saveMedicalRecord(obj); });

    connect(api_, &Widget::medicalRecordLoaded,
            dlg,  &MedicalRecordDialog::onRecordLoaded);
    connect(api_, &Widget::medicalRecordSaved,
            dlg,  &MedicalRecordDialog::onRecordSaved);

    // 进入即加载
    api_->loadMedicalRecord(patientId);
    dlg->open();
}
void DoctorMainWindow::openMedicalOrdersDialog(int patientId, const QString &patientName) {
    auto *dlg = new MedicalOrdersDialog(patientId, patientName, this);
    connect(dlg, &MedicalOrdersDialog::requestLoadOrders,
            this, [this](int pid, int orderId){ api_->loadMedicalOrders(pid, orderId); });
    connect(dlg, &MedicalOrdersDialog::requestSaveOrders,
            this, [this](const QJsonObject &payload){ if (!payload.isEmpty()) api_->saveMedicalOrders(payload); });

    connect(api_, &Widget::medicalOrdersLoaded, dlg, &MedicalOrdersDialog::onOrdersLoaded);
    connect(api_, &Widget::medicalOrdersSaved, dlg, &MedicalOrdersDialog::onOrdersSaved);

    api_->loadMedicalOrders(patientId, 0);
    dlg->open();
}
void DoctorMainWindow::showAttendancePage() {
    if (!attendancePage) {
        attendancePage = new AttendanceWidget();
        connect(attendancePage, &AttendanceWidget::backRequested,
                this, &DoctorMainWindow::showDashboardPage);

        // === Widget → Page ===
        connect(api_, &Widget::leaveRecordsLoaded,
                attendancePage, &AttendanceWidget::onLeaveRecordsLoaded);

        connect(api_, &Widget::attendanceTodayLoaded,
                attendancePage, &AttendanceWidget::onAttendanceTodayLoaded);

        connect(api_, &Widget::clockEventDone,
                attendancePage, &AttendanceWidget::onClockEventDone);

        // === Page → Main → Widget ===
        connect(attendancePage, &AttendanceWidget::requestLoadAttendanceToday,
                this, [this]{ api_->loadAttendanceToday(doctorId_); });

        connect(attendancePage, &AttendanceWidget::requestClockEvent,
                this, [this](const QString &kind){ api_->clockEvent(doctorId_, kind); });

        connect(attendancePage, &AttendanceWidget::requestSubmitLeave,
                this, [this](const QString &leaveType,
                             const QString &start,
                             const QString &end,
                             const QString &reason){
                    api_->submitLeave(doctorId_, leaveType, start, end, reason);
                });

        // 已有：销假占位
        connect(attendancePage, &AttendanceWidget::requestCancelLeave,
                this, [this](int leaveId){
                    // api_->revokeLeave(doctorId_, leaveId); // 等后端接口
                    qDebug() << "[UI] requestCancelLeave" << leaveId << "(待接入服务器接口)";
                });

        // 留在你原逻辑：请假提交结果 -> 弹窗 & 刷新列表
        connect(api_, &Widget::leaveSubmitted, this, [this](const QJsonObject &obj){
            const bool ok = obj.value("success").toBool(true);
            if (ok) {
                QMessageBox::information(this, "已提交", "请假提交成功。");
                api_->loadLeaveRecords(doctorId_);
            } else {
                QMessageBox::warning(this, "提交失败",
                                     obj.value("error").toString("未知错误"));
            }
        });

        centralStack->addWidget(attendancePage);
    }

    centralStack->setCurrentWidget(attendancePage);

    // 进入页面：拉取今日考勤与请假记录
    api_->loadAttendanceToday(doctorId_);
    api_->loadLeaveRecords(doctorId_);
}
void DoctorMainWindow::showCommunicationPage() {
    if (!communicationWidget) {
        communicationWidget = new CommunicationWidget();
        centralStack->addWidget(communicationWidget);

        // === Widget → UI ===
        connect(api_, &Widget::doctorContactsLoadedOk,
                communicationWidget, &CommunicationWidget::setContacts);
        connect(api_, &Widget::doctorContactsLoadedFailed,
                communicationWidget, &CommunicationWidget::showError);

        connect(api_, &Widget::chatHistoryLoadedOk,
                communicationWidget, &CommunicationWidget::setMessages);
        connect(api_, &Widget::chatHistoryLoadedFailed,
                communicationWidget, &CommunicationWidget::showError);

        connect(api_, &Widget::messageSentOk,
                communicationWidget, &CommunicationWidget::appendMessage);
        connect(api_, &Widget::messageSentFailed,
                communicationWidget, &CommunicationWidget::showError);

        // === UI → Widget（经由 Main）===
        connect(communicationWidget, &CommunicationWidget::requestLoadContacts,
                this, [this]{ api_->loadDoctorContactsForDoctor(doctorId_); });
        connect(communicationWidget, &CommunicationWidget::requestOpenConversation,
                this, [this](int convId){ api_->loadChatHistoryByConversation(convId, 200); });
        connect(communicationWidget, &CommunicationWidget::requestSendMessageByConv,
                this, [this](int convId, const QString &text){
                    api_->sendChatMessageByConversation(convId, "doctor", text);
                });
        connect(communicationWidget, &CommunicationWidget::requestSendMessageByPeer,
                this, [this](int patientId, const QString &text){
                    api_->sendChatMessageByPeer(patientId, doctorId_, "doctor", text);
                });

        connect(communicationWidget, &CommunicationWidget::backRequested,
                this, &DoctorMainWindow::showDashboardPage);
    }

    api_->loadDoctorContactsForDoctor(doctorId_);
    centralStack->setCurrentWidget(communicationWidget);
}

void DoctorMainWindow::showPrescriptionPage() {
    if (!prescriptionPage) {
        prescriptionPage = new PrescriptionWidget();
        connect(prescriptionPage, &PrescriptionWidget::backRequested,
                this, &DoctorMainWindow::showDashboardPage);
        centralStack->addWidget(prescriptionPage);

        // === API → UI ===
        connect(api_, &Widget::loadPatientListOk,
                prescriptionPage, &PrescriptionWidget::setPatientList);

        connect(api_, &Widget::loadMedicineDataOk,
                prescriptionPage, &PrescriptionWidget::setMedicineCatalog);

        // === UI → API ===
        // 暂存：放到本地购物车（Widget 内部会 emit addToCartOk）
        connect(prescriptionPage, &PrescriptionWidget::saveRequested,
                this, [this](const QJsonObject &){
            const int pid = prescriptionPage->currentPatientId();
            if (pid < 0) { QMessageBox::warning(this, "提示", "请先选择患者"); return; }
            const QJsonArray cart = prescriptionPage->cartFromCurrentSelection();
            api_->addToCart(cart, 0);
        });

        // 开具处方：提交到后端（Widget 会发 onPurchaseClickedOk）
        connect(prescriptionPage, &PrescriptionWidget::issueRequested,
                this, [this](const QJsonObject &){
            const int pid = prescriptionPage->currentPatientId();
            if (pid < 0) { QMessageBox::warning(this, "提示", "请先选择患者"); return; }
            const QJsonArray cart = prescriptionPage->cartFromCurrentSelection();
            api_->onPurchaseClicked(pid, cart, 0);
        });

        // 结果反馈
        connect(api_, &Widget::addToCartOk, this, [this](const QJsonObject &order){
            Q_UNUSED(order);
            QMessageBox::information(this, "已暂存", "处方已暂存到本地订单草稿。");
        });
        connect(api_, &Widget::onPurchaseClickedOk, this, [this](const QJsonObject &resp){
            if (resp.value("success").toBool(true))
                QMessageBox::information(this, "提交成功", "处方已提交至药房/收费。");
            else
                QMessageBox::warning(this, "提交失败", resp.value("error").toString("未知错误"));
        });
    }

    centralStack->setCurrentWidget(prescriptionPage);

    // 进入页面自动拉取患者与药品目录
    api_->loadPatientList();
    api_->loadMedicineData();
}
void DoctorMainWindow::showDashboardPage() {
    centralStack->setCurrentWidget(dashboardPage);
}

// === 用户菜单 ===
void DoctorMainWindow::showUserMenu() {
    if (userNameLabel && userMenu) {
        const QPoint pos = userNameLabel->mapToGlobal(
            QPoint(userNameLabel->width() - userMenu->sizeHint().width(), userNameLabel->height()));
        userMenu->popup(pos);
    }
}
void DoctorMainWindow::onLogoutClicked() {
    qDebug() << "请求退出登录，关闭窗口。";
    this->close();
}

bool DoctorMainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == userNameLabel) {
        if (event->type() == QEvent::Enter) {
            showUserMenu();
            return true;
        } else if (event->type() == QEvent::Leave) {
            QTimer::singleShot(150, this, [this](){
                if (userNameLabel &&
                    !userNameLabel->geometry().contains(userNameLabel->mapFromGlobal(QCursor::pos())) &&
                    userMenu && !userMenu->geometry().contains(QCursor::pos())) {
                    userMenu->hide();
                }
            });
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void DoctorMainWindow::applyStyles() {
    this->setStyleSheet(R"(
        #doctorMainWindow {
            background-color: #F5F7FA;
            font-family: 'Microsoft YaHei', sans-serif;
        }
        #headerTitleLabel {
            font-size: 24px;
            font-weight: bold;
            color: #333333;
        }
        #timeLabel {
            font-size: 28px;
            font-weight: bold;
            color: #333333;
        }
        #dateLabel {
            font-size: 14px;
            color: #888888;
        }
        #userNameLabel {
            font-size: 16px;
            color: #2B6CB0; /* 适度融合 UI 版的蓝色文字 */
            font-weight: bold;
            padding: 6px 10px; /* 为悬停点击留出热区 */
            border-radius: 6px;
        }
        #userNameLabel:hover {
            background-color: #EBF8FF; /* 轻微高亮 */
        }
        #footerLabel {
            font-size: 14px;
            color: #AAAAAA;
        }

        /* --- Dashboard Button Styles --- */
        QPushButton {
            border: 1px solid transparent;
            border-radius: 15px;
            text-align: center;
            color: white;
            padding: 10px;
        }
        QPushButton:hover {
            border: 2px solid rgba(255, 255, 255, 0.8);
        }
        QPushButton:pressed {
            background-color: rgba(0, 0, 0, 0.1);
        }

        #buttonTitle {
            font-size: 18px;
            font-weight: bold;
            background: transparent;
        }
        #buttonSubtitle {
            font-size: 12px;
            color: rgba(255, 255, 255, 0.9);
            background: transparent;
        }

        /* --- Individual Button Colors --- */
        #btnPersonalInfo { background-color: #3498DB; }
        #btnPatientInfo { background-color: #2ECC71; }
        #btnAttendance { background-color: #9B59B6; }
        #btnCommunication { background-color: #F39C12; }
        #btnRecords { background-color: #5D6D7E; }
        #btnOrders { background-color: #E74C3C; }
        #btnPrescription { background-color: #1ABC9C; }

        /* === 新增：菜单样式（不影响原布局） === */
        QMenu {
            background-color: #FFFFFF;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            padding: 8px 25px 8px 15px;
            color: #2D3748;
            font-size: 14px;
        }
        QMenu::item:selected {
            background-color: #EBF8FF;
            border-radius: 5px;
        }
        QMenu::separator {
            height: 1px;
            background-color: #E2E8F0;
            margin: 5px 0px;
        }
    )");
}
