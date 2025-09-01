#ifndef PATIENT_MANAGEMENT_WIDGET_H
#define PATIENT_MANAGEMENT_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QList>
#include "patient_data_types.h"  // 包含数据类型定义

class PatientManagementWidget : public QWidget
{
Q_OBJECT

public:
    explicit PatientManagementWidget(QWidget *parent = nullptr);

signals:
    void backRequested();  // 添加返回信号

private slots:
    void onPatientSelected(int id);
    void onBackButtonClicked();  // 添加返回按钮槽函数

private:
    // UI创建方法
    void initUI();
    void applyStyles();

    QWidget* createHeader();
    QWidget* createSearchPanel();
    QWidget* createContentArea();
    QWidget* createPatientListPanel();
    QWidget* createPatientDetailPanel();

    QPushButton* createPatientListItem(const PatientData &patient);
    QWidget* createPatientSummaryCard();
    QWidget* createDetailInfoGrid();
    QWidget* createActionButtons();

    void updateDetailPanel(const PatientData &data);

    // 数据成员
    QList<PatientData> samplePatients;
    QButtonGroup *patientListGroup;

    // 详情面板控件
    QLabel *detailNameLabel;
    QLabel *detailIdLabel;
    QLabel *detailGenderLabel;
    QLabel *detailAgeLabel;
    QLabel *detailBirthLabel;
    QLabel *detailIdCardLabel;
    QLabel *detailDeptLabel;
    QLabel *detailBedLabel;
    QLabel *detailAdmitDateLabel;
    QLabel *detailStayDaysLabel;

    // 状态标签
    QLabel *statusInpatient;
    QLabel *statusInsurance;
    QLabel *statusKeyFocus;
};

#endif // PATIENT_MANAGEMENT_WIDGET_H