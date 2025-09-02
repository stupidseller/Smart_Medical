#ifndef MEDICAL_RECORD_DIALOG_H
#define MEDICAL_RECORD_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QTextEdit>
#include <QJsonObject>
#include <QDate>
#include <QTime>
#include "patient_data_types.h"

class MedicalRecordDialog : public QDialog
{
    Q_OBJECT
public:
    // ✅ 现有构造：从 PatientData 进来
    explicit MedicalRecordDialog(const PatientData &patientData, QWidget *parent = nullptr);
    // ✅ 兼容旧用法：从 patientId + name 进来（让 Main 无需改动）
    explicit MedicalRecordDialog(int patientId, const QString &patientName, QWidget *parent = nullptr);

public slots:
    // —— 通用：一次性从 JSON 绑定或增量更新（有则覆盖，无则忽略）——
    void bindRuntimeData(const QJsonObject &payload);

    // —— 细粒度更新 ——
    void setChiefComplaint(const QString &t);
    void setPresentIllness(const QString &t);
    void setPastHistory(const QString &t);
    void setDiagnosis(const QString &diag, const QString &icd);
    void setRecordTimestamp(const QDate &date, const QTime &time);

    // ✅ 接收 Widget 回执
    void onRecordLoaded(const QJsonObject &record, bool found);      // 对应 Widget::medicalRecordLoaded
    void onRecordSaved(bool ok, int recordId, const QString &msg);   // 对应 Widget::medicalRecordSaved

signals:
    // ✅ 让 Main 去调 Widget
    void requestLoadRecord(int patientId);
    void requestSaveRecord(const QJsonObject &record);

    // （保留原注释中的刷新意图；不强制使用）
    void refreshRequested(const QString &patientIdStr);

private:
    // UI
    void initUI();
    void applyStyles();
    QWidget* createHeader();
    QWidget* createPatientInfo();
    QWidget* createMainContent();
    QWidget* createDiagnosisSection();
    QWidget* createToolbar();                 // ✅ 新增：编辑/保存/刷新栏

    void loadPatientData(const PatientData &data); // 仅填充基础身份信息

    // 工具
    static QString pickDate(const QJsonObject &o, const char *key, const char *fmt = "yyyy-MM-dd");
    static QString pickTime(const QJsonObject &o, const char *key, const char *fmt = "HH:mm");

    // ✅ 编辑/校验/打包
    void setEditMode(bool on);
    void setDirty(bool on);
    bool validateAndBuildPayload(QJsonObject &out) const; // 构建提交 JSON（含 patient_id/record_id 等）

protected:
    void showEvent(QShowEvent *e) override; // 首次显示时自动请求加载

private:
    // 患者数据
    PatientData m_patientData;
    int         m_pidInt = -1;        // 用于 API 的 patient_id（整型）
    int         m_recordId = 0;       // 服务器返回的 record_id（更新时带回）
    bool        m_firstShowLoaded = false;
    bool        m_editMode = false;
    bool        m_dirty = false;

    // 头部信息
    QLabel *m_nameLabel{}, *m_genderLabel{}, *m_ageLabel{}, *m_patientIdLabel{},
           *m_deptLabel{}, *m_bedLabel{}, *m_admitDateLabel{}, *m_recordDateLabel{},
           *m_recordTimeLabel{};

    // 内容
    QTextEdit *m_chiefComplaintEdit{}, *m_presentIllnessEdit{}, *m_pastHistoryEdit{};
    QLabel *m_diagnosisLabel{}, *m_icdCodeLabel{};

    // 工具栏按钮
    QPushButton *m_editBtn{}, *m_saveBtn{}, *m_refreshBtn{}, *m_closeBtn{};
};

#endif // MEDICAL_RECORD_DIALOG_H
