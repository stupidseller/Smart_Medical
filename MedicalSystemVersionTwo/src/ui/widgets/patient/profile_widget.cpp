#include "profile_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QDate>
#include <QMessageBox>
#include <QRegularExpression> // 新增：用于实现身份证号验证

ProfileWidget::ProfileWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("ProfileWidget");
    initUI();
    initStyleSheets();

    // 控件创建后，立即从后端加载数据
    loadUserProfileData();
}

ProfileWidget::~ProfileWidget() {}

void ProfileWidget::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 无边距，由滚动区内的Widget控制

    // 使用滚动区确保在小屏幕上内容也能完整显示
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *mainContentWidget = new QWidget();
    mainContentWidget->setObjectName("mainContentWidget");
    QVBoxLayout *contentLayout = new QVBoxLayout(mainContentWidget);
    contentLayout->setContentsMargins(40, 20, 40, 40);
    contentLayout->setSpacing(25);

    // 添加各个UI部分
    contentLayout->addWidget(createHeader());
    contentLayout->addWidget(createPatientIdSection());
    contentLayout->addWidget(createFormSection("", "基本信息", createBasicInfoForm()));
    contentLayout->addWidget(createFormSection("", "联系方式", createContactInfoForm()));
    contentLayout->addWidget(createFormSection("", "医疗信息", createMedicalInfoForm()));
    contentLayout->addStretch();
    contentLayout->addWidget(createActionButtons());

    scrollArea->setWidget(mainContentWidget);
    mainLayout->addWidget(scrollArea);
}

// --- 后端交互函数 (伪代码实现) ---

void ProfileWidget::loadUserProfileData() {
    //
    // 后端交互函数：获取当前登录用户的个人信息
    // API端点建议: GET /api/patient/profile
    // 成功响应 (JSON):
    // {
    //   "name": "张患者", "gender": "女", "birthDate": "1985-06-15", ...
    // }
    //

    // --- 前端伪代码 ---
    // 假设已从后端获取数据，现在填充到UI控件中
    nameLineEdit->setText("张患者");
    genderComboBox->setCurrentText("女");
    birthDateEdit->setDate(QDate(1985, 6, 15));
    updateAge(); // 根据出生日期计算年龄
    idNumberLineEdit->setText("310105198506151234"); // 示例数据改为18位
    bloodTypeComboBox->setCurrentText("O型");

    mobileLineEdit->setText("13812345678");
    emailLineEdit->setText("zhang***@example.com"); // 邮箱不可修改
    emergencyContactLineEdit->setText("李明 (丈夫)");
    emergencyPhoneLineEdit->setText("13912345678");

    insuranceTypeComboBox->setCurrentText("城镇职工基本医疗保险");
    insuranceCardLineEdit->setText("YB1234567890");
    allergiesLineEdit->setText("青霉素、海鲜");
    historyLineEdit->setText("高血压、轻度脂肪肝");
}

void ProfileWidget::saveUserProfileData() {
    //
    // 后端交互函数：提交修改后的用户个人信息
    // API端点建议: PUT /api/patient/profile
    // 请求体 (JSON):
    // {
    //   "idNumber": "310105198506151234",
    //   "insuranceCard": "YB1234567890",
    //   "mobile": "13812345678",
    //   ...
    // }
    //

    // --- 前端伪代码 ---
    // 1. 从UI控件中收集已修改的数据
    QString idNumber = idNumberLineEdit->text();
    QString mobile = mobileLineEdit->text();
    QString emergencyContact = emergencyContactLineEdit->text();
    // ... 获取其他可修改字段的值 ...

    // 2. 进行前端数据验证
    // **已修改：增加身份证号验证逻辑**
    QRegularExpression idRegex("^\\d{18}$"); // 正则表达式，严格匹配18位数字
    if (!idRegex.match(idNumber).hasMatch()) {
        QMessageBox::warning(this, "输入错误", "请输入有效的18位数字身份证号码。");
        return; // 验证失败，中断保存操作
    }

    if (mobile.length() != 11) { // 此处可以替换为更复杂的手机号正则验证
        QMessageBox::warning(this, "提示", "请输入有效的手机号码。");
        return;
    }

    // 3. 发送数据到后端 (此处用消息框模拟成功)
    QMessageBox::information(this, "成功", "您的个人信息已保存。");

    // 4. (可选) 保存成功后可以触发返回操作
    emit backRequested();
}

// --- UI 构建函数实现 ---

