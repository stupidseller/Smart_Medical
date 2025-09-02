#include "profile_widget.h"
#include "widget.h"
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
#include <QMessageBox> // 用于测试保存功能
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>   // 如果后面有 toVariant() 之类操作就留着
#include <QDebug>
ProfileWidget::ProfileWidget(Widget *api, int patientId, QWidget *parent)
    : QWidget(parent), m_api(api), m_patientId(patientId)
{
    setObjectName("ProfileWidget");
    initUI();
    initStyleSheets();

    // 绑定信号
    connect(m_api, &Widget::patientProfileLoaded, this, &ProfileWidget::onProfileLoaded);
    connect(m_api, &Widget::patientProfileSaved,  this, &ProfileWidget::onProfileSaved);

    // 进入页面就拉取
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
    if (m_api && m_patientId > 0) {
        m_api->sendGetPatientProfile(m_patientId);
    }
}
void ProfileWidget::onProfileLoaded(const QJsonObject &p) {
    m_lastProfile = p;
    fillFormFromProfile(p);
}
void ProfileWidget::fillFormFromProfile(const QJsonObject &p) {
    // 姓名
    const QString name = p.value("name").toString();
    nameLineEdit->setText(name);
    if (headerNameLabel) headerNameLabel->setText(name.isEmpty() ? "—" : name);
    if (headerIdLabel)   headerIdLabel->setText(QString("ID: P%1").arg(m_patientId));

    // 性别映射
    auto setGender = [&](const QString &g){
        QString s = g.trimmed();
        s = s.toLower();
        int idx = 2; // “其他”
        if (s=="男" || s=="m" || s=="male" || s=="1") idx = 0;
        else if (s=="女" || s=="f" || s=="female" || s=="0") idx = 1;
        genderComboBox->setCurrentIndex(idx);
    };
    setGender(p.value("gender").toString());

    // 生日 + 年龄
    const QDate bd = QDate::fromString(p.value("birth_date").toString(), "yyyy-MM-dd");
    birthDateEdit->setDate(bd.isValid() ? bd : QDate::currentDate());
    updateAge();

    // 身份证
    idNumberLineEdit->setText(p.value("id_number").toString());

    // 血型（限定 A/B/AB/O/其他）
    const QString bt = p.value("blood_type").toString().toUpper();
    int idx = bloodTypeComboBox->findText(bt);
    bloodTypeComboBox->setCurrentIndex(idx < 0 ? bloodTypeComboBox->findText("其他") : idx);

    // 联系方式
    mobileLineEdit->setText(p.value("mobile").toString());
    emailLineEdit->setText(p.value("email").toString());
    emergencyContactLineEdit->setText(p.value("emergency_contact").toString());
    emergencyPhoneLineEdit->setText(p.value("emergency_phone").toString());

    // 医保/病史
    insuranceTypeComboBox->setCurrentText(p.value("insurance_type").toString());
    insuranceCardLineEdit->setText(p.value("insurance_card").toString());
    allergiesLineEdit->setText(p.value("allergies").toString());
    historyLineEdit->setText(p.value("history").toString());
}


QJsonObject ProfileWidget::buildPatchFromForm() const {
    QJsonObject patch;
    patch.insert("patient_id", m_patientId);

    // 只提交允许修改的字段（与服务器 allow 白名单一致）
    patch.insert("gender",            genderComboBox->currentText());
    patch.insert("birth_date",        birthDateEdit->date().toString("yyyy-MM-dd"));
    patch.insert("blood_type",        bloodTypeComboBox->currentText());
    patch.insert("mobile",            mobileLineEdit->text());
    patch.insert("emergency_contact", emergencyContactLineEdit->text());
    patch.insert("emergency_phone",   emergencyPhoneLineEdit->text());
    patch.insert("insurance_type",    insuranceTypeComboBox->currentText());
    patch.insert("allergies",         allergiesLineEdit->text());
    patch.insert("history",           historyLineEdit->text());

    return patch;
}



// --- UI 构建函数实现 ---

QWidget* ProfileWidget::createHeader() {
    QWidget *header = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0,0,0,0);

    QLabel* title = new QLabel("个人信息");
    title->setObjectName("pageTitle");

    QPushButton *backButton = new QPushButton("返回仪表盘");
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

    headerNameLabel = new QLabel("—");
    headerNameLabel->setObjectName("patientName");
    headerIdLabel = new QLabel(QString("ID: P%1").arg(m_patientId > 0 ? QString::number(m_patientId) : "—"));
    headerIdLabel->setObjectName("patientId");

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(5);
    textLayout->addWidget(headerNameLabel);
    textLayout->addWidget(headerIdLabel);

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
    genderComboBox->addItems({"男", "女", "其他"});

    birthDateEdit = new QDateEdit();
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("yyyy-MM-dd");
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
    idNumberLineEdit->setReadOnly(true);
    idNumberLineEdit->setProperty("class", "readOnlyField");

    bloodTypeComboBox = new QComboBox();
    bloodTypeComboBox->addItems({"A","B","AB","O","其他"});

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
    insuranceCardLineEdit->setReadOnly(true);
    insuranceCardLineEdit->setProperty("class", "readOnlyField");

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
void ProfileWidget::onProfileSaved(bool ok, const QString &msg) {
    if (ok) {
        QMessageBox::information(this, "成功", msg.isEmpty()? "已保存": msg);
        // 保存成功后可重新拉一次，确保 UI 与库一致
        loadUserProfileData();
    } else {
        QMessageBox::warning(this, "保存失败", msg.isEmpty()? "请稍后再试": msg);
    }
}
void ProfileWidget::onSaveChangesClicked() {
    // 简单校验（可按需加强）
    const QString mobile = mobileLineEdit->text().trimmed();
    if (!mobile.isEmpty() && mobile.size()!=11) {
        QMessageBox::warning(this, "提示", "请输入有效的手机号码（11位）。");
        return;
    }

    if (m_api) m_api->sendUpdatePatientProfile(buildPatchFromForm());
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
        #sectionBox QLabel {
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
