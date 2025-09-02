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
#include <QPainter>
#include <QPixmap>

static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2B6CB0" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* briefcaseIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2B6CB0" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="20" height="14" rx="2" ry="2"></rect><path d="M16 21V5a2 2 0 0 0-2-2h-4a2 2 0 0 0-2 2v16"></path></svg>)";
static const char* phoneIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#2B6CB0" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>)";
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";

DoctorProfileWidget::DoctorProfileWidget(QWidget *parent) : QWidget(parent)
{
    // 默认占位数据（服务器还未返回时避免空白）
    currentData = {
        "","", "男", "1980-01-01", "", "O型",
        "主治医师", "心血管内科", "", "", "", "", ""
    };

    initUI();
    applyStyles();
    loadDataToUI();
}

void DoctorProfileWidget::initUI() {
    setObjectName("profileWidget");
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *container = new QWidget();
    container->setObjectName("container");
    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(30, 20, 30, 30);
    containerLayout->setSpacing(20);

    containerLayout->addWidget(createHeader());
    containerLayout->addWidget(createProfileCard());

    mainStack = new QStackedWidget();
    mainStack->addWidget(createViewPage());
    mainStack->addWidget(createEditPage());
    containerLayout->addWidget(mainStack);

    mainLayout->addWidget(container);
}

QWidget* DoctorProfileWidget::createHeader() {
    QWidget *header = new QWidget();
    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton *backButton = new QPushButton(" 返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &DoctorProfileWidget::backRequested);

    QSvgRenderer renderer{QByteArray(backIconSvg)};
    QPixmap pixmap(18, 18); pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap); renderer.render(&painter);
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
    auto *layout = new QHBoxLayout(card);

    // 左侧头像（省略，可按需添加）

    // 右侧文字
    auto *infoLayout = new QVBoxLayout();
    cardNameLabel = new QLabel("—");
    cardNameLabel->setObjectName("profileName");
    cardDetailsLabel = new QLabel("—");
    cardDetailsLabel->setObjectName("profileDetails");
    cardIdLabel = new QLabel("工号: —");
    cardIdLabel->setObjectName("profileId");

    infoLayout->addWidget(cardNameLabel);
    infoLayout->addWidget(cardDetailsLabel);
    infoLayout->addWidget(cardIdLabel);
    layout->addLayout(infoLayout);
    return card;
}

