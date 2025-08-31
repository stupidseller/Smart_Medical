#ifndef PROFILE_WIDGET_H
#define PROFILE_WIDGET_H
#include <QJsonObject>
#include <QWidget>

class QLineEdit;
class QComboBox;
class QDateEdit;
class QLabel;
class Widget;

class ProfileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileWidget(Widget *api, int patientId, QWidget *parent = nullptr);
    ~ProfileWidget();

signals:
    void backRequested();

private slots:
    void onProfileLoaded(const QJsonObject &profile);
    void onProfileSaved(bool ok, const QString &msg);
    void updateAge();
    void onSaveChangesClicked();

private:
    void initUI();
    void initStyleSheets();

    void loadUserProfileData();
    void fillFormFromProfile(const QJsonObject &p);
    QJsonObject buildPatchFromForm() const;

private:
    Widget *m_api = nullptr;
    int     m_patientId = -1;
    QJsonObject m_lastProfile;

    QWidget* createHeader();
    QWidget* createPatientIdSection();
    QWidget* createFormSection(const QString &iconPath, const QString &title, QWidget* formWidget);
    QWidget* createBasicInfoForm();
    QWidget* createContactInfoForm();
    QWidget* createMedicalInfoForm();
    QWidget* createActionButtons();

    // 头部显示用（新增）
    QLabel *headerNameLabel = nullptr;
    QLabel *headerIdLabel   = nullptr;

    QLineEdit *nameLineEdit;
    QComboBox *genderComboBox;
    QDateEdit *birthDateEdit;
    QLineEdit *ageLineEdit;
    QLineEdit *idNumberLineEdit;
    QComboBox *bloodTypeComboBox;
    QLineEdit *mobileLineEdit;
    QLineEdit *emailLineEdit;
    QLineEdit *emergencyContactLineEdit;
    QLineEdit *emergencyPhoneLineEdit;
    QComboBox *insuranceTypeComboBox;
    QLineEdit *insuranceCardLineEdit;
    QLineEdit *allergiesLineEdit;
    QLineEdit *historyLineEdit;
};

#endif // PROFILE_WIDGET_H
