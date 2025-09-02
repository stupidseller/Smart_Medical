#ifndef DOCTOR_PROFILE_WIDGET_H
#define DOCTOR_PROFILE_WIDGET_H

#include <QWidget>

// 前置声明
class QStackedWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QDateEdit;

// 用于存储医生信息的数据结构
struct DoctorProfileData {
    QString id;
    QString name;
    QString gender;
    QString birthDate;
    QString idCard;
    QString bloodType;
    QString title;
    QString department;
    QString specialty;
    QString officePhone;
    QString mobilePhone;
    QString email;
    QString officeLocation;
};


class DoctorProfileWidget : public QWidget
{
Q_OBJECT

public:
    explicit DoctorProfileWidget(QWidget *parent = nullptr);

signals:
    // 当用户点击“返回首页”时发射此信号
    void backRequested();

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

    // 从数据结构加载数据到UI
    void loadDataToUI();
    // 从UI收集数据到数据结构
    void collectDataFromUI();

    QStackedWidget *mainStack;
    DoctorProfileData currentData;

    // --- 浏览模式的控件 ---
    QLabel *viewNameLabel, *viewGenderLabel, *viewBirthDateLabel, *viewAgeLabel,
            *viewIdCardLabel, *viewBloodTypeLabel, *viewIdLabel, *viewTitleLabel,
            *viewDepartmentLabel, *viewSpecialtyLabel, *viewOfficePhoneLabel,
            *viewMobilePhoneLabel, *viewEmailLabel, *viewOfficeLocationLabel;

    // --- 编辑模式的控件 ---
    QLineEdit *editNameLineEdit, *editIdCardLineEdit, *editSpecialtyLineEdit,
            *editOfficePhoneLineEdit, *editMobilePhoneLineEdit,
            *editEmailLineEdit, *editOfficeLocationLineEdit, *editIdLineEdit;
    QComboBox *editGenderComboBox, *editBloodTypeComboBox, *editTitleComboBox,
            *editDepartmentComboBox;
    QDateEdit *editBirthDateEdit;
};

#endif // DOCTOR_PROFILE_WIDGET_H
