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
#include "patient_data_types.h"

class MedicalRecordDialog : public QDialog
{
Q_OBJECT

public:
    explicit MedicalRecordDialog(const PatientData &patientData, QWidget *parent = nullptr);

private:
    void initUI();
    void applyStyles();
    QWidget* createHeader();
    QWidget* createPatientInfo();
    QWidget* createMainContent();
    QWidget* createDiagnosisSection();

    void loadPatientData(const PatientData &data);

private:
    // 患者数据
    PatientData m_patientData;

    // UI组件
    QLabel *m_nameLabel;
    QLabel *m_genderLabel;
    QLabel *m_ageLabel;
    QLabel *m_patientIdLabel;
    QLabel *m_deptLabel;
    QLabel *m_bedLabel;
    QLabel *m_admitDateLabel;
    QLabel *m_recordDateLabel;
    QLabel *m_recordTimeLabel;

    QTextEdit *m_chiefComplaintEdit;
    QTextEdit *m_presentIllnessEdit;
    QTextEdit *m_pastHistoryEdit;
    QLabel *m_diagnosisLabel;
    QLabel *m_icdCodeLabel;
};

#endif // MEDICAL_RECORD_DIALOG_H