QWidget* ProfileWidget::createHeader() {
    QWidget *header = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0,0,0,0);

    QLabel* title = new QLabel("个人信息");
    title->setObjectName("pageTitle");

    // **已修改：按钮文本从“返回仪表盘”改为“返回主页”**
    QPushButton *backButton = new QPushButton("返回主页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &ProfileWidget::backRequested);

    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(backButton);

    return header;
}


QWidget* ProfileWidget::createPatientIdSection() {
    QWidget *section = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(section);
    layout->setSpacing(20);

    QLabel* icon = new QLabel();
    icon->setObjectName("patientIcon");
    icon->setFixedSize(64, 64);

    QLabel *name = new QLabel("张患者");
    name->setObjectName("patientName");
    QLabel *id = new QLabel("ID: P202305001");
    id->setObjectName("patientId");

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(5);
    textLayout->addWidget(name);
    textLayout->addWidget(id);

    layout->addWidget(icon);
    layout->addLayout(textLayout);
    layout->addStretch();

    return section;
}

QWidget* ProfileWidget::createFormSection(const QString &iconPath, const QString &title, QWidget* formWidget) {
    QFrame *sectionBox = new QFrame();
    sectionBox->setObjectName("sectionBox");
    QVBoxLayout *layout = new QVBoxLayout(sectionBox);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("sectionTitle");

    layout->addWidget(titleLabel);
    layout->addWidget(formWidget);

    return sectionBox;
}

QWidget* ProfileWidget::createBasicInfoForm() {
    QWidget *form = new QWidget();
    QFormLayout *layout = new QFormLayout(form);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->setHorizontalSpacing(20);
    layout->setVerticalSpacing(15);

    nameLineEdit = new QLineEdit();
    nameLineEdit->setReadOnly(true);
    nameLineEdit->setProperty("class", "readOnlyField");

    genderComboBox = new QComboBox();
    genderComboBox->addItems({"男", "女"});

    birthDateEdit = new QDateEdit();
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("yyyy/MM/dd");
    connect(birthDateEdit, &QDateEdit::dateChanged, this, &ProfileWidget::updateAge);

    ageLineEdit = new QLineEdit();
    ageLineEdit->setReadOnly(true);
    ageLineEdit->setProperty("class", "readOnlyField");
    ageLineEdit->setFixedWidth(100);

    QHBoxLayout *birthAgeLayout = new QHBoxLayout();
    birthAgeLayout->addWidget(birthDateEdit);
    birthAgeLayout->addSpacing(20);
    birthAgeLayout->addWidget(new QLabel("年龄"));
    birthAgeLayout->addWidget(ageLineEdit);
    birthAgeLayout->addStretch();

    idNumberLineEdit = new QLineEdit();
    // **已修改：移除身份证号的只读属性，使其可编辑**
    // idNumberLineEdit->setReadOnly(true);
    // idNumberLineEdit->setProperty("class", "readOnlyField");

    bloodTypeComboBox = new QComboBox();
    bloodTypeComboBox->addItems({"A型", "B型", "AB型", "O型", "Rh+"});

    layout->addRow("姓名:", nameLineEdit);
    layout->addRow("性别:", genderComboBox);
    layout->addRow("出生日期:", birthAgeLayout);
    layout->addRow("身份证号:", idNumberLineEdit);
    layout->addRow("血型:", bloodTypeComboBox);

    return form;
}

QWidget* ProfileWidget::createContactInfoForm() {
    QWidget *form = new QWidget();
    QFormLayout *layout = new QFormLayout(form);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->setHorizontalSpacing(20);
    layout->setVerticalSpacing(15);

    mobileLineEdit = new QLineEdit(); // 手机号可修改
    emailLineEdit = new QLineEdit();
    emailLineEdit->setReadOnly(true); // 邮箱不可修改
    emailLineEdit->setProperty("class", "readOnlyField");

    QLabel *emailHint = new QLabel("此信息不可修改");
    emailHint->setObjectName("fieldHint");

    QVBoxLayout* emailLayout = new QVBoxLayout();
    emailLayout->setSpacing(5);
    emailLayout->addWidget(emailLineEdit);
    emailLayout->addWidget(emailHint);

    emergencyContactLineEdit = new QLineEdit();
    emergencyPhoneLineEdit = new QLineEdit();

    layout->addRow("手机号码:", mobileLineEdit);
    layout->addRow("电子邮箱:", emailLayout);
    layout->addRow("紧急联系人:", emergencyContactLineEdit);
    layout->addRow("紧急联系电话:", emergencyPhoneLineEdit);

    return form;
}

