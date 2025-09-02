#include "leave_record_item_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVariant>
#include <QStyle>

static inline LeaveRecordItemWidget::Status mapStatus(const QString &s) {
    const QString t = s.trimmed().toLower();
    if (t == "approved") return LeaveRecordItemWidget::Approved;
    if (t == "rejected") return LeaveRecordItemWidget::Rejected;
    return LeaveRecordItemWidget::Pending;
}

QString LeaveRecordItemWidget::makeDateRange(const QJsonObject &o) {
    const QString s = o.value("start_date").toString();
    const QString e = o.value("end_date").toString();
    return (s.isEmpty() && e.isEmpty()) ? QString() : QString("%1 至 %2").arg(s, e);
}

LeaveRecordItemWidget::Status LeaveRecordItemWidget::toStatus(const QString &s) {
    return mapStatus(s);
}

LeaveRecordItemWidget::LeaveRecordItemWidget(const QJsonObject &record, QWidget *parent)
    : QWidget(parent)
{
    // 先落地数据
    m_leaveId   = record.value("leave_id").toInt();
    m_leaveType = record.value("leave_type").toString();
    m_dateRange = makeDateRange(record);
    m_reason    = record.value("reason").toString();
    m_status    = toStatus(record.value("status").toString());

    setupUi();
    refreshTexts();
    applyStatus(m_status);
}

LeaveRecordItemWidget::LeaveRecordItemWidget(const QString &leaveType,
                                             const QString &dateRange,
                                             const QString &reason,
                                             Status status,
                                             int leaveId,
                                             QWidget *parent)
    : QWidget(parent),
      m_leaveId(leaveId),
      m_leaveType(leaveType),
      m_dateRange(dateRange),
      m_reason(reason),
      m_status(status)
{
    setupUi();
    refreshTexts();
    applyStatus(m_status);
}

void LeaveRecordItemWidget::setupUi()
{
    setObjectName("leaveRecordItem");

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 10, 12, 10);
    mainLayout->setSpacing(10);

    // 左侧信息
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(4);

    lblType_   = new QLabel(this);   lblType_->setObjectName("leaveTypeLabel");
    lblDate_   = new QLabel(this);   lblDate_->setObjectName("leaveDateLabel");
    lblReason_ = new QLabel(this);   lblReason_->setObjectName("leaveReasonLabel");
    lblReason_->setWordWrap(true);

    infoLayout->addWidget(lblType_);
    infoLayout->addWidget(lblDate_);
    infoLayout->addWidget(lblReason_);
    infoLayout->addStretch();

    // 右侧状态与按钮
    auto *statusLayout = new QVBoxLayout();
    statusLayout->setSpacing(8);
    statusLayout->setAlignment(Qt::AlignTop);

    statusLabel  = new QLabel(this);
    statusLabel->setObjectName("statusLabel");

    cancelButton = new QPushButton("申请销假", this);
    cancelButton->setObjectName("cancelLeaveButton");

    statusLayout->addWidget(statusLabel, 0, Qt::AlignRight);
    statusLayout->addWidget(cancelButton, 0, Qt::AlignRight);

    // 交互
    connect(cancelButton, &QPushButton::clicked, this, [this]{
        emit cancelLeaveRequested(m_leaveId);
    });

    // 组合
    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(statusLayout);
}

void LeaveRecordItemWidget::refreshTexts()
{
    if (lblType_)   lblType_->setText(m_leaveType);
    if (lblDate_)   lblDate_->setText(m_dateRange);
    if (lblReason_) lblReason_->setText(m_reason);
}

void LeaveRecordItemWidget::applyStatus(Status s)
{
    m_status = s;
    switch (s) {
        case Approved:
            statusLabel->setText("已批准");
            setProperty("status", QVariant("approved"));
            cancelButton->setVisible(true);   // 仅批准后可“销假”
            break;
        case Pending:
            statusLabel->setText("审批中");
            setProperty("status", QVariant("pending"));
            cancelButton->setVisible(false);
            break;
        case Rejected:
            statusLabel->setText("已驳回");
            setProperty("status", QVariant("rejected"));
            cancelButton->setVisible(false);
            break;
    }
    // 触发 QSS 重绘
    style()->unpolish(this);
    style()->polish(this);
}

void LeaveRecordItemWidget::bindRecord(const QJsonObject &record)
{
    m_leaveId   = record.value("leave_id").toInt(m_leaveId);
    m_leaveType = record.value("leave_type").toString(m_leaveType);
    m_dateRange = makeDateRange(record);
    m_reason    = record.value("reason").toString(m_reason);
    refreshTexts();
    updateStatusFromServer(record.value("status").toString());
}

void LeaveRecordItemWidget::updateStatusFromServer(const QString &statusStr)
{
    applyStatus(toStatus(statusStr));
}
