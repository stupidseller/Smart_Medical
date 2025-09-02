#ifndef PATIENT_MANAGEMENT_WIDGET_H
#define PATIENT_MANAGEMENT_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QList>
#include <QJsonArray>
#include <QJsonObject>
#include <QHash>            // ✅ 新增
#include "patient_data_types.h"
#include "patient_list_item_widget.h"
#include <QHash>
class PatientListItemWidget; // 前置

class QLineEdit;
class QVBoxLayout;

class PatientManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PatientManagementWidget(QWidget *parent = nullptr);

signals:
    void backRequested();
    void currentPatientChanged(int id);
    // ✅ 新增：让 Main(API) 去拉取某个患者的详细资料
    void requestLoadPatientProfile(int patientId);
    void openProfileRequested(int patientId);
       void openMedicalRecordRequested(int patientId, const QString &patientName);
       void openMedicalOrdersRequested(int patientId, const QString &patientName);
       void refreshItemRequested(int patientId); // 让 Main 触发重拉 profile/record/orders

private slots:
    void onPatientSelected(int id);
    void onBackButtonClicked();
    void onSearchTriggered();
    void onViewMedicalRecordClicked();
    void onViewMedicalOrdersClicked();
    void updateListItemProfile(const QJsonObject &profile);
        void updateListItemRecordFlag(const QJsonObject &record, bool found);
        void updateListItemOrdersFlag(const QJsonObject &order, const QJsonArray &items, bool found);

public slots:
    // —— 运行时数据绑定/操作 ——
    void setPatientList(const QJsonArray &patients);
    void upsertPatient(const QJsonObject &patient);
    void removePatientById(int id);
    void selectPatientById(int id);
    void setSearchText(const QString &text);

    // ✅ 新增：接收 API 的患者档案并更新右侧详情
    void setPatientProfile(const QJsonObject &profile);

private:
    // UI创建
    QHash<int, PatientListItemWidget*> m_itemById;
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

    // 数据 & UI 更新
    void updateDetailPanel(const PatientData &data);
    void clearDetailPanel();
    void updatePatientList();

    // 工具
    static PatientData parsePatientJson(const QJsonObject &o);
    const PatientData* findPatientById(int id) const;
    int findPatientIndexById(int id) const;

private:
    // 数据
    QList<PatientData> m_patients;
    int m_currentSelectedPatientId = -1;

    // ✅ 新增：profile 缓存（id -> profile）
    QHash<int, QJsonObject> m_profileById;

    // 列表面板
    QButtonGroup *patientListGroup = nullptr;
    QVBoxLayout *patientListLayout = nullptr;

    // 搜索
    QLineEdit *searchInput = nullptr;

    // 详情面板控件
    QLabel *detailNameLabel = nullptr;
    QLabel *detailIdLabel = nullptr;
    QLabel *detailGenderLabel = nullptr;
    QLabel *detailAgeLabel = nullptr;
    QLabel *detailBirthLabel = nullptr;
    QLabel *detailIdCardLabel = nullptr;
    QLabel *detailDeptLabel = nullptr;
    QLabel *detailBedLabel = nullptr;
    QLabel *detailAdmitDateLabel = nullptr;
    QLabel *detailStayDaysLabel = nullptr;

    // 状态标签
    QLabel *statusInpatient = nullptr;
    QLabel *statusInsurance = nullptr;
    QLabel *statusKeyFocus = nullptr;

    // 操作按钮
    QPushButton *recordsBtn = nullptr;
    QPushButton *ordersBtn = nullptr;
};

#endif // PATIENT_MANAGEMENT_WIDGET_H