QWidget* ProfileWidget::createMedicalInfoForm() {
    QWidget *form = new QWidget();
    QFormLayout *layout = new QFormLayout(form);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->setHorizontalSpacing(20);
    layout->setVerticalSpacing(15);

    insuranceTypeComboBox = new QComboBox();
    insuranceTypeComboBox->addItems({"城镇职工基本医疗保险", "城乡居民基本医疗保险", "公费医疗", "商业保险", "自费"});

    insuranceCardLineEdit = new QLineEdit();
    // **已修改：移除医保卡号的只读属性，使其可编辑**
    // insuranceCardLineEdit->setReadOnly(true);
    // insuranceCardLineEdit->setProperty("class", "readOnlyField");

    allergiesLineEdit = new QLineEdit();
    historyLineEdit = new QLineEdit();

    layout->addRow("医保类型:", insuranceTypeComboBox);
    layout->addRow("医保卡号:", insuranceCardLineEdit);
    layout->addRow("过敏史:", allergiesLineEdit);
    layout->addRow("既往病史:", historyLineEdit);

    return form;
}

QWidget* ProfileWidget::createActionButtons() {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->addStretch();

    QPushButton* cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, &QPushButton::clicked, this, &ProfileWidget::backRequested);

    QPushButton* saveButton = new QPushButton("保存更改");
    saveButton->setObjectName("saveButton");
    connect(saveButton, &QPushButton::clicked, this, &ProfileWidget::onSaveChangesClicked);

    layout->addWidget(cancelButton);
    layout->addWidget(saveButton);
    layout->addStretch();
    container->setFixedHeight(80);

    return container;
}


// --- 槽函数实现 ---

void ProfileWidget::updateAge() {
    QDate birthDate = birthDateEdit->date();
    QDate currentDate = QDate::currentDate();
    int age = currentDate.year() - birthDate.year();
    if (currentDate.month() < birthDate.month() ||
        (currentDate.month() == birthDate.month() && currentDate.day() < birthDate.day())) {
        age--;
    }
    ageLineEdit->setText(QString::number(age));
}

void ProfileWidget::onSaveChangesClicked() {
    // 调用包含验证逻辑的保存函数
    saveUserProfileData();
}

// --- 样式表 ---
void ProfileWidget::initStyleSheets() {
    QString qss = R"(
        #ProfileWidget, #mainContentWidget {
            background-color: #F7FAFC;
        }
        #scrollArea {
             border: none;
        }
        #pageTitle {
            font-size: 22px;
            font-weight: bold;
            color: #2D3748;
        }
        #backButton {
            background-color: transparent;
            color: #4A5568;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 8px 16px;
        }
        #backButton:hover {
            background-color: #E2E8F0;
        }
        #patientIcon {
            background-color: #3182CE;
            border-radius: 32px;
            /* In real app, use: border-image: url(:/icons/user_avatar.png); */
        }
        #patientName {
            font-size: 24px;
            font-weight: bold;
            color: #1A202C;
        }
        #patientId {
            font-size: 14px;
            color: #718096;
        }
        #sectionBox {
            background-color: white;
            border: 1px solid #E2E8F0;
            border-radius: 12px;
            padding: 25px;
        }
        #sectionTitle {
            font-size: 18px;
            font-weight: bold;
            color: #2D3748;
            margin-bottom: 15px;
        }
        QFormLayout QLabel {
            font-size: 14px;
            color: #4A5568;
        }
        QLineEdit, QComboBox, QDateEdit {
            border: 1px solid #CBD5E0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            background-color: #FFFFFF;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus {
            border-color: #3182CE;
            box-shadow: 0 0 0 2px rgba(49, 130, 206, 0.2);
        }
        .readOnlyField {
            background-color: #F7FAFC;
            color: #718096;
        }
        #fieldHint {
            font-size: 12px;
            color: #E53E3E;
        }
        #cancelButton {
            background-color: #F7FAFC;
            color: #4A5568;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 10px 24px;
            font-size: 15px;
        }
        #cancelButton:hover {
            background-color: #EDF2F7;
        }
        #saveButton {
            background-color: #3182CE;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 24px;
            font-size: 15px;
            font-weight: bold;
        }
        #saveButton:hover {
            background-color: #2B6CB0;
        }
    )";
    this->setStyleSheet(qss);
}