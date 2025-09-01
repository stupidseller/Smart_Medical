#include "appointment_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
AppointmentDialog::AppointmentDialog(const QString &doctorName, const QString &doctorTitle, const QString &department, const QString &time, const QString &fee, QWidget *parent)
        : QDialog(parent),
          m_doctorName(doctorName),
          m_doctorTitle(doctorTitle),
          m_department(department),
          m_time(time),
          m_fee(fee),
          descriptionEdit(nullptr)
{
    setObjectName("AppointmentDialog");
    setModal(true); // 设置为模态对话框
    setWindowTitle("预约挂号");
    initUI();
    initStyleSheets();
}

AppointmentDialog::~AppointmentDialog() {}

void AppointmentDialog::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    // 顶部标题
    QLabel *titleLabel = new QLabel("预约挂号");
    titleLabel->setObjectName("dialogTitle");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    // 信息表单
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->addRow("医生:", new QLabel(m_doctorName));
    formLayout->addRow("职称:", new QLabel(m_doctorTitle));
    formLayout->addRow("科室:", new QLabel(m_department));
    formLayout->addRow("时间:", new QLabel(m_time));
    formLayout->addRow("费用:", new QLabel(m_fee));
    mainLayout->addLayout(formLayout);

    // 病情描述
    descriptionEdit = new QTextEdit();
    descriptionEdit->setPlaceholderText("请简要描述您的症状或需要咨询的问题");
    formLayout->addRow("病情描述 (可选):", descriptionEdit);

    // 底部按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, &QPushButton::clicked, this, &AppointmentDialog::reject);

    QPushButton *confirmButton = new QPushButton("确认挂号");
    confirmButton->setObjectName("saveButton"); // 复用样式
    connect(confirmButton, &QPushButton::clicked, this, &AppointmentDialog::onConfirmBooking);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

QString AppointmentDialog::getDiseaseDescription() const {
    return descriptionEdit ? descriptionEdit->toPlainText() : QString();
}

void AppointmentDialog::onConfirmBooking() {
    // 调用伪代码后端函数
    if (submitAppointmentRequest()) {
        accept(); // 如果提交成功，则关闭对话框并返回 QDialog::Accepted
    }
}
//
void AppointmentDialog::doSubmitAppointmentRequest(int patientId, int doctorId, int slotId, const QString &desc)
{
    // 直接把数据交给 Api
    emit submitAppointmentRequest(patientId, doctorId, slotId, desc);
}
void AppointmentDialog::onSubmitAppointmentRequestOk(const QJsonObject &resp)
{
    const bool ok = resp.value("success").toBool();
    if (!ok) {
        const QString err = resp.value("error").toString();
        qWarning() << "[AppointmentDialog] submit failed:" << err;
        // 你可以用 QMessageBox 提示用户
        // QMessageBox::warning(this, tr("预约失败"), err);
        return;
    }

    const qlonglong apptId = resp.value("appointment_id").toVariant().toLongLong();
    const int pid  = resp.value("patient_id").toInt();
    const int did  = resp.value("doctor_id").toInt();
    const int sid  = resp.value("slot_id").toInt();

    qDebug() << "[AppointmentDialog] submit ok. appt_id=" << apptId
             << "patient=" << pid << "doctor=" << did << "slot=" << sid;

    // TODO: 刷新 UI、关闭对话框等
    // QMessageBox::information(this, tr("预约成功"), tr("预约已提交，编号：%1").arg(apptId));
}
// shangmian gaide
void AppointmentDialog::initStyleSheets() {
    this->setStyleSheet(R"(
        #AppointmentDialog { background-color: white; border-radius: 12px; }
        #dialogTitle { font-size: 20px; font-weight: bold; color: #1A202C; }
        QLabel { font-size: 14px; color: #4A5568; }
        QTextEdit {
            border: 1px solid #CBD5E0;
            border-radius: 6px;
            padding: 8px;
            min-height: 80px;
        }
        #cancelButton, #saveButton {
            padding: 10px 24px;
            font-size: 15px;
            border-radius: 8px;
        }
        #cancelButton { background-color: #F7FAFC; color: #4A5568; border: 1px solid #E2E8F0; }
        #cancelButton:hover { background-color: #EDF2F7; }
        #saveButton { background-color: #3182CE; color: white; border: none; font-weight: bold; }
        #saveButton:hover { background-color: #2B6CB0; }
    )");
}
