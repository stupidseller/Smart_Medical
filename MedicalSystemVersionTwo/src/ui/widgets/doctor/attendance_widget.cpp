#include "attendance_widget.h"
#include "leave_record_item_widget.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QFrame>
#include <QTimer>
#include <QDateTime>
#include <QSvgWidget>

// --- SVG Icons ---
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";
static const char* calendarIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2D3748" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line></svg>)";
static const char* clockInIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#48BB78" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"></path><circle cx="8.5" cy="7" r="4"></circle><polyline points="17 11 19 13 23 9"></polyline></svg>)";
static const char* leaveAppIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#ED8936" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line><line x1="12" y1="14" x2="12" y2="18"></line><line x1="10" y1="16" x2="14" y2="16"></line></svg>)";
static const char* leaveRecordIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#4299E1" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 3h6a4 4 0 0 1 4 4v14a3 3 0 0 0-3-3H2z"></path><path d="M22 3h-6a4 4 0 0 0-4 4v14a3 3 0 0 1 3-3h7z"></path></svg>)";


AttendanceWidget::AttendanceWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    applyStyles();

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &AttendanceWidget::updateClock);
    clockTimer->start(1000);
    updateClock();
}

void AttendanceWidget::initUI() {
    this->setObjectName("attendanceWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 30);
    mainLayout->setSpacing(20);

    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createStatusBanner());

    QHBoxLayout *panelsLayout = new QHBoxLayout();
    panelsLayout->setSpacing(20);
    panelsLayout->addWidget(createClockInPanel(), 1);
    panelsLayout->addWidget(createLeaveApplicationPanel(), 1);
    panelsLayout->addWidget(createLeaveRecordsPanel(), 1);

    mainLayout->addLayout(panelsLayout);
}

QWidget* AttendanceWidget::createHeader() {
    // (代码与DoctorProfileWidget中的createHeader类似，可以复用)
    QWidget *header = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0,0,0,0);

    QPushButton *backButton = new QPushButton(" 返回首页");
    // (此处省略了setIcon的代码，因为QSvgRenderer等头文件未包含，为保持代码简洁)
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &AttendanceWidget::backRequested);

    QLabel *title = new QLabel("考勤管理");
    title->setObjectName("pageTitle");

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    // (可添加右侧用户头像)

    return header;
}


QWidget* AttendanceWidget::createStatusBanner() {
    QWidget* banner = new QFrame();
    banner->setObjectName("statusBanner");
    QHBoxLayout* layout = new QHBoxLayout(banner);
    layout->setSpacing(15);

    QSvgWidget* icon = new QSvgWidget();
    icon->load(QByteArray(calendarIconSvg));
    icon->setFixedSize(32, 32);

    QLabel* title = new QLabel("今日考勤状态");
    title->setObjectName("bannerTitle");
    QLabel* status = new QLabel("已打卡");
    status->setObjectName("bannerStatus");

    layout->addWidget(icon);
    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(status);

    return banner;
}


QWidget* AttendanceWidget::createClockInPanel() {
    QWidget* panel = new QFrame();
    panel->setObjectName("contentPanel");
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    // ... (此处省略创建标题栏的代码)

    clockTimeLabel = new QLabel("15:02:10");
    clockTimeLabel->setObjectName("clockTimeLabel");
    clockTimeLabel->setAlignment(Qt::AlignCenter);

    clockDateLabel = new QLabel("2025年08月31日 星期日");
    clockDateLabel->setObjectName("clockDateLabel");
    clockDateLabel->setAlignment(Qt::AlignCenter);

    QPushButton* btnClockIn = new QPushButton(" 上班打卡");
    QPushButton* btnClockOut = new QPushButton(" 下班打卡");
    btnClockIn->setObjectName("clockInButton");
    btnClockOut->setObjectName("clockOutButton");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnClockIn);
    btnLayout->addWidget(btnClockOut);

    // ... (此处省略创建打卡记录部分的代码)

    layout->addWidget(clockTimeLabel);
    layout->addWidget(clockDateLabel);
    layout->addLayout(btnLayout);
    layout->addStretch();

    return panel;
}

