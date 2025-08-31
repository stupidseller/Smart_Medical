/**
 * @file login_dialog.cpp
 * @brief 登录对话框的实现文件
 * 
 * 这个文件实现了医疗系统的登录和注册界面，包括：
 * - 登录界面：用户名/邮箱、密码、角色选择
 * - 注册界面：用户类型选择、基本信息填写、医生科室选择
 * - 表单验证
 * - 界面样式设置
 */

#include "login_dialog.h"
#include <QStackedWidget>    // 用于切换登录/注册页面
#include <QLineEdit>         // 文本输入框
#include <QPushButton>       // 按钮控件
#include <QComboBox>         // 下拉选择框
#include <QLabel>            // 标签控件
#include <QGridLayout>       // 网格布局
#include <QVBoxLayout>       // 垂直布局
#include <QHBoxLayout>       // 水平布局
#include <QFormLayout>       // 表单布局
#include <QMessageBox>       // 消息框
#include <QStyle>            // 样式相关
#include <QTimer>            // 定时器

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
LoginDialog::LoginDialog(QWidget *parent)
        : QDialog(parent)
{
    initLayout();           // 初始化界面布局
    initStyleSheets();      // 初始化样式表
    onLoginTabClicked();    // 默认显示登录页面
}

LoginDialog::~LoginDialog() {}

/**
 * @brief 初始化对话框布局
 * 
 * 创建主布局，包括：
 * - 设置窗口标题和大小
 * - 创建水平布局
 * - 添加左侧面板（系统标题和说明）
 * - 添加右侧面板（登录注册界面）
 */
void LoginDialog::initLayout() {
    setWindowTitle("智能医疗系统");
    resize(900, 600);                    // 设置窗口初始大小
    setMinimumSize(900, 600);            // 设置最小大小限制
    setObjectName("LoginDialog");        // 设置对象名，用于样式表

    // 创建主水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // 设置边距为0
    mainLayout->setSpacing(0);                    // 设置部件间距为0
    mainLayout->addWidget(createLeftPanel());     // 添加左侧面板
    mainLayout->addWidget(createRightPanel());    // 添加右侧面板
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

/**
 * @brief 处理注册按钮点击事件
 * 
 * 执行注册表单的验证，包括：
 * 1. 必填项验证
 * 2. 邮箱格式验证
 * 3. 密码一致性验证
 * 4. 医生科室选择验证
 */
void LoginDialog::onRegisterAttempt() {
    // 验证必填项
    if (registerUserLineEdit->text().isEmpty() ||
            registerEmailLineEdit->text().isEmpty() ||
        registerPasswordLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "注册失败", "用户名、邮箱和密码均为必填项。");
        return;
    }

    // 获取并验证邮箱格式
    const QString email = registerEmailLineEdit->text().trimmed();
    // 检查邮箱是否包含@和.，且@不在开头或结尾，并且只能有一个@
    if (!email.contains("@") || !email.contains(".") || 
        email.indexOf("@") == 0 || 
        email.indexOf("@") == email.length() - 1 ||
        email.count("@") > 1) {
        QMessageBox::warning(this, "注册失败", "请输入正确的邮箱格式（例如：example@domain.com）");
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

/**
 * @brief 处理选择患者类型的点击事件
 * 
 * 当用户选择注册为患者时：
 * 1. 设置用户类型为patient
 * 2. 更新按钮样式
 * 3. 隐藏科室选择相关控件
 */
void LoginDialog::onPatientTypeClicked() {
    selectedUserType = "patient";
    // 更新按钮选中状态
    patientTypeButton->setProperty("selected", true);
    doctorTypeButton->setProperty("selected", false);
    // 刷新按钮样式
    style()->unpolish(patientTypeButton); style()->polish(patientTypeButton);
    style()->unpolish(doctorTypeButton); style()->polish(doctorTypeButton);
    // 隐藏科室选择
    departmentLabel->setVisible(false);
    registerDepartmentComboBox->setVisible(false);
}

/**
 * @brief 处理选择医生类型的点击事件
 * 
 * 当用户选择注册为医生时：
 * 1. 设置用户类型为doctor
 * 2. 更新按钮样式
 * 3. 显示科室选择相关控件
 */
void LoginDialog::onDoctorTypeClicked() {
    selectedUserType = "doctor";
    // 更新按钮选中状态
    patientTypeButton->setProperty("selected", false);
    doctorTypeButton->setProperty("selected", true);
    // 刷新按钮样式
    style()->unpolish(patientTypeButton); style()->polish(patientTypeButton);
    style()->unpolish(doctorTypeButton); style()->polish(doctorTypeButton);
    // 显示科室选择
    departmentLabel->setVisible(true);
    registerDepartmentComboBox->setVisible(true);
}

/**
 * @brief 处理登录按钮点击事件
 * 
 * 执行登录表单的验证和处理：
 * 1. 验证所有必填字段
 * 2. 发送登录请求信号
 */
/**
 * @brief 清空所有输入框的内容
 * 
 * 清空登录和注册表单中的所有输入内容，包括：
 * - 用户名/邮箱输入框
 * - 密码输入框
 * - 确认密码输入框
 * - 下拉选择框重置为默认选项
 */
void LoginDialog::clearInputs() {
    // 清空登录表单
    loginUserLineEdit->clear();
    loginPasswordLineEdit->clear();
    loginRoleComboBox->setCurrentIndex(0);
    
    // 清空注册表单
    registerUserLineEdit->clear();
    registerEmailLineEdit->clear();
    registerPasswordLineEdit->clear();
    registerConfirmPasswordLineEdit->clear();
    registerDepartmentComboBox->setCurrentIndex(0);
}

void LoginDialog::onLoginAttempt() {
    // 验证所有字段是否已填写
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
    
    // 登录请求发送后清空输入框
    clearInputs();
}

/**
 * @brief 初始化界面样式表
 * 
 * 设置整个登录对话框的样式，包括：
 * - 整体配色方案
 * - 按钮样式
 * - 输入框样式
 * - 下拉框样式
 * - 标签样式
 * - 各种状态下的样式变化
 */
void LoginDialog::initStyleSheets() {
    // 使用Qt Raw String语法定义样式表
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
