#include "login_dialog.h"
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>

LoginDialog::LoginDialog(QWidget *parent)
        : QDialog(parent)
{
    initLayout();
    initStyleSheets();
    onLoginTabClicked();
}

LoginDialog::~LoginDialog() {}

void LoginDialog::initLayout() {
    setWindowTitle("智能医疗系统");
    resize(900, 600);
    setMinimumSize(900, 600);
    setObjectName("LoginDialog");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(createLeftPanel());
    mainLayout->addWidget(createRightPanel());
}

QWidget* LoginDialog::createLeftPanel() {
    QWidget *panel = new QWidget();
    panel->setObjectName("leftPanel");
    panel->setFixedWidth(350);

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(40, 50, 40, 50);
    layout->setSpacing(10);

    QLabel *titleLabel = new QLabel("智能医疗系统");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setWordWrap(true);

    QLabel *subtitleLabel = new QLabel("为您提供全面的医疗健康服务");
    subtitleLabel->setObjectName("subtitleLabel");

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();
    return panel;
}

QWidget* LoginDialog::createRightPanel() {
    QWidget *panel = new QWidget();
    panel->setObjectName("rightPanel");
    QVBoxLayout *rightLayout = new QVBoxLayout(panel);
    rightLayout->setContentsMargins(60, 40, 60, 40);

    // Tab 按钮
    loginTabButton = new QPushButton("登录");
    registerTabButton = new QPushButton("注册");
    loginTabButton->setObjectName("tabButton");
    registerTabButton->setObjectName("tabButton");
    QHBoxLayout *tabLayout = new QHBoxLayout();
    tabLayout->setSpacing(20);
    tabLayout->addWidget(loginTabButton);
    tabLayout->addWidget(registerTabButton);
    tabLayout->addStretch();

    // 页面堆
    mainStackedWidget = new QStackedWidget(this);
    mainStackedWidget->addWidget(createLoginWidget());
    mainStackedWidget->addWidget(createRegisterWidget());

    rightLayout->addLayout(tabLayout);
    rightLayout->addWidget(mainStackedWidget);

    connect(loginTabButton, &QPushButton::clicked, this, &LoginDialog::onLoginTabClicked);
    connect(registerTabButton, &QPushButton::clicked, this, &LoginDialog::onRegisterTabClicked);
    return panel;
}

QWidget* LoginDialog::createLoginWidget() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 20, 0, 0);
    layout->setSpacing(20);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    loginUserLineEdit = new QLineEdit();
    loginUserLineEdit->setPlaceholderText("请输入用户名或邮箱");
    loginPasswordLineEdit = new QLineEdit();
    loginPasswordLineEdit->setPlaceholderText("请输入密码");
    loginPasswordLineEdit->setEchoMode(QLineEdit::Password);
    loginRoleComboBox = new QComboBox();
    loginRoleComboBox->addItem("  请选择身份");
    loginRoleComboBox->addItem("  患者");
    loginRoleComboBox->addItem("  医生");

    formLayout->addRow("用户名/邮箱", loginUserLineEdit);
    formLayout->addRow("密码", loginPasswordLineEdit);
    formLayout->addRow("登录身份", loginRoleComboBox);

    QPushButton *loginButton = new QPushButton("登录");
    loginButton->setObjectName("primaryButton");
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginAttempt);

    QPushButton *forgotPasswordButton = new QPushButton("忘记密码?");
    forgotPasswordButton->setObjectName("linkButton");

    layout->addLayout(formLayout);
    layout->addSpacing(10);
    layout->addWidget(loginButton);
    layout->addWidget(forgotPasswordButton, 0, Qt::AlignRight);
    layout->addStretch();
    return widget;
}

