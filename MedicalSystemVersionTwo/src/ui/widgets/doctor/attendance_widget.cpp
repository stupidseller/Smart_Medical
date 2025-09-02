#include "attendance_widget.h"
#include "leave_record_item_widget.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QJsonArray>
#include <QJsonObject>
#include <QScrollBar>

// SVG
static const char* calendarIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2D3748" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line></svg>)";

static inline QString pickS(const QJsonObject &o, std::initializer_list<const char*> keys, const QString &def={}) {
    for (auto k : keys) if (o.contains(k)) return o.value(k).toString();
    return def;
}
static inline bool pickB(const QJsonObject &o, std::initializer_list<const char*> keys, bool def=false) {
    for (auto k : keys) if (o.contains(k)) return o.value(k).toBool(def);
    return def;
}
static inline QJsonArray pickA(const QJsonObject &o, std::initializer_list<const char*> keys) {
    for (auto k : keys) if (o.contains(k)) return o.value(k).toArray();
    return {};
}

AttendanceWidget::AttendanceWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    applyStyles();

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &AttendanceWidget::updateClock);
    clockTimer->start(1000);
    updateClock();

    // 进入页面主动要一次今日考勤 & 请假记录（Main 也会再拉一次也没问题）
    emit requestLoadAttendanceToday();
}

void AttendanceWidget::initUI() {
    setObjectName("attendanceWidget");
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 30);
    mainLayout->setSpacing(20);

    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createStatusBanner());

    auto *panelsLayout = new QHBoxLayout();
    panelsLayout->setSpacing(20);
    panelsLayout->addWidget(createClockInPanel(), 1);
    panelsLayout->addWidget(createLeaveApplicationPanel(), 1);
    panelsLayout->addWidget(createLeaveRecordsPanel(), 1);

    mainLayout->addLayout(panelsLayout);
}

QWidget* AttendanceWidget::createHeader() {
    auto *header = new QWidget();
    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0,0,0,0);

    auto *backButton = new QPushButton(" 返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &AttendanceWidget::backRequested);

    auto *title = new QLabel("考勤管理");
    title->setObjectName("pageTitle");

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    return header;
}

QWidget* AttendanceWidget::createStatusBanner() {
    auto *banner = new QFrame();
    banner->setObjectName("statusBanner");
    auto *layout = new QHBoxLayout(banner);
    layout->setSpacing(15);

    auto *icon = new QSvgWidget();
    icon->load(QByteArray(calendarIconSvg));
    icon->setFixedSize(32, 32);

    auto *title = new QLabel("今日考勤状态");
    title->setObjectName("bannerTitle");

    todayStatusLabel = new QLabel("未打卡");
    todayStatusLabel->setObjectName("bannerStatus");

    layout->addWidget(icon);
    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(todayStatusLabel);
    return banner;
}

QWidget* AttendanceWidget::createClockInPanel() {
    auto *panel = new QFrame();
    panel->setObjectName("contentPanel");
    auto *layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    clockTimeLabel = new QLabel("--:--:--");
    clockTimeLabel->setObjectName("clockTimeLabel");
    clockTimeLabel->setAlignment(Qt::AlignCenter);

    clockDateLabel = new QLabel("----年--月--日 ----");
    clockDateLabel->setObjectName("clockDateLabel");
    clockDateLabel->setAlignment(Qt::AlignCenter);

    btnClockIn_  = new QPushButton(" 上班打卡");
    btnClockOut_ = new QPushButton(" 下班打卡");
    btnClockIn_->setObjectName("clockInButton");
    btnClockOut_->setObjectName("clockOutButton");
    connect(btnClockIn_,  &QPushButton::clicked, this, &AttendanceWidget::onClickClockIn);
    connect(btnClockOut_, &QPushButton::clicked, this, &AttendanceWidget::onClickClockOut);

    auto *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnClockIn_);
    btnLayout->addWidget(btnClockOut_);

    // 今日打卡记录列表
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setObjectName("scrollArea");

    auto *content = new QWidget();
    clockHistoryLayout_ = new QVBoxLayout(content);
    clockHistoryLayout_->setSpacing(6);
    clockHistoryLayout_->addStretch();

    scroll->setWidget(content);

    layout->addWidget(clockTimeLabel);
    layout->addWidget(clockDateLabel);
    layout->addLayout(btnLayout);
    layout->addWidget(scroll, 1);
    return panel;
}