QWidget* DoctorProfileWidget::createViewPage() {
    QWidget *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setSpacing(25);

    // Basic
    auto *basicInfoLayout = new QGridLayout(); basicInfoLayout->setSpacing(20);
    viewNameLabel = new QLabel(); viewGenderLabel = new QLabel();
    viewBirthDateLabel = new QLabel(); viewAgeLabel = new QLabel();
    viewIdCardLabel = new QLabel(); viewBloodTypeLabel = new QLabel();
    basicInfoLayout->addWidget(new QLabel("姓名"), 0, 0); basicInfoLayout->addWidget(viewNameLabel, 0, 1);
    basicInfoLayout->addWidget(new QLabel("性别"), 0, 2); basicInfoLayout->addWidget(viewGenderLabel, 0, 3);
    basicInfoLayout->addWidget(new QLabel("出生日期"), 1, 0); basicInfoLayout->addWidget(viewBirthDateLabel, 1, 1);
    basicInfoLayout->addWidget(new QLabel("年龄"), 1, 2); basicInfoLayout->addWidget(viewAgeLabel, 1, 3);
    basicInfoLayout->addWidget(new QLabel("身份证号"), 2, 0); basicInfoLayout->addWidget(viewIdCardLabel, 2, 1);
    basicInfoLayout->addWidget(new QLabel("血型"), 2, 2); basicInfoLayout->addWidget(viewBloodTypeLabel, 2, 3);
    QWidget *basicInfoContent = new QWidget(); basicInfoContent->setLayout(basicInfoLayout);
    layout->addWidget(createSection(userIconSvg, "基本信息", basicInfoContent));

    // Professional
    auto *profInfoLayout = new QGridLayout(); profInfoLayout->setSpacing(20);
    viewIdLabel = new QLabel(); viewTitleLabel = new QLabel();
    viewDepartmentLabel = new QLabel(); viewSpecialtyLabel = new QLabel();
    profInfoLayout->addWidget(new QLabel("工号"), 0, 0); profInfoLayout->addWidget(viewIdLabel, 0, 1);
    profInfoLayout->addWidget(new QLabel("职称"), 0, 2); profInfoLayout->addWidget(viewTitleLabel, 0, 3);
    profInfoLayout->addWidget(new QLabel("科室"), 1, 0); profInfoLayout->addWidget(viewDepartmentLabel, 1, 1);
    profInfoLayout->addWidget(new QLabel("专业方向"), 1, 2); profInfoLayout->addWidget(viewSpecialtyLabel, 1, 3);
    QWidget *profInfoContent = new QWidget(); profInfoContent->setLayout(profInfoLayout);
    layout->addWidget(createSection(briefcaseIconSvg, "职业信息", profInfoContent));

    // Contact
    auto *contactInfoLayout = new QGridLayout(); contactInfoLayout->setSpacing(20);
    viewOfficePhoneLabel = new QLabel(); viewMobilePhoneLabel = new QLabel();
    viewEmailLabel = new QLabel(); viewOfficeLocationLabel = new QLabel();
    contactInfoLayout->addWidget(new QLabel("办公电话"), 0, 0); contactInfoLayout->addWidget(viewOfficePhoneLabel, 0, 1);
    contactInfoLayout->addWidget(new QLabel("手机号码"), 0, 2); contactInfoLayout->addWidget(viewMobilePhoneLabel, 0, 3);
    contactInfoLayout->addWidget(new QLabel("电子邮箱"), 1, 0); contactInfoLayout->addWidget(viewEmailLabel, 1, 1);
    contactInfoLayout->addWidget(new QLabel("办公室位置"), 1, 2); contactInfoLayout->addWidget(viewOfficeLocationLabel, 1, 3);
    QWidget *contactInfoContent = new QWidget(); contactInfoContent->setLayout(contactInfoLayout);
    layout->addWidget(createSection(phoneIconSvg, "联系方式", contactInfoContent));

    // Action
    auto *editButton = new QPushButton("编辑信息");
    editButton->setObjectName("primaryButton");
    connect(editButton, &QPushButton::clicked, this, &DoctorProfileWidget::switchToEditMode);
    auto *btnLayout = new QHBoxLayout(); btnLayout->addStretch(); btnLayout->addWidget(editButton);
    layout->addLayout(btnLayout);

    return page;
}

