#ifndef PATIENT_MANAGEMENT_WIDGET_H
#define PATIENT_MANAGEMENT_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QList>
#include "patient_data_types.h"
#include "patient_list_item_widget.h" // 确保包含自定义控件的头文件

// 前置声明
class QLineEdit;
class QVBoxLayout;

class PatientManagementWidget : public QWidget
{
Q_OBJECT

public:
    explicit PatientManagementWidget(QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void onPatientSelected(int id);
    void onBackButtonClicked();
    // 新增：触发搜索和列表更新的槽函数
    void onSearchTriggered();

private:
    // UI创建方法
    void initUI();
    void applyStyles();

    QWidget* createHeader();
    QWidget* createSearchPanel();
    QWidget* createContentArea();
    QWidget* createPatientListPanel();
    QWidget* createPatientDetailPanel();

    QWidget* createPatientSummaryCard();
    QWidget* createDetailInfoGrid();
    QWidget* createActionButtons();

    // 数据和UI更新方法
    void updateDetailPanel(const PatientData &data);
    void clearDetailPanel();
    // 新增：更新患者列表的核心函数
    void updatePatientList();


    // 数据成员
    QList<PatientData> samplePatients;
    QButtonGroup *patientListGroup;
    QVBoxLayout *patientListLayout; // 持有列表布局的指针以便修改

    // 搜索框指针
    QLineEdit *searchInput;

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