QWidget* AttendanceWidget::createLeaveApplicationPanel() {
    auto *panel = new QFrame();
    panel->setObjectName("contentPanel");
    auto *layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    auto *form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    leaveTypeCombo_ = new QComboBox();
    leaveTypeCombo_->addItems({"请选择请假类型", "病假", "事假", "年假"});

    startDateEdit_ = new QDateEdit();
    startDateEdit_->setCalendarPopup(true);
    startDateEdit_->setDisplayFormat("yyyy-MM-dd");
    startDateEdit_->setDate(QDate::currentDate());

    endDateEdit_ = new QDateEdit();
    endDateEdit_->setCalendarPopup(true);
    endDateEdit_->setDisplayFormat("yyyy-MM-dd");
    endDateEdit_->setDate(QDate::currentDate());

    reasonEdit_ = new QTextEdit();
    reasonEdit_->setPlaceholderText("请输入请假事由...");

    form->addRow("请假类型",  leaveTypeCombo_);
    form->addRow("开始时间",  startDateEdit_);
    form->addRow("结束时间",  endDateEdit_);
    form->addRow("请假事由",  reasonEdit_);

    auto *submitButton = new QPushButton("提交申请");
    submitButton->setObjectName("primaryButton");
    connect(submitButton, &QPushButton::clicked, this, &AttendanceWidget::onSubmitLeave);

    layout->addLayout(form);
    layout->addStretch();
    layout->addWidget(submitButton);
    return panel;
}

QWidget* AttendanceWidget::createLeaveRecordsPanel() {
    auto *panel = new QFrame();
    panel->setObjectName("contentPanel");
    auto *layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("scrollArea");

    auto *scrollContent = new QWidget();
    m_recordsLayout = new QVBoxLayout(scrollContent);
    m_recordsLayout->setSpacing(10);
    m_recordsLayout->setContentsMargins(5, 5, 5, 5);
    m_recordsLayout->addStretch(); // 占位

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);
    return panel;
}

/* ============ 槽：Widget -> UI ============ */

void AttendanceWidget::onLeaveRecordsLoaded(const QJsonArray &records)
{
    if (!m_recordsLayout) return;
    clearRecordList();

    for (const QJsonValue &v : records) {
        const QJsonObject rec = v.toObject();
        auto *item = new LeaveRecordItemWidget(rec, this);
        // 子项 signal 直连到我这儿的 signal（转给 Main）
        connect(item, &LeaveRecordItemWidget::cancelLeaveRequested,
                this,  &AttendanceWidget::requestCancelLeave);
        m_recordsLayout->insertWidget(m_recordsLayout->count() - 1, item);
    }
}

void AttendanceWidget::onAttendanceTodayLoaded(const QJsonObject &obj)
{
    // 解析：尽量兼容不同字段名
    const bool success = pickB(obj, {"success"}, true);
    if (!success) {
        todayStatusLabel->setText(pickS(obj, {"error","message"}, "获取考勤失败"));
        return;
    }

    const QJsonArray records = pickA(obj, {"records","today","punches"});
    // 计算状态
    int inCount = 0, outCount = 0;
    for (const auto &v : records) {
        const auto o = v.toObject();
        const QString kind = pickS(o, {"kind","type"});
        if (kind.compare("in", Qt::CaseInsensitive)==0)  ++inCount;
        if (kind.compare("out", Qt::CaseInsensitive)==0) ++outCount;
    }

    canClockIn_  = pickB(obj, {"can_clock_in"},  inCount<=outCount);
    canClockOut_ = pickB(obj, {"can_clock_out"}, inCount>outCount);

    if (todayStatusLabel) {
        if (inCount==0 && outCount==0) todayStatusLabel->setText("未打卡");
        else todayStatusLabel->setText(QString("已打卡 %1 次").arg(inCount+outCount));
    }
    if (btnClockIn_)  btnClockIn_->setEnabled(canClockIn_);
    if (btnClockOut_) btnClockOut_->setEnabled(canClockOut_);

    // 刷新“今日打卡记录”
    if (clockHistoryLayout_) {
        // 清空（保留最后一个 stretch）
        while (clockHistoryLayout_->count() > 1) {
            auto *it = clockHistoryLayout_->takeAt(0);
            if (it->widget()) it->widget()->deleteLater();
            delete it;
        }
        for (const auto &v : records) {
            const auto o = v.toObject();
            const QString t = pickS(o, {"time","timestamp","at"});
            const QString k = pickS(o, {"kind","type"});
            auto *row = new QLabel(QString("%1  %2").arg(k=="in"?"上班":"下班", t));
            clockHistoryLayout_->insertWidget(clockHistoryLayout_->count()-1, row);
        }
    }
}

