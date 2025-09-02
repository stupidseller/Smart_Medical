#ifndef LEAVE_RECORD_ITEM_WIDGET_H
#define LEAVE_RECORD_ITEM_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QJsonObject>

class LeaveRecordItemWidget : public QWidget
{
    Q_OBJECT
public:
    enum Status { Pending, Approved, Rejected };

    // 直接用结构化数据构造（推荐）
    explicit LeaveRecordItemWidget(const QJsonObject &record, QWidget *parent = nullptr);

    // 兼容旧用法
    explicit LeaveRecordItemWidget(const QString &leaveType,
                                   const QString &dateRange,
                                   const QString &reason,
                                   Status status,
                                   int leaveId = 0,
                                   QWidget *parent = nullptr);

    int  leaveId() const { return m_leaveId; }
    void bindRecord(const QJsonObject &record);  // 复用卡片刷新数据

signals:
    void cancelLeaveRequested(int leaveId);  // 点“申请销假”→ 交给上层
    void openLeaveRequested(int leaveId);    // 预留：点击整卡打开详情

public slots:
    void updateStatusFromServer(const QString &statusStr); // Widget 推送状态后刷新外观

private:
    static Status toStatus(const QString &s);
    static QString makeDateRange(const QJsonObject &o);

    void setupUi();
    void applyStatus(Status s);
    void refreshTexts();

private:
    // 数据
    int     m_leaveId   = 0;
    QString m_leaveType;
    QString m_dateRange;
    QString m_reason;
    Status  m_status    = Pending;

    // UI
    QLabel      *lblType_    = nullptr;
    QLabel      *lblDate_    = nullptr;
    QLabel      *lblReason_  = nullptr;
    QLabel      *statusLabel = nullptr;
    QPushButton *cancelButton= nullptr;
};

#endif // LEAVE_RECORD_ITEM_WIDGET_H
