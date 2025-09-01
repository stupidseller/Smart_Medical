#ifndef PROFILE_WIDGET_H
#define PROFILE_WIDGET_H

#include <QWidget>

// 前置声明
class QLineEdit;
class QComboBox;
class QDateEdit;
class QLabel;

class ProfileWidget : public QWidget
{
Q_OBJECT

public:
    explicit ProfileWidget(QWidget *parent = nullptr);
    ~ProfileWidget();

signals:
    // 当用户点击“返回”或“取消”时，可以发出此信号，通知主窗口返回主页
    void backRequested();

private slots:
    // 当出生日期改变时，自动计算并更新年龄
    void updateAge();
    // “保存更改”按钮的槽函数
    void onSaveChangesClicked();


private:
    // --- 后端交互函数 (伪代码) ---
    void loadUserProfileData(); // 从后端加载用户数据
    void saveUserProfileData(); // 将修改后的数据保存到后端

    // --- UI 构建函数 ---
    void initUI();
    void initStyleSheets();
    QWidget* createHeader();
    QWidget* createPatientIdSection();
    QWidget* createFormSection(const QString &iconPath, const QString &title, QWidget* formWidget);
    QWidget* createBasicInfoForm();
    QWidget* createContactInfoForm();
    QWidget* createMedicalInfoForm();
    QWidget* createActionButtons();

    // --- UI 控件成员变量 ---

    // 基本信息
    QLineEdit *nameLineEdit;
    QComboBox *genderComboBox;
    QDateEdit *birthDateEdit;
    QLineEdit *ageLineEdit;
    QLineEdit *idNumberLineEdit;
    QComboBox *bloodTypeComboBox;

    // 联系方式
    QLineEdit *mobileLineEdit;
    QLineEdit *emailLineEdit;
    QLineEdit *emergencyContactLineEdit;
    QLineEdit *emergencyPhoneLineEdit;

    // 医疗信息
    QComboBox *insuranceTypeComboBox;
    QLineEdit *insuranceCardLineEdit;
    QLineEdit *allergiesLineEdit;
    QLineEdit *historyLineEdit;
};

#endif // PROFILE_WIDGET_H