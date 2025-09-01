#include "doctor_main_window.h"
#include "attendance_widget.h"
#include "patient_management_widget.h"
#include "doctor_profile_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QTimer>
#include <QSvgWidget>
#include <QDebug>
#include <QStackedWidget>
#include <QStyle> // 新增：用于刷新样式

// --- 嵌入式SVG图标数据 ---
static const char* userIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* usersIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"></path><circle cx="9" cy="7" r="4"></circle><path d="M23 21v-2a4 4 0 0 0-3-3.87"></path><path d="M16 3.13a4 4 0 0 1 0 7.75"></path></svg>)";
static const char* calendarIcon = R"(<svg xmlns="http://www.w.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line></svg>)";
static const char* messageIcon = R"(<svg xmlns="http://www.w.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z"></path></svg>)";
static const char* fileTextIcon = R"(<svg xmlns="http://www.w.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="16" y1="13" x2="8" y2="13"></line><line x1="16" y1="17" x2="8" y2="17"></line><polyline points="10 9 9 9 8 9"></polyline></svg>)";
static const char* clipboardIcon = R"(<svg xmlns="http://www.w.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"></path><rect x="8" y="2" width="8" height="4" rx="1" ry="1"></rect></svg>)";
static const  char* editIcon = R"(<svg xmlns="http://www.w.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path><path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path></svg>)";

// --- UserProfileWidget 实现 ---
UserProfileWidget::UserProfileWidget(QWidget* parent) : QFrame(parent)
{
    this->setObjectName("userProfileWidget");
    this->setCursor(Qt::PointingHandCursor);

    // 垂直布局，上方是信息，下方是按钮
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 10);
    mainLayout->setSpacing(8);

    // 上方信息部分 (头像 + 名字)
    QWidget* infoWidget = new QWidget();
    QHBoxLayout* infoLayout = new QHBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(10);

    QSvgWidget* userIconWidget = new QSvgWidget();
    // 使用深色图标以在浅色悬浮背景上可见
    QString darkUserIcon = QString(userIcon).replace("stroke=\"white\"", "stroke=\"#555555\"");
    userIconWidget->load(darkUserIcon.toUtf8());
    userIconWidget->setFixedSize(24, 24);

    QLabel* userNameLabel = new QLabel("王医生");
    userNameLabel->setObjectName("userNameLabel");

    infoLayout->addWidget(userIconWidget);
    infoLayout->addWidget(userNameLabel);
    infoLayout->addStretch();

    // 退出登录按钮 (初始隐藏)
    logoutButton = new QPushButton("退出登录");
    logoutButton->setObjectName("logoutButton");
    logoutButton->hide(); // 默认隐藏
    connect(logoutButton, &QPushButton::clicked, this, &UserProfileWidget::logoutClicked);

    mainLayout->addWidget(infoWidget);
    mainLayout->addWidget(logoutButton, 0, Qt::AlignCenter);
}

void UserProfileWidget::enterEvent(QEvent* event)
{
    // 设置属性以便QSS识别悬浮状态
    this->setProperty("hover", true);
    logoutButton->show();
    // 刷新样式
    style()->unpolish(this);
    style()->polish(this);
    QFrame::enterEvent(event);
}

void UserProfileWidget::leaveEvent(QEvent* event)
{
    this->setProperty("hover", false);
    logoutButton->hide();
    // 刷新样式
    style()->unpolish(this);
    style()->polish(this);
    QFrame::leaveEvent(event);
}

// --- DoctorMainWindow 实现 ---
DoctorMainWindow::DoctorMainWindow(QWidget *parent)
        : QWidget(parent)
{
    initUI();
    applyStyles();

    // 初始化并启动时钟
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DoctorMainWindow::updateClock);
    timer->start(1000);
    updateClock();
}

DoctorMainWindow::~DoctorMainWindow() {}

