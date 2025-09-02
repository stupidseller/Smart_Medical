#ifndef ATTENDANCE_WIDGET_H
#define ATTENDANCE_WIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

class QLabel;
class QTimer;
class QVBoxLayout;
class QComboBox;
class QDateEdit;
class QTextEdit;
class QPushButton;

class AttendanceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AttendanceWidget(QWidget *parent = nullptr);

signals:
    void backRequested();

    // —— UI -> Main（转给 Widget 执行）——
    void requestLoadAttendanceToday();
    void requestClockEvent(const QString &kind); // "in" / "out"
    void requestSubmitLeave(const QString &leaveType,
                            const QString &startDate, // "yyyy-MM-dd"
                            const QString &endDate,   // "yyyy-MM-dd"
                            const QString &reason);
    void requestCancelLeave(int leaveId); // 透传 LeaveRecordItem 的信号

public slots:
    // —— Widget -> UI ——
    void onLeaveRecordsLoaded(const QJsonArray &records);
    void onAttendanceTodayLoaded(const QJsonObject &obj);
    void onClockEventDone(const QJsonObject &obj);

private slots:
    void updateClock();
    void onClickClockIn();
    void onClickClockOut();
    void onSubmitLeave();

private:
    void initUI();
    void applyStyles();
    void clearRecordList();

    QWidget* createHeader();
    QWidget* createStatusBanner();
    QWidget* createClockInPanel();
    QWidget* createLeaveApplicationPanel();
    QWidget* createLeaveRecordsPanel();

    // ===== UI 指针 =====
    QTimer *clockTimer = nullptr;
    QLabel *clockTimeLabel = nullptr;
    QLabel *clockDateLabel = nullptr;

    QLabel *todayStatusLabel = nullptr;      // 状态横幅中的“已打卡/未打卡”
    QVBoxLayout *m_recordsLayout = nullptr;  // 请假记录列表布局

    // 打卡区
    QPushButton *btnClockIn_ = nullptr;
    QPushButton *btnClockOut_ = nullptr;
    QVBoxLayout *clockHistoryLayout_ = nullptr; // 今日打卡历史

    // 请假表单
    QComboBox  *leaveTypeCombo_ = nullptr;
    QDateEdit  *startDateEdit_  = nullptr;
    QDateEdit  *endDateEdit_    = nullptr;
    QTextEdit  *reasonEdit_     = nullptr;

    // 本地状态
    bool canClockIn_  = true;
    bool canClockOut_ = false;
};

#endif // ATTENDANCE_WIDGET_H
