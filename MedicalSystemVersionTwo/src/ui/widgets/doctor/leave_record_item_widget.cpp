#include "leave_record_item_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVariant>
LeaveRecordItemWidget::LeaveRecordItemWidget(const QString &leaveType, const QString &dateRange,
                                             const QString &reason, Status status, QWidget *parent)
        : QWidget(parent), leaveType(leaveType), dateRange(dateRange), reason(reason), status(status)
{
    this->setObjectName("leaveRecordItem");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // 左侧信息
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);
    QLabel *typeLabel = new QLabel(leaveType);
    typeLabel->setObjectName("leaveTypeLabel");
    QLabel *dateLabel = new QLabel(dateRange);
    dateLabel->setObjectName("leaveDateLabel");
    QLabel *reasonLabel = new QLabel(reason);
    reasonLabel->setObjectName("leaveReasonLabel");
    infoLayout->addWidget(typeLabel);
    infoLayout->addWidget(dateLabel);
    infoLayout->addWidget(reasonLabel);
    infoLayout->addStretch();

    // 右侧状态和按钮
    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->setSpacing(10);
    statusLayout->setAlignment(Qt::AlignTop);

    statusLabel = new QLabel();
    statusLabel->setObjectName("statusLabel");

    cancelButton = new QPushButton("申请销假");
    cancelButton->setObjectName("cancelLeaveButton");

    statusLayout->addWidget(statusLabel, 0, Qt::AlignRight);
    statusLayout->addWidget(cancelButton, 0, Qt::AlignRight);

    // 设置状态和按钮可见性
    switch (status) {
        case Approved:
            statusLabel->setText("已批准");
            this->setProperty("status", QVariant("approved"));
            cancelButton->setVisible(true);
            break;
        case Pending:
            statusLabel->setText("审批中");
            this->setProperty("status", QVariant("pending"));
            cancelButton->setVisible(false);
            break;
        case Rejected:
            statusLabel->setText("已驳回");
            this->setProperty("status", QVariant("rejected"));
            cancelButton->setVisible(false);
            break;
    }

    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(statusLayout);
}