QWidget* LoginDialog::createRegisterWidget() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 20, 0, 0);
    layout->setSpacing(15);

    // 用户类型选择
    QHBoxLayout *typeLayout = new QHBoxLayout();
    patientTypeButton = new QPushButton("患者\n使用医疗服务");
    doctorTypeButton = new QPushButton("医生\n提供医疗服务");
    patientTypeButton->setObjectName("typeButton");
    doctorTypeButton->setObjectName("typeButton");
    typeLayout->addWidget(patientTypeButton);
    typeLayout->addWidget(doctorTypeButton);
    connect(patientTypeButton, &QPushButton::clicked, this, &LoginDialog::onPatientTypeClicked);
    connect(doctorTypeButton, &QPushButton::clicked, this, &LoginDialog::onDoctorTypeClicked);

    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    registerUserLineEdit = new QLineEdit();
    registerUserLineEdit->setPlaceholderText("请设置用户名");
    registerEmailLineEdit = new QLineEdit();
    registerEmailLineEdit->setPlaceholderText("请输入邮箱");
    registerPasswordLineEdit = new QLineEdit();
    registerPasswordLineEdit->setPlaceholderText("请设置密码 (至少6位)");
    registerPasswordLineEdit->setEchoMode(QLineEdit::Password);
    registerConfirmPasswordLineEdit = new QLineEdit();
    registerConfirmPasswordLineEdit->setPlaceholderText("请再次输入密码");
    registerConfirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("用户名", registerUserLineEdit);
    formLayout->addRow("邮箱", registerEmailLineEdit);
    formLayout->addRow("密码", registerPasswordLineEdit);
    formLayout->addRow("确认密码", registerConfirmPasswordLineEdit);

    // 添加科室选项，默认隐藏
    departmentLabel = new QLabel("科室");
    registerDepartmentComboBox = new QComboBox();
    registerDepartmentComboBox->addItem("  请选择科室");
    registerDepartmentComboBox->addItem("  内科");
    registerDepartmentComboBox->addItem("  外科");
    registerDepartmentComboBox->addItem("  儿科");
    registerDepartmentComboBox->addItem("  妇产科");
    registerDepartmentComboBox->addItem("  五官科");
    registerDepartmentComboBox->addItem("  皮肤科");
    registerDepartmentComboBox->addItem("  中医科");
    formLayout->addRow(departmentLabel, registerDepartmentComboBox);


    // 注册和返回按钮
    QPushButton *registerButton = new QPushButton("注册");
    registerButton->setObjectName("primaryButton");
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterAttempt);

    QPushButton *backToLoginButton = new QPushButton("已有账号? 返回登录");
    backToLoginButton->setObjectName("linkButton");
    connect(backToLoginButton, &QPushButton::clicked, this, &LoginDialog::onLoginTabClicked);

    layout->addLayout(typeLayout);
    layout->addSpacing(10);
    layout->addLayout(formLayout);
    layout->addWidget(registerButton);
    layout->addWidget(backToLoginButton, 0, Qt::AlignHCenter);
    layout->addStretch();

    // 初始化时默认选择患者，并隐藏科室选项
    onPatientTypeClicked();
    patientTypeButton->setProperty("selected", true);
    style()->unpolish(patientTypeButton);
    style()->polish(patientTypeButton);

    return widget;
}

void LoginDialog::onRegisterAttempt() {
    // 前端验证
    if (registerUserLineEdit->text().isEmpty() ||
            registerEmailLineEdit->text().isEmpty() ||
        registerPasswordLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "注册失败", "用户名、邮箱和密码均为必填项。");
        return;
    }
    if (registerPasswordLineEdit->text() != registerConfirmPasswordLineEdit->text()) {
        QMessageBox::warning(this, "注册失败", "两次输入的密码不一致。");
        return;
    }
    // 如果是医生注册，验证科室是否已选择
    if (selectedUserType == "doctor" && registerDepartmentComboBox->currentIndex() == 0) {
        QMessageBox::warning(this, "注册失败", "请为医生选择一个科室。");
        return;
    }

    const QString username   = registerUserLineEdit->text().trimmed();
        const QString email      = registerEmailLineEdit->text().trimmed();
        const QString password   = registerPasswordLineEdit->text();
        const QString userType   = selectedUserType; // "patient" / "doctor"
        const QString department = (userType == "doctor")
                                   ? registerDepartmentComboBox->currentText().trimmed()
                                   : QString();

    emit registerRequested(username, email, password, userType, department);
    //QMessageBox::information(this, "注册请求已发送", "已向服务器发送注册请求，请稍候查看结果。");
}


void LoginDialog::onLoginTabClicked() {
    mainStackedWidget->setCurrentIndex(0);
    loginTabButton->setProperty("active", true);
    registerTabButton->setProperty("active", false);
    style()->unpolish(loginTabButton); style()->polish(loginTabButton);
    style()->unpolish(registerTabButton); style()->polish(registerTabButton);
}

void LoginDialog::onRegisterTabClicked() {
    mainStackedWidget->setCurrentIndex(1);
    loginTabButton->setProperty("active", false);
    registerTabButton->setProperty("active", true);
    style()->unpolish(loginTabButton); style()->polish(loginTabButton);
    style()->unpolish(registerTabButton); style()->polish(registerTabButton);
}

