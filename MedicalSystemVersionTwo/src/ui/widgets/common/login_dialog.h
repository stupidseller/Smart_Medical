#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>


// 前置声明
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel; // 添加 QLabel 的前置声明

class LoginDialog : public QDialog
{
Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
signals:
    // 登录/注册请求，交给外部(Widget)去通过TCP发送
        void loginRequested(const QString &username,
                            const QString &password,
                            const QString &roleText);      // “患者”/“医生”

        void registerRequested(const QString &username,
                               const QString &email,
                               const QString &password,
                               const QString &userType,     // "patient"/"doctor"
                               const QString &department);  // 患者为空串

public slots:
    void onLoginTabClicked();
private slots:
    void onRegisterTabClicked();
    void onPatientTypeClicked();
    void onDoctorTypeClicked();
    void onLoginAttempt();
    void onRegisterAttempt();

private:
    void initLayout();
    void initStyleSheets();
    QWidget* createLeftPanel();
    QWidget* createRightPanel();
    QWidget* createLoginWidget();
    QWidget* createRegisterWidget();

    // 控件指针
    QStackedWidget *mainStackedWidget;
    QPushButton *loginTabButton;
    QPushButton *registerTabButton;
    QLineEdit *loginUserLineEdit;
    QLineEdit *loginPasswordLineEdit;
    QComboBox *loginRoleComboBox;
    QPushButton *patientTypeButton;
    QPushButton *doctorTypeButton;
    QLineEdit *registerUserLineEdit;
    QLineEdit *registerEmailLineEdit;
    QLineEdit *registerPasswordLineEdit;
    QLineEdit *registerConfirmPasswordLineEdit;

    // 为科室选择添加的控件
    QLabel *departmentLabel;
    QComboBox *registerDepartmentComboBox;

    QString selectedUserType; // "patient"/"doctor"
};

#endif // LOGIN_DIALOG_H