QWidget* DoctorProfileWidget::createEditPage() {
    QWidget *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setSpacing(25);

    // Basic
    auto *basicForm = new QFormLayout(); basicForm->setSpacing(15);
    editNameLineEdit = new QLineEdit();
    editGenderComboBox = new QComboBox(); editGenderComboBox->addItems({"男","女"});
    editBirthDateEdit = new QDateEdit(); editBirthDateEdit->setCalendarPopup(true); editBirthDateEdit->setDisplayFormat("yyyy/MM/dd");
    editIdCardLineEdit = new QLineEdit();
    editBloodTypeComboBox = new QComboBox(); editBloodTypeComboBox->addItems({"A型","B型","AB型","O型","其他"});
    basicForm->addRow("姓名", editNameLineEdit);
    basicForm->addRow("性别", editGenderComboBox);
    basicForm->addRow("出生日期", editBirthDateEdit);
    basicForm->addRow("身份证号", editIdCardLineEdit);
    basicForm->addRow("血型", editBloodTypeComboBox);
    QWidget *basicFormContent = new QWidget(); basicFormContent->setLayout(basicForm);
    layout->addWidget(createSection(userIconSvg, "基本信息", basicFormContent));

    // Professional
    auto *profForm = new QFormLayout(); profForm->setSpacing(15);
    editIdLineEdit = new QLineEdit(); editIdLineEdit->setReadOnly(true);
    editTitleComboBox = new QComboBox(); editTitleComboBox->addItems({"住院医师","主治医师","副主任医师","主任医师"});
    editDepartmentComboBox = new QComboBox(); editDepartmentComboBox->addItems({"心血管内科","神经内科","呼吸内科","普外科"});
    editSpecialtyLineEdit = new QLineEdit();
    profForm->addRow("工号", editIdLineEdit);
    profForm->addRow("职称", editTitleComboBox);
    profForm->addRow("科室", editDepartmentComboBox);
    profForm->addRow("专业方向", editSpecialtyLineEdit);
    QWidget *profFormContent = new QWidget(); profFormContent->setLayout(profForm);
    layout->addWidget(createSection(briefcaseIconSvg, "职业信息", profFormContent));

    // Contact
    auto *contactForm = new QFormLayout(); contactForm->setSpacing(15);
    editOfficePhoneLineEdit = new QLineEdit();
    editMobilePhoneLineEdit = new QLineEdit();
    editEmailLineEdit = new QLineEdit(); editEmailLineEdit->setReadOnly(true);
    editOfficeLocationLineEdit = new QLineEdit();
    contactForm->addRow("办公电话", editOfficePhoneLineEdit);
    contactForm->addRow("手机号码", editMobilePhoneLineEdit);
    contactForm->addRow("电子邮箱", editEmailLineEdit);
    contactForm->addRow("办公室位置", editOfficeLocationLineEdit);
    QWidget *contactFormContent = new QWidget(); contactFormContent->setLayout(contactForm);
    layout->addWidget(createSection(phoneIconSvg, "联系方式", contactFormContent));

    // Actions
    auto *cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("secondaryButton");
    connect(cancelButton, &QPushButton::clicked, this, &DoctorProfileWidget::switchToViewMode);
    auto *saveButton = new QPushButton("保存更改");
    saveButton->setObjectName("primaryButton");
    connect(saveButton, &QPushButton::clicked, this, &DoctorProfileWidget::saveChanges);
    auto *btnLayout = new QHBoxLayout(); btnLayout->addStretch(); btnLayout->addWidget(cancelButton); btnLayout->addWidget(saveButton);
    layout->addLayout(btnLayout);

    return page;
}

QWidget* DoctorProfileWidget::createSection(const QString &svgIcon, const QString &title, QWidget *content) {
    QWidget *section = new QWidget();
    auto *layout = new QVBoxLayout(section); layout->setSpacing(15);
    auto *titleLayout = new QHBoxLayout();
    auto *icon = new QSvgWidget(); icon->load(QByteArray(svgIcon.toUtf8())); icon->setFixedSize(20, 20);
    auto *titleLabel = new QLabel(title); titleLabel->setObjectName("sectionTitle");
    titleLayout->addWidget(icon); titleLayout->addWidget(titleLabel); titleLayout->addStretch();

    auto *line = new QFrame(); line->setFrameShape(QFrame::HLine); line->setFrameShadow(QFrame::Sunken); line->setObjectName("separator");
    layout->addLayout(titleLayout);
    layout->addWidget(line);
    layout->addWidget(content);
    return section;
}

void DoctorProfileWidget::switchToEditMode() {
    loadDataToUI();           // 用最新数据喂表单
    mainStack->setCurrentIndex(1);
}
void DoctorProfileWidget::switchToViewMode() {
    mainStack->setCurrentIndex(0);
}

// 从 UI 生成“仅改动字段”的 patch，并自动携带 doctor_id
QJsonObject DoctorProfileWidget::buildPatchFromUI() const
{
    QJsonObject patch;
    auto addIfChanged = [&](const QString &key, const QString &oldVal, const QString &newVal){
        if (newVal.trimmed() != oldVal.trimmed()) patch.insert(key, newVal.trimmed());
    };

    // 新值（不落盘 currentData，只做比较）
    const QString name        = editNameLineEdit->text();
    const QString gender      = editGenderComboBox->currentText();
    const QString birthDate   = editBirthDateEdit->date().toString("yyyy-MM-dd");
    const QString idCard      = editIdCardLineEdit->text();
    const QString bloodType   = editBloodTypeComboBox->currentText();
    const QString title       = editTitleComboBox->currentText();
    const QString department  = editDepartmentComboBox->currentText();
    const QString specialty   = editSpecialtyLineEdit->text();
    const QString officePhone = editOfficePhoneLineEdit->text();
    const QString mobilePhone = editMobilePhoneLineEdit->text();
    const QString officeLoc   = editOfficeLocationLineEdit->text();

    addIfChanged("name",           currentData.name,           name);
    addIfChanged("gender",         currentData.gender,         gender);
    addIfChanged("birth_date",     currentData.birthDate,      birthDate);
    addIfChanged("id_card",        currentData.idCard,         idCard);
    addIfChanged("blood_type",     currentData.bloodType,      bloodType);
    addIfChanged("title",          currentData.title,          title);
    addIfChanged("department",     currentData.department,     department);
    addIfChanged("specialty",      currentData.specialty,      specialty);
    addIfChanged("office_phone",   currentData.officePhone,    officePhone);
    addIfChanged("mobile_phone",   currentData.mobilePhone,    mobilePhone);
    addIfChanged("office_location",currentData.officeLocation, officeLoc);

    if (m_doctorId > 0) patch.insert("doctor_id", m_doctorId);
    return patch;
}