void DoctorMainWindow::initUI() {
    this->setObjectName("doctorMainWindow");
    this->setWindowTitle("智慧医院医生工作平台");
    this->resize(1024, 768);

    centralStack = new QStackedWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(centralStack);

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

    // **已修改：使用新的 UserProfileWidget**
    UserProfileWidget *userWidget = new UserProfileWidget();
    connect(userWidget, &UserProfileWidget::logoutClicked, this, &DoctorMainWindow::onLogoutClicked);

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

    QPushButton *btnPersonalInfo = qobject_cast<QPushButton*>(createDashboardButton(userIcon, "个人信息", "查看和编辑医生个人资料", "btnPersonalInfo"));
    connect(btnPersonalInfo, &QPushButton::clicked, this, &DoctorMainWindow::showProfilePage);

    QPushButton *btnPatientInfo = qobject_cast<QPushButton*>(createDashboardButton(usersIcon, "患者信息", "查看和管理患者资料与病历", "btnPatientInfo"));
    connect(btnPatientInfo, &QPushButton::clicked, this, &DoctorMainWindow::showPatientManagementPage);

    QPushButton *btnAttendance = qobject_cast<QPushButton*>(createDashboardButton(calendarIcon, "考勤管理", "打卡签到、排班安排", "btnAttendance"));
    connect(btnAttendance, &QPushButton::clicked, this, &DoctorMainWindow::showAttendancePage);

    QPushButton *btnCommunication = qobject_cast<QPushButton*>(createDashboardButton(messageIcon, "医患沟通", "与患者在线交流、回答咨询", "btnCommunication"));
    connect(btnCommunication, &QPushButton::clicked, [](){ qDebug() << "医患沟通 clicked"; });

    QPushButton *btnRecords = qobject_cast<QPushButton*>(createDashboardButton(fileTextIcon, "病历管理", "创建、查看和编辑患者病历", "btnRecords"));
    connect(btnRecords, &QPushButton::clicked, [](){ qDebug() << "病历管理 clicked"; });

    QPushButton *btnOrders = qobject_cast<QPushButton*>(createDashboardButton(clipboardIcon, "医嘱管理", "开具、查看和管理患者医嘱", "btnOrders"));
    connect(btnOrders, &QPushButton::clicked, [](){ qDebug() << "医嘱管理 clicked"; });

    QPushButton *btnPrescription = qobject_cast<QPushButton*>(createDashboardButton(editIcon, "处方管理", "开具、审核和管理电子处方", "btnPrescription"));
    connect(btnPrescription, &QPushButton::clicked, [](){ qDebug() << "处方管理 clicked"; });

    gridLayout->addWidget(btnPersonalInfo, 0, 0);
    gridLayout->addWidget(btnPatientInfo, 0, 1);
    gridLayout->addWidget(btnAttendance, 0, 2);
    gridLayout->addWidget(btnCommunication, 0, 3);
    gridLayout->addWidget(btnRecords, 1, 0);
    gridLayout->addWidget(btnOrders, 1, 1);
    gridLayout->addWidget(btnPrescription, 1, 2);

    return gridContainer;
}

QWidget* DoctorMainWindow::createDashboardButton(const QString &svgIconData, const QString &title, const QString &subtitle, const QString &objectName) {
    QPushButton *button = new QPushButton();
    button->setObjectName(objectName);
    button->setMinimumSize(220, 150);
    button->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *layout = new QVBoxLayout(button);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);

    QSvgWidget *icon = new QSvgWidget();
    icon->load(QByteArray(svgIconData.toUtf8()));
    icon->setFixedSize(48, 48);
    icon->setAttribute(Qt::WA_TransparentForMouseEvents);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("buttonTitle");
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    QLabel *subtitleLabel = new QLabel(subtitle);
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
    QDateTime current = QDateTime::currentDateTime();
    if (timeLabel) timeLabel->setText(current.toString("hh:mm:ss"));
    if (dateLabel) dateLabel->setText(current.toString("yyyy.MM.dd dddd"));
}

void DoctorMainWindow::showProfilePage() {
    if (!profilePage) {
        profilePage = new DoctorProfileWidget();
        connect(profilePage, &DoctorProfileWidget::backRequested, this, &DoctorMainWindow::showDashboardPage);
        centralStack->addWidget(profilePage);
    }
    centralStack->setCurrentWidget(profilePage);
}

void DoctorMainWindow::showPatientManagementPage() {
    if (!patientManagementPage) {
        patientManagementPage = new PatientManagementWidget();
        connect(patientManagementPage, &PatientManagementWidget::backRequested, this, &DoctorMainWindow::showDashboardPage);
        centralStack->addWidget(patientManagementPage);
    }
    centralStack->setCurrentWidget(patientManagementPage);
}

void DoctorMainWindow::showDashboardPage() {
    centralStack->setCurrentWidget(dashboardPage);
}

void DoctorMainWindow::showAttendancePage() {
    if (!attendancePage) {
        attendancePage = new AttendanceWidget();
        connect(attendancePage, &AttendanceWidget::backRequested, this, &DoctorMainWindow::showDashboardPage);
        centralStack->addWidget(attendancePage);
    }
    centralStack->setCurrentWidget(attendancePage);
}

// **新增：实现登出槽函数**
void DoctorMainWindow::onLogoutClicked()
{
    qDebug() << "请求退出登录，关闭窗口。";
    this->close();
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
            color: #555555;
            font-weight: bold;
        }
        #footerLabel {
            font-size: 14px;
            color: #AAAAAA;
        }

        /* --- 新增: 用户资料控件样式 --- */
        #userProfileWidget {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 8px;
            padding: 5px;
        }
        #userProfileWidget[hover="true"] {
            background-color: #EAECEE; /* 悬浮时的浅灰色背景 */
            border: 1px solid #D5D8DC;
        }
        #logoutButton {
            background-color: #E74C3C;
            color: white;
            font-size: 13px;
            font-weight: bold;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
        }
        #logoutButton:hover {
            background-color: #C0392B;
        }
        #logoutButton:pressed {
            background-color: #A93226;
        }


        /* --- 仪表盘按钮样式 --- */
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

        /* --- 各个按钮颜色 --- */
        #btnPersonalInfo { background-color: #3498DB; }
        #btnPatientInfo { background-color: #2ECC71; }
        #btnAttendance { background-color: #9B59B6; }
        #btnCommunication { background-color: #F39C12; }
        #btnRecords { background-color: #5D6D7E; }
        #btnOrders { background-color: #E74C3C; }
        #btnPrescription { background-color: #1ABC9C; }
    )");
}