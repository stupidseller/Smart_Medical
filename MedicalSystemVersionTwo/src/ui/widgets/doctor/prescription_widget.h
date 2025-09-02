#ifndef PRESCRIPTION_WIDGET_H
#define PRESCRIPTION_WIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QHash>
#include <QStringList>

// 前置声明
class QVBoxLayout;
class QComboBox;
class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;

class PrescriptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PrescriptionWidget(QWidget *parent = nullptr);
    ~PrescriptionWidget();

    // 构建完整处方 JSON（已存在的出参，保留）
    QJsonObject buildPrescriptionJson() const;

    // 新增：导出购物车条目；供外部（DoctorMainWindow）直接提交/暂存
    QJsonArray  cartFromCurrentSelection() const;

    // 新增：获取当前患者 ID
    int currentPatientId() const { return m_currentPatientId; }

signals:
    void backRequested();
    void cancelRequested();
    void saveRequested(const QJsonObject &prescription);
    void issueRequested(const QJsonObject &prescription);
    void currentPatientChanged(int patientId);

public slots:
    // —— 数据绑定接口（Widget 提供数据灌入）——
    void setPatientList(const QJsonArray &patients);   // [{id,name,patient_id,gender,age,...}]
    void selectPatientById(int id);                    // 外部可直接选中患者
    void setDiagnosis(const QJsonObject &diagnosis);   // {main, department}
    void setAdvice(const QJsonObject &advice);         // {guidance, precautions}

    // 药品目录（来自 Widget::loadMedicineDataOk）
    void setMedicineCatalog(const QJsonArray &medicines); // [{id,name,spec,price,...}]

    // 药品行（可选外部注入）
    void setDrugs(const QJsonArray &drugs);            // [{name,spec,quantity,dosage,days}]
    void addDrug(const QJsonObject &drug);
    void clearDrugs();

private slots:
    void onAddDrugClicked();
    void onRemoveDrugClicked();
    void onCancelClicked();
    void onSaveClicked();
    void onIssueClicked();
    void onPatientComboChanged(int index);

private:
    void initUI();
    void applyStyles();

    QWidget* createHeader();
    QWidget* createPatientSelection();
    QWidget* createDiagnosisSection();
    QWidget* createDrugsSection();
    QWidget* createAdviceSection();
    QWidget* createFooterButtons();
    QWidget* createSectionTitle(const char* svgIcon, const QString& title);

    // 药品行结构
    struct DrugRow {
        QWidget*    row = nullptr;
        QComboBox*  name = nullptr;
        QLineEdit*  spec = nullptr;
        QLineEdit*  quantity = nullptr;
        QLineEdit*  dosage = nullptr;
        QLineEdit*  days = nullptr;
        QPushButton* removeBtn = nullptr;
    };
    DrugRow* createDrugRow(const QJsonObject *preset = nullptr);
    void     deleteDrugRow(DrugRow* r);
    QJsonArray serializeDrugs() const;

    // 患者信息刷新
    void updatePatientInfoLabels(int patientId);

private:
    // —— 顶部/病人选择 ——
    QComboBox* m_patientCombo = nullptr;
    QLabel*    m_labelName = nullptr;
    QLabel*    m_labelPid  = nullptr;
    QLabel*    m_labelGender = nullptr;
    QLabel*    m_labelAge = nullptr;

    // —— 诊断 ——
    QLineEdit* m_mainDiagnosisEdit = nullptr;
    QComboBox* m_departmentCombo   = nullptr;

    // —— 药品行 ——
    QVBoxLayout*      m_drugsLayout = nullptr;
    QList<DrugRow*>   m_drugRows;

    // —— 医嘱与建议 ——
    QTextEdit* m_guidanceEdit = nullptr;
    QTextEdit* m_precautionsEdit = nullptr;

    // —— 数据缓存 ——
    QHash<int, QJsonObject> m_patientMap;
    int m_currentPatientId = -1;

    // 新增：药品目录缓存（name/id -> obj），用于找单价等
    QHash<QString, QJsonObject> m_medsByName;
    QHash<int,     QJsonObject> m_medsById;
    QStringList                 m_medNames;
};

#endif // PRESCRIPTION_WIDGET_H
