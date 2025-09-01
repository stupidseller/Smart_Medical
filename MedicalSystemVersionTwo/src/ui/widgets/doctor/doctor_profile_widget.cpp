#include "doctor_profile_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QSvgWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QPainter>   // <-- 确保有这一行
#include <QPixmap>
#include "attendance_widget.h"
// --- SVG Icons ---
static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2B6CB0" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* briefcaseIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2B6CB0" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="20" height="14" rx="2" ry="2"></rect><path d="M16 21V5a2 2 0 0 0-2-2h-4a2 2 0 0 0-2 2v16"></path></svg>)";
static const char* phoneIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2B6CB0" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>)";
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";


DoctorProfileWidget::DoctorProfileWidget(QWidget *parent) : QWidget(parent)
{
    // 模拟加载初始数据
    currentData = {
            "D2021005", "王明", "男", "1980-03-12", "31010519800312****", "O型",
            "主任医师", "心血管内科", "冠心病介入治疗", "021-12345678",
            "139****1234", "wangming@hospital.com", "门诊大楼3楼A区305室"
    };

    initUI();
    applyStyles();
    loadDataToUI(); // 加载初始数据到UI
}

void DoctorProfileWidget::initUI() {
    this->setObjectName("profileWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 无边距，让背景色填满
    mainLayout->setSpacing(0);

    // 内部容器，用于设置边距和圆角
    QWidget *container = new QWidget();
    container->setObjectName("container");
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(30, 20, 30, 30);
    containerLayout->setSpacing(20);

    // 1. Header
    containerLayout->addWidget(createHeader());

    // 2. Profile Card
    containerLayout->addWidget(createProfileCard());

    // 3. Stacked Widget for View/Edit
    mainStack = new QStackedWidget();
    mainStack->addWidget(createViewPage());
    mainStack->addWidget(createEditPage());
    containerLayout->addWidget(mainStack);

    mainLayout->addWidget(container);
}

// 替换 doctor_profile_widget.cpp 中旧的同名函数
QWidget* DoctorProfileWidget::createHeader() {
    QWidget *header = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton *backButton = new QPushButton(" 返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &DoctorProfileWidget::backRequested);

    // --- 关键代码修改 ---
    // 这是旧的、可能产生歧义的写法：
    // QSvgRenderer renderer(QByteArray(backIconSvg));

    // 这是新的、无歧义的 C++11 写法，将圆括号 () 改为大括号 {}
    QSvgRenderer renderer{QByteArray(backIconSvg)};

    // --- 后续代码保持不变 ---
    QPixmap pixmap(18, 18);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    renderer.render(&painter);

    backButton->setIcon(QIcon(pixmap));

    QLabel *title = new QLabel("医生信息管理");
    title->setObjectName("pageTitle");

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();

    return header;
}


QWidget* DoctorProfileWidget::createProfileCard() {
    QWidget *card = new QWidget();
    card->setObjectName("profileCard");
    QHBoxLayout *layout = new QHBoxLayout(card);

    // ... Avatar ...

    QVBoxLayout *infoLayout = new QVBoxLayout();
    QLabel* nameLabel = new QLabel("王明医生");
    nameLabel->setObjectName("profileName");
    QLabel* detailsLabel = new QLabel("主任医师 | 心血管内科");
    detailsLabel->setObjectName("profileDetails");
    QLabel* idLabel = new QLabel("工号: D2021005");
    idLabel->setObjectName("profileId");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(detailsLabel);
    infoLayout->addWidget(idLabel);
    layout->addLayout(infoLayout);

    return card;
}

QWidget* DoctorProfileWidget::createViewPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setSpacing(25);

    // --- Basic Info ---
    QGridLayout* basicInfoLayout = new QGridLayout();
    basicInfoLayout->setSpacing(20);
    viewNameLabel = new QLabel();
    viewGenderLabel = new QLabel();
    viewBirthDateLabel = new QLabel();
    viewAgeLabel = new QLabel();
    viewIdCardLabel = new QLabel();
    viewBloodTypeLabel = new QLabel();
    basicInfoLayout->addWidget(new QLabel("姓名"), 0, 0); basicInfoLayout->addWidget(viewNameLabel, 0, 1);
    basicInfoLayout->addWidget(new QLabel("性别"), 0, 2); basicInfoLayout->addWidget(viewGenderLabel, 0, 3);
    basicInfoLayout->addWidget(new QLabel("出生日期"), 1, 0); basicInfoLayout->addWidget(viewBirthDateLabel, 1, 1);
    basicInfoLayout->addWidget(new QLabel("年龄"), 1, 2); basicInfoLayout->addWidget(viewAgeLabel, 1, 3);
    basicInfoLayout->addWidget(new QLabel("身份证号"), 2, 0); basicInfoLayout->addWidget(viewIdCardLabel, 2, 1);
    basicInfoLayout->addWidget(new QLabel("血型"), 2, 2); basicInfoLayout->addWidget(viewBloodTypeLabel, 2, 3);
    QWidget *basicInfoContent = new QWidget();
    basicInfoContent->setLayout(basicInfoLayout);
    layout->addWidget(createSection(userIconSvg, "基本信息", basicInfoContent));

    // --- Professional Info ---
    QGridLayout* profInfoLayout = new QGridLayout();
    profInfoLayout->setSpacing(20);
    viewIdLabel = new QLabel();
    viewTitleLabel = new QLabel();
    viewDepartmentLabel = new QLabel();
    viewSpecialtyLabel = new QLabel();
    profInfoLayout->addWidget(new QLabel("工号"), 0, 0); profInfoLayout->addWidget(viewIdLabel, 0, 1);
    profInfoLayout->addWidget(new QLabel("职称"), 0, 2); profInfoLayout->addWidget(viewTitleLabel, 0, 3);
    profInfoLayout->addWidget(new QLabel("科室"), 1, 0); profInfoLayout->addWidget(viewDepartmentLabel, 1, 1);
    profInfoLayout->addWidget(new QLabel("专业方向"), 1, 2); profInfoLayout->addWidget(viewSpecialtyLabel, 1, 3);
    QWidget *profInfoContent = new QWidget();
    profInfoContent->setLayout(profInfoLayout);
    layout->addWidget(createSection(briefcaseIconSvg, "职业信息", profInfoContent));

    // --- Contact Info ---
    QGridLayout* contactInfoLayout = new QGridLayout();
    contactInfoLayout->setSpacing(20);
    viewOfficePhoneLabel = new QLabel();
    viewMobilePhoneLabel = new QLabel();
    viewEmailLabel = new QLabel();
    viewOfficeLocationLabel = new QLabel();
    contactInfoLayout->addWidget(new QLabel("办公电话"), 0, 0); contactInfoLayout->addWidget(viewOfficePhoneLabel, 0, 1);
    contactInfoLayout->addWidget(new QLabel("手机号码"), 0, 2); contactInfoLayout->addWidget(viewMobilePhoneLabel, 0, 3);
    contactInfoLayout->addWidget(new QLabel("电子邮箱"), 1, 0); contactInfoLayout->addWidget(viewEmailLabel, 1, 1);
    contactInfoLayout->addWidget(new QLabel("办公室位置"), 1, 2); contactInfoLayout->addWidget(viewOfficeLocationLabel, 1, 3);
    QWidget *contactInfoContent = new QWidget();
    contactInfoContent->setLayout(contactInfoLayout);
    layout->addWidget(createSection(phoneIconSvg, "联系方式", contactInfoContent));

    // --- Action Button ---
    QPushButton *editButton = new QPushButton("编辑信息");
    editButton->setObjectName("primaryButton");
    connect(editButton, &QPushButton::clicked, this, &DoctorProfileWidget::switchToEditMode);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(editButton);
    layout->addLayout(btnLayout);

    return page;
}

QWidget* DoctorProfileWidget::createEditPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setSpacing(25);

    // --- Basic Info ---
    QFormLayout *basicForm = new QFormLayout();
    basicForm->setSpacing(15);
    editNameLineEdit = new QLineEdit();
    editGenderComboBox = new QComboBox(); editGenderComboBox->addItems({"男", "女"});
    editBirthDateEdit = new QDateEdit(); editBirthDateEdit->setCalendarPopup(true); editBirthDateEdit->setDisplayFormat("yyyy/MM/dd");
    editIdCardLineEdit = new QLineEdit();
    editBloodTypeComboBox = new QComboBox(); editBloodTypeComboBox->addItems({"A型", "B型", "AB型", "O型", "其他"});
    basicForm->addRow("姓名", editNameLineEdit);
    basicForm->addRow("性别", editGenderComboBox);
    basicForm->addRow("出生日期", editBirthDateEdit);
    basicForm->addRow("身份证号", editIdCardLineEdit);
    basicForm->addRow("血型", editBloodTypeComboBox);
    QWidget *basicFormContent = new QWidget();
    basicFormContent->setLayout(basicForm);
    layout->addWidget(createSection(userIconSvg, "基本信息", basicFormContent));

    // --- Professional Info ---
    QFormLayout *profForm = new QFormLayout();
    profForm->setSpacing(15);
    editIdLineEdit = new QLineEdit();
    editIdLineEdit->setReadOnly(true); // *** 工号不可修改 ***
    editTitleComboBox = new QComboBox(); editTitleComboBox->addItems({"住院医师", "主治医师", "副主任医师", "主任医师"});
    editDepartmentComboBox = new QComboBox(); editDepartmentComboBox->addItems({"心血管内科", "神经内科", "呼吸内科", "普外科"});
    editSpecialtyLineEdit = new QLineEdit();
    profForm->addRow("工号", editIdLineEdit);
    profForm->addRow("职称", editTitleComboBox);
    profForm->addRow("科室", editDepartmentComboBox);
    profForm->addRow("专业方向", editSpecialtyLineEdit);
    QWidget *profFormContent = new QWidget();
    profFormContent->setLayout(profForm);
    layout->addWidget(createSection(briefcaseIconSvg, "职业信息", profFormContent));

    // --- Contact Info ---
    QFormLayout *contactForm = new QFormLayout();
    contactForm->setSpacing(15);
    editOfficePhoneLineEdit = new QLineEdit();
    editMobilePhoneLineEdit = new QLineEdit();
    editEmailLineEdit = new QLineEdit();
    editEmailLineEdit->setReadOnly(true); // *** 电子邮箱不可修改 ***
    editOfficeLocationLineEdit = new QLineEdit();
    contactForm->addRow("办公电话", editOfficePhoneLineEdit);
    contactForm->addRow("手机号码", editMobilePhoneLineEdit);
    contactForm->addRow("电子邮箱", editEmailLineEdit);
    contactForm->addRow("办公室位置", editOfficeLocationLineEdit);
    QWidget *contactFormContent = new QWidget();
    contactFormContent->setLayout(contactForm);
    layout->addWidget(createSection(phoneIconSvg, "联系方式", contactFormContent));

    // --- Action Buttons ---
    QPushButton *cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("secondaryButton");
    connect(cancelButton, &QPushButton::clicked, this, &DoctorProfileWidget::switchToViewMode);

    QPushButton *saveButton = new QPushButton("保存更改");
    saveButton->setObjectName("primaryButton");
    connect(saveButton, &QPushButton::clicked, this, &DoctorProfileWidget::saveChanges);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(saveButton);
    layout->addLayout(btnLayout);

    return page;
}