void AttendanceWidget::onClockEventDone(const QJsonObject &obj)
{
    const bool ok = pickB(obj, {"success"}, false);
    if (todayStatusLabel) {
        todayStatusLabel->setText(ok ? "打卡成功" : pickS(obj, {"error","message"}, "打卡失败"));
    }
    // 成功后立即刷新今日考勤
    if (ok) emit requestLoadAttendanceToday();
}

/* ============ 槽：UI内部 ============ */

void AttendanceWidget::updateClock() {
    const QDateTime now = QDateTime::currentDateTime();
    if(clockTimeLabel) clockTimeLabel->setText(now.toString("HH:mm:ss"));
    if(clockDateLabel) clockDateLabel->setText(now.toString("yyyy年MM月dd日 dddd"));
}

void AttendanceWidget::onClickClockIn()  { emit requestClockEvent("in");  }
void AttendanceWidget::onClickClockOut() { emit requestClockEvent("out"); }

void AttendanceWidget::onSubmitLeave() {
    const int idx = leaveTypeCombo_ ? leaveTypeCombo_->currentIndex() : 0;
    const QString type = (idx<=0) ? "" : leaveTypeCombo_->currentText();
    const QString start = startDateEdit_ ? startDateEdit_->date().toString("yyyy-MM-dd") : "";
    const QString end   = endDateEdit_   ? endDateEdit_->date().toString("yyyy-MM-dd")   : "";
    const QString reason= reasonEdit_    ? reasonEdit_->toPlainText().trimmed()          : "";

    if (type.isEmpty() || start.isEmpty() || end.isEmpty()) {
        if (todayStatusLabel) todayStatusLabel->setText("请完整填写请假信息");
        return;
    }
    emit requestSubmitLeave(type, start, end, reason);
}

/* ============ 辅助 ============ */

void AttendanceWidget::clearRecordList()
{
    if (!m_recordsLayout) return;
    while (m_recordsLayout->count() > 1) {
        QLayoutItem *it = m_recordsLayout->takeAt(0);
        if (auto *w = it->widget()) w->deleteLater();
        delete it;
    }
}

void AttendanceWidget::applyStyles() {
    setStyleSheet(R"(
        #attendanceWidget { background-color:#F0F9FF; }
        #pageTitle { font-size:20px; font-weight:bold; color:#2D3748; }
        #backButton { border:none; font-size:14px; font-weight:bold; color:#4A5568; }

        #statusBanner {
            background-color:#EBF8FF; border-radius:10px; padding:15px;
            border-left:5px solid #68D391;
        }
        #bannerTitle { font-size:16px; color:#4A5568; }
        #bannerStatus { font-size:20px; font-weight:bold; color:#2D3748; }

        #contentPanel {
            background:#fff; border-radius:12px; padding:20px; border:1px solid #E2E8F0;
        }
        #clockTimeLabel { font-size:48px; font-weight:bold; color:#2D3748; }
        #clockDateLabel { font-size:16px; color:#718096; }
        #clockInButton, #clockOutButton {
            padding:12px; font-size:16px; font-weight:bold; border-radius:8px; color:#fff;
        }
        #clockInButton { background-color:#48BB78; }
        #clockOutButton { background-color:#F56565; }

        QComboBox, QDateEdit, QTextEdit {
            border:1px solid #E2E8F0; border-radius:6px; padding:8px; font-size:14px;
        }
        QTextEdit { min-height:80px; }
        #primaryButton {
            background:#3182CE; color:#fff; border-radius:8px; padding:10px 20px;
            font-size:14px; font-weight:bold;
        }
        #scrollArea { border:none; }
    )");
}