void DoctorProfileWidget::saveChanges() {
    const QJsonObject patch = buildPatchFromUI();
    if (patch.size() == 0) {
        QMessageBox::information(this, "提示", "没有改动，无需保存。");
        return;
    }
    // 交给 Main → Widget::sendUpdateDoctorProfile()
    emit saveRequested(patch);
}

void DoctorProfileWidget::setDoctorProfile(const QJsonObject &d)
{
    // 兜底读取：同时兼容 snake_case / camelCase / 可能别名
    auto S = [&](const char *a, const char *b=nullptr, const char *c=nullptr){
        if (d.contains(a)) return d.value(a).toString();
        if (b && d.contains(b)) return d.value(b).toString();
        if (c && d.contains(c)) return d.value(c).toString();
        return QString{};
    };
    auto I = [&](const char *a){ return d.value(a).toInt(); };

    m_doctorId          = d.contains("doctor_id") ? d.value("doctor_id").toInt() : m_doctorId;
    currentData.id      = S("employee_id","job_id","id");              // 只读展示
    currentData.name    = S("name","real_name");
    currentData.gender  = S("gender");
    currentData.birthDate = S("birth_date","birthday","dob");
    currentData.idCard  = S("id_card","idNumber","id_no");
    currentData.bloodType = S("blood_type","bloodType");
    currentData.title   = S("title","professional_title");
    currentData.department = S("department","dept","dept_name");
    currentData.specialty  = S("specialty","expertise");
    currentData.officePhone= S("office_phone","work_phone","telephone");
    currentData.mobilePhone= S("mobile_phone","mobile","phone");
    currentData.email      = S("email","mail");                        // 只读展示
    currentData.officeLocation = S("office_location","office","location");

    // 如果年龄未知，后面用生日计算
    loadDataToUI();
    switchToViewMode();
}

void DoctorProfileWidget::showError(const QString &err)
{
    QMessageBox::warning(this, "加载失败", err.isEmpty() ? "无法加载医生资料" : err);
}

void DoctorProfileWidget::onSaveResult(bool ok, const QString &message)
{
    if (ok) {
        QMessageBox::information(this, "保存成功", message.isEmpty() ? "信息已保存" : message);
        // 提示：Main 会在收到 ok 后重新拉最新资料，这里只切回浏览页
        switchToViewMode();
    } else {
        QMessageBox::warning(this, "保存失败", message.isEmpty() ? "请稍后重试" : message);
    }
}