QWidget* DoctorProfileWidget::createSection(const QString &svgIcon, const QString &title, QWidget *content) {
    QWidget *section = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setSpacing(15);

    // Title
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QSvgWidget *icon = new QSvgWidget();
    icon->load(QByteArray(svgIcon.toUtf8()));
    icon->setFixedSize(20, 20);
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("sectionTitle");
    titleLayout->addWidget(icon);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    // Separator
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("separator");

    layout->addLayout(titleLayout);
    layout->addWidget(line);
    layout->addWidget(content);

    return section;
}

void DoctorProfileWidget::switchToEditMode() {
    loadDataToUI(); // 确保编辑页面显示的是最新数据
    mainStack->setCurrentIndex(1);
}

void DoctorProfileWidget::switchToViewMode() {
    mainStack->setCurrentIndex(0);
}

void DoctorProfileWidget::saveChanges() {
    collectDataFromUI();
    loadDataToUI(); // 更新显示页面
    switchToViewMode();
    QMessageBox::information(this, "成功", "信息已保存。");
}

void DoctorProfileWidget::loadDataToUI() {
    // --- Load to View Page ---
    viewNameLabel->setText(currentData.name);
    viewGenderLabel->setText(currentData.gender);
    viewBirthDateLabel->setText(currentData.birthDate);
    // 计算年龄
    int age = QDate::currentDate().year() - QDate::fromString(currentData.birthDate, "yyyy-MM-dd").year();
    viewAgeLabel->setText(QString::number(age));
    viewIdCardLabel->setText(currentData.idCard);
    viewBloodTypeLabel->setText(currentData.bloodType);
    viewIdLabel->setText(currentData.id);
    viewTitleLabel->setText(currentData.title);
    viewDepartmentLabel->setText(currentData.department);
    viewSpecialtyLabel->setText(currentData.specialty);
    viewOfficePhoneLabel->setText(currentData.officePhone);
    viewMobilePhoneLabel->setText(currentData.mobilePhone);
    viewEmailLabel->setText(currentData.email);
    viewOfficeLocationLabel->setText(currentData.officeLocation);

    // --- Load to Edit Page ---
    editNameLineEdit->setText(currentData.name);
    editGenderComboBox->setCurrentText(currentData.gender);
    editBirthDateEdit->setDate(QDate::fromString(currentData.birthDate, "yyyy-MM-dd"));
    editIdCardLineEdit->setText(currentData.idCard);
    editBloodTypeComboBox->setCurrentText(currentData.bloodType);
    editIdLineEdit->setText(currentData.id);
    editTitleComboBox->setCurrentText(currentData.title);
    editDepartmentComboBox->setCurrentText(currentData.department);
    editSpecialtyLineEdit->setText(currentData.specialty);
    editOfficePhoneLineEdit->setText(currentData.officePhone);
    editMobilePhoneLineEdit->setText(currentData.mobilePhone);
    editEmailLineEdit->setText(currentData.email);
    editOfficeLocationLineEdit->setText(currentData.officeLocation);
}