void LoginDialog::onPatientTypeClicked() {
    selectedUserType = "patient";
    patientTypeButton->setProperty("selected", true);
    doctorTypeButton->setProperty("selected", false);
    style()->unpolish(patientTypeButton); style()->polish(patientTypeButton);
    style()->unpolish(doctorTypeButton); style()->polish(doctorTypeButton);
    departmentLabel->setVisible(false);
    registerDepartmentComboBox->setVisible(false);
}

void LoginDialog::onDoctorTypeClicked() {
    selectedUserType = "doctor";
    patientTypeButton->setProperty("selected", false);
    doctorTypeButton->setProperty("selected", true);
    style()->unpolish(patientTypeButton); style()->polish(patientTypeButton);
    style()->unpolish(doctorTypeButton); style()->polish(doctorTypeButton);
    departmentLabel->setVisible(true);
    registerDepartmentComboBox->setVisible(true);
}

void LoginDialog::onLoginAttempt() {
    if (loginUserLineEdit->text().isEmpty() ||
        loginPasswordLineEdit->text().isEmpty() ||
        loginRoleComboBox->currentIndex() == 0) {
        QMessageBox::warning(this, "登录失败", "所有字段均为必填项。");
        return;
    }

    const QString username = loginUserLineEdit->text().trimmed();
    const QString password = loginPasswordLineEdit->text();
    const QString role     = loginRoleComboBox->currentText().trimmed(); // "患者"/"医生"（你的下拉文本）

    emit loginRequested(username, password, role);
    //QMessageBox::information(this, "登录请求已发送", "已向服务器发送登录请求，请稍候查看结果。");
}

void LoginDialog::initStyleSheets() {
    QString qss = R"(
        #LoginDialog { background-color: #F7FAFC; }
        #leftPanel { background-color: #2C5282; }
        #titleLabel { color: white; font-size: 32px; font-weight: bold; }
        #subtitleLabel { color: #E2E8F0; font-size: 16px; }
        #rightPanel { background-color: #FFFFFF; font-family: 'Microsoft YaHei'; }

        #tabButton {
            font-size: 22px; font-weight: bold; color: #A0AEC0;
            border: none; background-color: transparent; padding-bottom: 5px;
        }
        #tabButton[active="true"] { color: #2B6CB0; border-bottom: 3px solid #2B6CB0; }

        QLineEdit {
            border: 1px solid #E2E8F0; border-radius: 8px;
            padding: 12px; font-size: 14px;
        }
        QLineEdit:focus { border-color: #3182CE; }
        QLabel { font-size: 14px; color: #1A202C; }

        /* === Start of Updated QComboBox Styles === */
        QComboBox {
            border: 1px solid #E2E8F0; border-radius: 8px;
            padding: 12px; font-size: 14px;
            background-color: white; /* Ensure combo box background is white */
        }

        /* Style the popup list to be white */
        QComboBox QAbstractItemView {
            background-color: white;
            color: #1A202C;
            border: 1px solid #E2E8F0;
            border-radius: 4px;
            selection-background-color: #EBF8FF; /* Light blue for selected item */
            selection-color: #2B6CB0;
            outline: 0px; /* Remove the focus border on the popup */
        }

        /* Style the dropdown button to remove the grey background but show the arrow */
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 25px;
            border-left-width: 0px;
            border-top-right-radius: 8px;
            border-bottom-right-radius: 8px;
            background: transparent; /* Keep the button background transparent */
        }

        QComboBox::down-arrow {
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTcgMTBMMTIgMTVMMTcgMTBIMVoiIGZpbGw9IiM2MzczNzciLz4KPC9zdmc+); /* Base64 encoded SVG for a simple down arrow */
            width: 12px; /* Adjust arrow size */
            height: 12px;
        }

        QComboBox:focus { border-color: #3182CE; }
        /* === End of Updated QComboBox Styles === */

        #primaryButton {
            background-color: #3182CE; color: white; border-radius: 8px;
            padding: 12px; font-size: 16px; font-weight: bold;
        }
        #primaryButton:hover { background-color: #2B6CB0; }

        #linkButton {
            background-color: transparent; color: #4A5568;
            border: none; font-size: 13px;
        }
        #linkButton:hover { color: #2B6CB0; }

        #typeButton {
            border: 1px solid #E2E8F0; border-radius: 8px; padding: 15px;
            text-align: left; font-size: 14px; line-height: 1.5;
        }
        #typeButton[selected="true"] {
            border: 2px solid #3182CE; background-color: #EBF8FF;
        }
    )";
    this->setStyleSheet(qss);
}