QWidget* AttendanceWidget::createLeaveApplicationPanel() {
    QWidget* panel = new QFrame();
    panel->setObjectName("contentPanel");
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    // ... (标题栏)

    QFormLayout *form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);
    QComboBox* leaveTypeCombo = new QComboBox();
    leaveTypeCombo->addItems({"请选择请假类型", "病假", "事假", "年假"});
    QDateEdit* startDateEdit = new QDateEdit();
    startDateEdit->setCalendarPopup(true);
    QDateEdit* endDateEdit = new QDateEdit();
    endDateEdit->setCalendarPopup(true);
    QTextEdit* reasonEdit = new QTextEdit();
    reasonEdit->setPlaceholderText("请输入请假事由...");

    form->addRow("请假类型", leaveTypeCombo);
    form->addRow("开始时间", startDateEdit);
    form->addRow("结束时间", endDateEdit);
    form->addRow("请假事由", reasonEdit);

    QPushButton *submitButton = new QPushButton("提交申请");
    submitButton->setObjectName("primaryButton");

    layout->addLayout(form);
    layout->addStretch();
    layout->addWidget(submitButton);

    return panel;
}

QWidget* AttendanceWidget::createLeaveRecordsPanel() {
    QWidget* panel = new QFrame();
    panel->setObjectName("contentPanel");
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    // ... (标题栏)

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("scrollArea");

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(10);
    scrollLayout->setContentsMargins(5, 5, 5, 5);

    // 添加示例数据
    scrollLayout->addWidget(new LeaveRecordItemWidget("病假", "2023-12-10 至 2023-12-12", "感冒发烧，需要休息", LeaveRecordItemWidget::Approved));
    scrollLayout->addWidget(new LeaveRecordItemWidget("年假", "2023-12-20 至 2023-12-22", "个人年假休息", LeaveRecordItemWidget::Pending));
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    return panel;
}

void AttendanceWidget::updateClock() {
    QDateTime current = QDateTime::currentDateTime();
    if(clockTimeLabel) clockTimeLabel->setText(current.toString("HH:mm:ss"));
    if(clockDateLabel) clockDateLabel->setText(current.toString("yyyy年MM月dd日 dddd"));
}


void AttendanceWidget::applyStyles() {
    this->setStyleSheet(R"(
        #attendanceWidget {
            background-color: #F0F9FF; /* 整个控件的淡蓝色背景 */
        }
        #pageTitle { font-size: 20px; font-weight: bold; color: #2D3748; }
        #backButton { border: none; font-size: 14px; font-weight: bold; color: #4A5568; }

        #statusBanner {
            background-color: #EBF8FF;
            border-radius: 10px;
            padding: 15px;
            border-left: 5px solid #68D391; /* 绿色左边框 */
        }
        #bannerTitle { font-size: 16px; color: #4A5568; }
        #bannerStatus { font-size: 20px; font-weight: bold; color: #2D3748; }

        #contentPanel {
            background-color: white;
            border-radius: 12px;
            padding: 20px;
            /* box-shadow is not directly supported in QSS, this is a visual placeholder */
            border: 1px solid #E2E8F0;
        }

        /* --- Clock-in Panel --- */
        #clockTimeLabel { font-size: 48px; font-weight: bold; color: #2D3748; }
        #clockDateLabel { font-size: 16px; color: #718096; }
        #clockInButton, #clockOutButton {
            padding: 12px; font-size: 16px; font-weight: bold;
            border-radius: 8px; color: white;
        }
        #clockInButton { background-color: #48BB78; }
        #clockOutButton { background-color: #F56565; }

        /* --- Leave Application Panel --- */
        QComboBox, QDateEdit, QTextEdit {
            border: 1px solid #E2E8F0;
            border-radius: 6px;
            padding: 8px;
            font-size: 14px;
        }
        QTextEdit { min-height: 80px; }
        #primaryButton {
            background-color: #3182CE; color: white; border-radius: 8px;
            padding: 10px 20px; font-size: 14px; font-weight: bold;
        }

        /* --- Leave Records Panel --- */
        #scrollArea { border: none; }
        #leaveRecordItem {
            background-color: #F7FAFC;
            border-radius: 8px;
            padding: 12px;
        }
        #leaveTypeLabel { font-size: 16px; font-weight: bold; color: #2D3748; }
        #leaveDateLabel, #leaveReasonLabel { font-size: 13px; color: #718096; }

        #statusLabel {
            font-size: 12px; font-weight: bold;
            padding: 4px 10px; border-radius: 10px;
        }
        #leaveRecordItem[status="approved"] #statusLabel {
            background-color: #C6F6D5; color: #2F855A;
        }
        #leaveRecordItem[status="pending"] #statusLabel {
            background-color: #FEEBC8; color: #975A16;
        }

        #cancelLeaveButton {
            font-size: 13px;
            color: #3182CE;
            background-color: transparent;
            border: none;
            padding: 5px;
        }
    )");
}