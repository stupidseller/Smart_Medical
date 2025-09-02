#ifndef MEDICAL_RECORD_WIDGET_H
#define MEDICAL_RECORD_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QTextEdit>

// 病历数据结构
struct MedicalRecordData {
    // 基本信息
    QString patientName;         // 姓名
    QString gender;              // 性别
    QString age;                 // 年龄
    QString recordNumber;        // 病历号
    QString department;          // 科室
    QString bedNumber;           // 床号
    QString admissionDate;       // 入院日期
    QString recordDate;          // 记录日期
    QString recordTime;          // 记录时间

    // 病历内容
    QString chiefComplaint;      // 主诉
    QString presentIllness;      // 现病史
    QString pastHistory;         // 既往史
    QString diagnosis;           // 诊断
    QString icdCode;             // ICD编码
    QString treatmentPlan;       // 治疗方案
    QString doctorName;          // 医生姓名
};

// 诊断条目结构
struct DiagnosisItem {
    QString type;               // 诊断类型（主要诊断/次要诊断）
    QString content;            // 诊断内容
    QString icdCode;            // ICD编码
};

class MedicalRecordWidget : public QWidget
{
Q_OBJECT

public:
    explicit MedicalRecordWidget(QWidget *parent = nullptr);
    void setMedicalRecordData(const MedicalRecordData &data);

signals:
    void backRequested();

private slots:
    void onBackButtonClicked();
    void onPrintRecordClicked();
    void onExportRecordClicked();

private:
    void initUI();
    void setupData();
    void applyStyles();

    // UI创建方法
    QWidget* createHeader();
    QWidget* createRecordHeader();
    QWidget* createPatientInfoTable();
    QWidget* createRecordContent();
    QWidget* createDiagnosisSection();
    QWidget* createActionButtons();

    // 工具方法
    QFrame* createSectionFrame(const QString &title, const QString &content);
    QFrame* createTableCell(const QString &label, const QString &value, bool isHeader = false);

    // 数据成员
    MedicalRecordData recordData;
    QList<DiagnosisItem> diagnosisList;

    // UI控件
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QPushButton *printBtn;
    QPushButton *exportBtn;
};

#endif // MEDICAL_RECORD_WIDGET_H