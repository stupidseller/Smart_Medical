#ifndef LEAVE_RECORD_ITEM_WIDGET_H
#define LEAVE_RECORD_ITEM_WIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;

class LeaveRecordItemWidget : public QWidget
{
Q_OBJECT

public:
    enum Status {
        Pending,
        Approved,
        Rejected
    };

    explicit LeaveRecordItemWidget(const QString &leaveType, const QString &dateRange,
                                   const QString &reason, Status status, QWidget *parent = nullptr);

private:
    void initUI();

    QString leaveType;
    QString dateRange;
    QString reason;
    Status status;

    QLabel *statusLabel;
    QPushButton *cancelButton;
};

#endif // LEAVE_RECORD_ITEM_WIDGET_H