void DoctorProfileWidget::collectDataFromUI() {
    currentData.name = editNameLineEdit->text();
    currentData.gender = editGenderComboBox->currentText();
    currentData.birthDate = editBirthDateEdit->date().toString("yyyy-MM-dd");
    currentData.idCard = editIdCardLineEdit->text();
    currentData.bloodType = editBloodTypeComboBox->currentText();
    // id and email are read-only, no need to collect
    currentData.title = editTitleComboBox->currentText();
    currentData.department = editDepartmentComboBox->currentText();
    currentData.specialty = editSpecialtyLineEdit->text();
    currentData.officePhone = editOfficePhoneLineEdit->text();
    currentData.mobilePhone = editMobilePhoneLineEdit->text();
    currentData.officeLocation = editOfficeLocationLineEdit->text();
}


void DoctorProfileWidget::applyStyles() {
    this->setStyleSheet(R"(
        #profileWidget { background-color: #EBF8FF; }
        #container {
            background-color: white;
            border-radius: 15px;
        }
        #backButton {
            border: none;
            color: #4A5568;
            font-size: 14px;
            font-weight: bold;
        }
        #pageTitle {
            font-size: 20px;
            font-weight: bold;
            color: #2D3748;
        }
        #profileCard {
            background-color: #F7FAFC;
            border-radius: 10px;
            padding: 20px;
        }
        #profileName { font-size: 18px; font-weight: bold; }
        #profileDetails, #profileId { font-size: 14px; color: #718096; }

        #sectionTitle {
            font-size: 16px;
            font-weight: bold;
            color: #2D3748;
        }
        #separator { background-color: #EDF2F7; }

        /* --- View Mode Labels --- */
        QLabel { font-size: 14px; color: #4A5568; }

        /* --- Edit Mode Inputs --- */
        QLineEdit, QComboBox, QDateEdit {
            border: 1px solid #E2E8F0;
            border-radius: 6px;
            padding: 8px;
            font-size: 14px;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border-color: #3182CE; }
        QLineEdit[readOnly="true"] { background-color: #F7FAFC; color: #718096; }
        QComboBox::drop-down { border: none; }

        /* --- Buttons --- */
        QPushButton {
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        #primaryButton {
            background-color: #3182CE;
            color: white;
        }
        #primaryButton:hover { background-color: #2B6CB0; }
        #secondaryButton {
            background-color: #E2E8F0;
            color: #2D3748;
        }
        #secondaryButton:hover { background-color: #CBD5E0; }
    )");
}