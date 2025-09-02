#ifndef HOSPITALIZATION_INFO_WIDGET_H
#define HOSPITALIZATION_INFO_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>

// 住院信息数据结构
struct HospitalizationData {
    QString patientNumber;        // 住院号
    QString admissionDate;        // 入院日期
    QString attendingDoctor;      // 主治医生
    QString wardAndBed;          // 病房床位
    QString department;          // 科室
    QString diagnosis;           // 入院诊断

    // 费用信息
    double bedFee;               // 床位费
    double treatmentFee;         // 诊疗费
    double examFee;              // 检查费
    double medicineFee;          // 药品费
    double otherFee;             // 其他费用
    double totalFee;             // 当前合计
};

// 医嘱信息结构
struct MedicalOrder {
    QString date;                // 日期
    QString type;                // 医嘱类型（长期医嘱/临时医嘱）
    QString content;             // 医嘱内容
    QString doctor;              // 开嘱医生
    QString status;              // 执行状态
};

class HospitalizationInfoWidget : public QWidget
{
Q_OBJECT

public:
    explicit HospitalizationInfoWidget(QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void onBackButtonClicked();
    void onPrintBillClicked();
    void onViewAllOrdersClicked();

private:
    void initUI();
    void setupData();
    void applyStyles();

    // UI创建方法
    QWidget* createHeader();
    QWidget* createCurrentStatusCard();
    QWidget* createInfoAndBillSection();
    QWidget* createBasicInfoCard();
    QWidget* createBillInfoCard();
    QWidget* createMedicalOrdersCard();

    // 数据成员
    HospitalizationData hospData;
    QList<MedicalOrder> medicalOrders;

    // UI控件
    QLabel *statusNumberLabel;
    QLabel *statusDateLabel;
    QLabel *statusDoctorLabel;
    QLabel *statusWardLabel;
    QLabel *inpatientStatusLabel;

    // 基本信息标签
    QLabel *basicNumberLabel;
    QLabel *basicDateLabel;
    QLabel *basicDoctorLabel;
    QLabel *basicDeptLabel;
    QLabel *basicWardLabel;
    QLabel *basicDiagnosisLabel;

    // 费用信息标签
    QLabel *bedFeeLabel;
    QLabel *treatmentFeeLabel;
    QLabel *examFeeLabel;
    QLabel *medicineFeeLabel;
    QLabel *otherFeeLabel;
    QLabel *totalFeeLabel;

    // 医嘱列表容器
    QVBoxLayout *ordersListLayout;
    QPushButton *printBillBtn;
    QPushButton *viewAllOrdersBtn;
};

#endif // HOSPITALIZATION_INFO_WIDGET_H