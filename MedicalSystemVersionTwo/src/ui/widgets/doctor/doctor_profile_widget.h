#ifndef DOCTOR_PROFILE_WIDGET_H
#define DOCTOR_PROFILE_WIDGET_H

#include <QWidget>
#include <QJsonObject>

// 前置声明
class QStackedWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QDateEdit;

// 结构化数据（用于 View/表单的绑定）
struct DoctorProfileData {
    QString id;             // 工号（只读）
    QString name;
    QString gender;
    QString birthDate;      // yyyy-MM-dd
    QString idCard;
    QString bloodType;
    QString title;
    QString department;
    QString specialty;
    QString officePhone;
    QString mobilePhone;
    QString email;          // 只读
    QString officeLocation;
};

class DoctorProfileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DoctorProfileWidget(QWidget *parent = nullptr);

signals:
    void backRequested();
    // 交给 Main → Widget::sendUpdateDoctorProfile()
    void saveRequested(const QJsonObject &patch);

public slots:
    // Widget → 本控件
    void setDoctorProfile(const QJsonObject &doctor);
    void showError(const QString &err);
    // Widget 回执
    void onSaveResult(bool ok, const QString &message);

private slots:
    void switchToEditMode();
    void switchToViewMode();
    void saveChanges();

private:
    void initUI();
    void applyStyles();
    QWidget* createViewPage();
    QWidget* createEditPage();
    QWidget* createHeader();
    QWidget* createProfileCard();
    QWidget* createSection(const QString &svgIcon, const QString &title, QWidget *content);

    void loadDataToUI();                 // 结构体 → UI
    QJsonObject buildPatchFromUI() const;// 生成“仅改动字段”的 patch（包含 doctor_id）

    // ===== 成员 =====
    QStackedWidget *mainStack = nullptr;
    DoctorProfileData currentData;       // 最近一次服务器数据（或默认值）
    int m_doctorId = 0;                  // 服务器返回的 doctor_id，用于 patch

    // --- Profile 卡片（抬头区域） ---
    QLabel *cardNameLabel = nullptr;
    QLabel *cardDetailsLabel = nullptr;
    QLabel *cardIdLabel = nullptr;

    // --- 浏览模式 ---
    QLabel *viewNameLabel = nullptr, *viewGenderLabel = nullptr, *viewBirthDateLabel = nullptr, *viewAgeLabel = nullptr,
           *viewIdCardLabel = nullptr, *viewBloodTypeLabel = nullptr, *viewIdLabel = nullptr, *viewTitleLabel = nullptr,
           *viewDepartmentLabel = nullptr, *viewSpecialtyLabel = nullptr, *viewOfficePhoneLabel = nullptr,
           *viewMobilePhoneLabel = nullptr, *viewEmailLabel = nullptr, *viewOfficeLocationLabel = nullptr;

    // --- 编辑模式 ---
    QLineEdit *editNameLineEdit = nullptr, *editIdCardLineEdit = nullptr, *editSpecialtyLineEdit = nullptr,
              *editOfficePhoneLineEdit = nullptr, *editMobilePhoneLineEdit = nullptr,
              *editEmailLineEdit = nullptr, *editOfficeLocationLineEdit = nullptr, *editIdLineEdit = nullptr;
    QComboBox *editGenderComboBox = nullptr, *editBloodTypeComboBox = nullptr, *editTitleComboBox = nullptr,
              *editDepartmentComboBox = nullptr;
    QDateEdit *editBirthDateEdit = nullptr;
};

#endif // DOCTOR_PROFILE_WIDGET_H