void DoctorProfileWidget::loadDataToUI() {
    // 顶部卡片
    if (cardNameLabel)   cardNameLabel->setText(currentData.name.isEmpty() ? "—" : currentData.name + " 医生");
    if (cardDetailsLabel)cardDetailsLabel->setText(
        (currentData.title.isEmpty() ? "—" : currentData.title) +
        QString(" | ") +
        (currentData.department.isEmpty() ? "—" : currentData.department));
    if (cardIdLabel)     cardIdLabel->setText("工号: " + (currentData.id.isEmpty() ? "—" : currentData.id));

    // View
    if (viewNameLabel)         viewNameLabel->setText(currentData.name);
    if (viewGenderLabel)       viewGenderLabel->setText(currentData.gender);
    if (viewBirthDateLabel)    viewBirthDateLabel->setText(currentData.birthDate);
    if (viewAgeLabel) {
        int age = 0;
        const QDate d = QDate::fromString(currentData.birthDate, "yyyy-MM-dd");
        if (d.isValid()) {
            const QDate today = QDate::currentDate();
            age = today.year() - d.year() - ((today.month() < d.month() || (today.month()==d.month() && today.day()<d.day())) ? 1 : 0);
        }
        viewAgeLabel->setText(QString::number(age));
    }
    if (viewIdCardLabel)       viewIdCardLabel->setText(currentData.idCard);
    if (viewBloodTypeLabel)    viewBloodTypeLabel->setText(currentData.bloodType);
    if (viewIdLabel)           viewIdLabel->setText(currentData.id);
    if (viewTitleLabel)        viewTitleLabel->setText(currentData.title);
    if (viewDepartmentLabel)   viewDepartmentLabel->setText(currentData.department);
    if (viewSpecialtyLabel)    viewSpecialtyLabel->setText(currentData.specialty);
    if (viewOfficePhoneLabel)  viewOfficePhoneLabel->setText(currentData.officePhone);
    if (viewMobilePhoneLabel)  viewMobilePhoneLabel->setText(currentData.mobilePhone);
    if (viewEmailLabel)        viewEmailLabel->setText(currentData.email);
    if (viewOfficeLocationLabel)viewOfficeLocationLabel->setText(currentData.officeLocation);

    // Edit
    if (editNameLineEdit)        editNameLineEdit->setText(currentData.name);
    if (editGenderComboBox)      editGenderComboBox->setCurrentText(currentData.gender.isEmpty()? "男" : currentData.gender);
    if (editBirthDateEdit)       editBirthDateEdit->setDate(QDate::fromString(currentData.birthDate, "yyyy-MM-dd"));
    if (editIdCardLineEdit)      editIdCardLineEdit->setText(currentData.idCard);
    if (editBloodTypeComboBox)   editBloodTypeComboBox->setCurrentText(currentData.bloodType.isEmpty()? "O型" : currentData.bloodType);
    if (editIdLineEdit)          editIdLineEdit->setText(currentData.id);                // 只读
    if (editTitleComboBox)       editTitleComboBox->setCurrentText(currentData.title);
    if (editDepartmentComboBox)  editDepartmentComboBox->setCurrentText(currentData.department);
    if (editSpecialtyLineEdit)   editSpecialtyLineEdit->setText(currentData.specialty);
    if (editOfficePhoneLineEdit) editOfficePhoneLineEdit->setText(currentData.officePhone);
    if (editMobilePhoneLineEdit) editMobilePhoneLineEdit->setText(currentData.mobilePhone);
    if (editEmailLineEdit)       editEmailLineEdit->setText(currentData.email);          // 只读
    if (editOfficeLocationLineEdit) editOfficeLocationLineEdit->setText(currentData.officeLocation);
}

void DoctorProfileWidget::applyStyles() {
    setStyleSheet(R"(
        #profileWidget { background-color: #EBF8FF; }
        #container { background-color: white; border-radius: 15px; }
        #backButton { border: none; color: #4A5568; font-size: 14px; font-weight: bold; }
        #pageTitle { font-size: 20px; font-weight: bold; color: #2D3748; }
        #profileCard { background-color: #F7FAFC; border-radius: 10px; padding: 20px; }
        #profileName { font-size: 18px; font-weight: bold; }
        #profileDetails, #profileId { font-size: 14px; color: #718096; }
        #sectionTitle { font-size: 16px; font-weight: bold; color: #2D3748; }
        #separator { background-color: #EDF2F7; }
        QLabel { font-size: 14px; color: #4A5568; }
        QLineEdit, QComboBox, QDateEdit {
            border: 1px solid #E2E8F0; border-radius: 6px; padding: 8px; font-size: 14px;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border-color: #3182CE; }
        QLineEdit[readOnly="true"] { background-color: #F7FAFC; color: #718096; }
        QComboBox::drop-down { border: none; }
        QPushButton { border-radius: 8px; padding: 10px 20px; font-size: 14px; font-weight: bold; }
        #primaryButton { background-color: #3182CE; color: white; }
        #primaryButton:hover { background-color: #2B6CB0; }
        #secondaryButton { background-color: #E2E8F0; color: #2D3748; }
        #secondaryButton:hover { background-color: #CBD5E0; }
    )");
}
