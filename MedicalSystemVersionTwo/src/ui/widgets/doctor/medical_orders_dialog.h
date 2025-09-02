#ifndef MEDICAL_ORDERS_DIALOG_H
#define MEDICAL_ORDERS_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QTableWidget>
#include <QTextEdit>
#include "patient_data_types.h"

class MedicalOrdersDialog : public QDialog
{
Q_OBJECT

public:
    explicit MedicalOrdersDialog(const PatientData &patientData, QWidget *parent = nullptr);

private:
    void initPointers();
    void initUI();
    void applyStyles();
    QWidget* createHeader();
    QWidget* createPatientInfo();
    QWidget* createOrdersTable();
    QWidget* createNotesSection();

    void loadPatientData(const PatientData &data);
    void setupTableData();  // 同步设置表格数据

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
    QLabel *m_diagnosisLabel;

    QTableWidget *m_ordersTable;
    QTextEdit *m_notesEdit;
};

#endif // MEDICAL_ORDERS_DIALOG_H