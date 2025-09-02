#include "patient_management_widget.h"
#include "patient_data_types.h"
#include "patient_list_item_widget.h" // 引入自定义控件
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QButtonGroup>
#include <QFrame>
#include <QSvgWidget>
#include <QDebug>
#include <QSpacerItem>

// SVG图标定义 (保持不变)
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";
static const char* searchIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="11" cy="11" r="8"></circle><line x1="21" y1="21" x2="16.65" y2="16.65"></line></svg>)";
static const char* patientAvatarSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="40" height="40" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";
static const char* patientAvatarLargeSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="60" height="60" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";
static const char* recordsIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#3B82F6" stroke="white" stroke-width="1.5"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14,2 14,8 20,8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg>)";
static const char* ordersIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#10B981" stroke="white" stroke-width="1.5"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"/><rect x="8" y="2" width="8" height="4" rx="1" ry="1"/></svg>)";

PatientManagementWidget::PatientManagementWidget(QWidget *parent) : QWidget(parent)
{
    // 初始化示例数据
    samplePatients << PatientData{0, "张三", "P202305001", 38, "心血管内科", true, true, true};
    samplePatients << PatientData{1, "李四", "P202305002", 45, "神经内科", false, true, false};
    samplePatients << PatientData{2, "王五", "P202305003", 62, "消化内科", true, false, true};
    samplePatients << PatientData{3, "刘六", "P202305004", 29, "呼吸内科", false, true, false};
    samplePatients << PatientData{4, "陈七", "P202305005", 52, "骨科", false, true, false};

    initUI();
    applyStyles();

    // 初始加载并显示完整患者列表
    updatePatientList();
}

void PatientManagementWidget::initUI() {
    this->setObjectName("patientManagementWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(16);

    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createSearchPanel());
    mainLayout->addWidget(createContentArea(), 1);
}

QWidget* PatientManagementWidget::createHeader() {
    QFrame *header = new QFrame();
    header->setObjectName("headerFrame");
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);

    QPushButton *backBtn = new QPushButton();
    backBtn->setObjectName("backButton");
    QSvgWidget *backIcon = new QSvgWidget();
    backIcon->load(QByteArray(backIconSvg));
    QHBoxLayout *backLayout = new QHBoxLayout(backBtn);
    backLayout->addWidget(backIcon);
    backLayout->addWidget(new QLabel("返回首页"));
    backLayout->setSpacing(8);
    backLayout->setContentsMargins(12, 8, 12, 8);

    connect(backBtn, &QPushButton::clicked, this, &PatientManagementWidget::onBackButtonClicked);

    QLabel *title = new QLabel("患者信息管理");
    title->setObjectName("headerTitle");

    // ... (User info part remains unchanged)
    QFrame *userFrame = new QFrame();
    userFrame->setObjectName("userFrame");
    QHBoxLayout *userLayout = new QHBoxLayout(userFrame);
    userLayout->setContentsMargins(12, 8, 12, 8);
    userLayout->setSpacing(8);
    QSvgWidget *userIcon = new QSvgWidget();
    userIcon->load(QByteArray(patientAvatarSvg));
    userIcon->setFixedSize(24, 24);
    QLabel *userName = new QLabel("王医生");
    userName->setObjectName("userName");
    userLayout->addWidget(userIcon);
    userLayout->addWidget(userName);

    headerLayout->addWidget(backBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(userFrame);

    return header;
}

QWidget* PatientManagementWidget::createSearchPanel() {
    QFrame *searchFrame = new QFrame();
    searchFrame->setObjectName("searchFrame");
    QVBoxLayout *layout = new QVBoxLayout(searchFrame);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    // ... (Search title part remains unchanged)
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *searchTitle = new QLabel("患者搜索");
    searchTitle->setObjectName("sectionTitle");
    titleLayout->addWidget(searchTitle);
    titleLayout->addStretch();
    QLabel *searchSubtitle = new QLabel("患者姓名/病历号");
    searchSubtitle->setObjectName("searchSubtitle");

    // **已修改：存储搜索框指针，并连接信号**
    QHBoxLayout *searchInputLayout = new QHBoxLayout();
    searchInput = new QLineEdit(); // 使用成员变量
    searchInput->setObjectName("searchInput");
    searchInput->setPlaceholderText("请输入患者姓名或病历号...");

    // **已修改：简化搜索按钮**
    QPushButton *searchBtn = new QPushButton("搜索");
    searchBtn->setObjectName("searchButton");

    searchInputLayout->addWidget(searchInput, 1);
    searchInputLayout->addWidget(searchBtn);
    searchInputLayout->setSpacing(12);

    // **已修改：连接信号到新的槽函数**
    connect(searchBtn, &QPushButton::clicked, this, &PatientManagementWidget::onSearchTriggered);
    connect(searchInput, &QLineEdit::returnPressed, this, &PatientManagementWidget::onSearchTriggered);

    layout->addLayout(titleLayout);
    layout->addWidget(searchSubtitle);
    layout->addLayout(searchInputLayout);

    return searchFrame;
}


QWidget* PatientManagementWidget::createContentArea() {
    QFrame *contentFrame = new QFrame();
    QHBoxLayout *contentLayout = new QHBoxLayout(contentFrame);
    contentLayout->setSpacing(16);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    contentLayout->addWidget(createPatientListPanel(), 3);
    contentLayout->addWidget(createPatientDetailPanel(), 7);

    return contentFrame;
}

QWidget* PatientManagementWidget::createPatientListPanel() {
    QFrame *listFrame = new QFrame();
    listFrame->setObjectName("patientListFrame");
    QVBoxLayout *layout = new QVBoxLayout(listFrame);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    QLabel *listTitle = new QLabel("患者列表");
    listTitle->setObjectName("sectionTitle");
    layout->addWidget(listTitle);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("patientScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollContent = new QWidget();
    patientListLayout = new QVBoxLayout(scrollContent); // 使用成员变量
    patientListLayout->setSpacing(4);
    patientListLayout->setContentsMargins(0, 0, 0, 0);

    patientListGroup = new QButtonGroup(this);
    patientListGroup->setExclusive(true);

    // **修改：列表的实际内容将在 updatePatientList() 中动态创建**

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea, 1);

    connect(patientListGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &PatientManagementWidget::onPatientSelected);

    return listFrame;
}

// **已删除：不再需要 createPatientListItem 函数，改用 PatientListItemWidget**

QWidget* PatientManagementWidget::createPatientDetailPanel() {
    QWidget *detailContainer = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(detailContainer);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(createPatientSummaryCard());
    mainLayout->addWidget(createDetailInfoGrid(), 1);
    mainLayout->addWidget(createActionButtons());

    return detailContainer;
}

// createPatientSummaryCard, createDetailInfoGrid, createActionButtons 保持不变
QWidget* PatientManagementWidget::createPatientSummaryCard() {
    QFrame *summaryCard = new QFrame();
    summaryCard->setObjectName("detailCard");
    QHBoxLayout *summaryLayout = new QHBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(24, 20, 24, 20);
    summaryLayout->setSpacing(16);

    QSvgWidget *largeAvatar = new QSvgWidget();
    largeAvatar->load(QByteArray(patientAvatarLargeSvg));
    largeAvatar->setFixedSize(60, 60);

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8);

    QHBoxLayout *nameStatusLayout = new QHBoxLayout();
    detailNameLabel = new QLabel();
    detailNameLabel->setObjectName("detailPatientName");
    statusInpatient = new QLabel("在院");
    statusInpatient->setObjectName("statusInpatient");
    nameStatusLayout->addWidget(detailNameLabel);
    nameStatusLayout->addWidget(statusInpatient);
    nameStatusLayout->addStretch();

    detailIdLabel = new QLabel();
    detailIdLabel->setObjectName("detailPatientId");

    QHBoxLayout *tagsLayout = new QHBoxLayout();
    statusInsurance = new QLabel("医保");
    statusInsurance->setObjectName("statusInsurance");
    statusKeyFocus = new QLabel("重点关注");
    statusKeyFocus->setObjectName("statusKeyFocus");
    tagsLayout->addWidget(statusInsurance);
    tagsLayout->addWidget(statusKeyFocus);
    tagsLayout->addStretch();

    infoLayout->addLayout(nameStatusLayout);
    infoLayout->addWidget(detailIdLabel);
    infoLayout->addLayout(tagsLayout);

    summaryLayout->addWidget(largeAvatar);
    summaryLayout->addLayout(infoLayout, 1);
    return summaryCard;
}

QWidget* PatientManagementWidget::createDetailInfoGrid() {
    QHBoxLayout *gridContainer = new QHBoxLayout();
    gridContainer->setSpacing(16);

    QFrame *basicInfoCard = new QFrame();
    basicInfoCard->setObjectName("detailCard");
    QVBoxLayout *basicLayout = new QVBoxLayout(basicInfoCard);
    basicLayout->setContentsMargins(20, 16, 20, 16);
    basicLayout->setSpacing(12);

    QHBoxLayout *basicTitleLayout = new QHBoxLayout();
    QLabel *basicTitle = new QLabel("👤 基本信息");
    basicTitle->setObjectName("cardTitle");
    basicTitleLayout->addWidget(basicTitle);
    basicTitleLayout->addStretch();

    QGridLayout *basicGrid = new QGridLayout();
    basicGrid->setSpacing(12);
    detailGenderLabel = new QLabel();
    detailAgeLabel = new QLabel();
    detailBirthLabel = new QLabel();
    detailIdCardLabel = new QLabel();

    basicGrid->addWidget(new QLabel("性别:"), 0, 0);
    basicGrid->addWidget(detailGenderLabel, 0, 1);
    basicGrid->addWidget(new QLabel("年龄:"), 1, 0);
    basicGrid->addWidget(detailAgeLabel, 1, 1);
    basicGrid->addWidget(new QLabel("出生日期:"), 2, 0);
    basicGrid->addWidget(detailBirthLabel, 2, 1);
    basicGrid->addWidget(new QLabel("身份证号:"), 3, 0);
    basicGrid->addWidget(detailIdCardLabel, 3, 1);

    basicLayout->addLayout(basicTitleLayout);
    basicLayout->addLayout(basicGrid);
    basicLayout->addStretch();

    QFrame *hospitalInfoCard = new QFrame();
    hospitalInfoCard->setObjectName("detailCard");
    QVBoxLayout *hospitalLayout = new QVBoxLayout(hospitalInfoCard);
    hospitalLayout->setContentsMargins(20, 16, 20, 16);
    hospitalLayout->setSpacing(12);

    QHBoxLayout *hospitalTitleLayout = new QHBoxLayout();
    QLabel *hospitalTitle = new QLabel("🏥 住院信息");
    hospitalTitle->setObjectName("cardTitle");
    hospitalTitleLayout->addWidget(hospitalTitle);
    hospitalTitleLayout->addStretch();

    QGridLayout *hospitalGrid = new QGridLayout();
    hospitalGrid->setSpacing(12);
    detailDeptLabel = new QLabel();
    detailBedLabel = new QLabel();
    detailAdmitDateLabel = new QLabel();
    detailStayDaysLabel = new QLabel();

    hospitalGrid->addWidget(new QLabel("科室:"), 0, 0);
    hospitalGrid->addWidget(detailDeptLabel, 0, 1);
    hospitalGrid->addWidget(new QLabel("床号:"), 1, 0);
    hospitalGrid->addWidget(detailBedLabel, 1, 1);
    hospitalGrid->addWidget(new QLabel("入院日期:"), 2, 0);
    hospitalGrid->addWidget(detailAdmitDateLabel, 2, 1);
    hospitalGrid->addWidget(new QLabel("住院天数:"), 3, 0);
    hospitalGrid->addWidget(detailStayDaysLabel, 3, 1);

    hospitalLayout->addLayout(hospitalTitleLayout);
    hospitalLayout->addLayout(hospitalGrid);
    hospitalLayout->addStretch();

    gridContainer->addWidget(basicInfoCard);
    gridContainer->addWidget(hospitalInfoCard);
    QWidget *gridWidget = new QWidget();
    gridWidget->setLayout(gridContainer);
    return gridWidget;
}

QWidget* PatientManagementWidget::createActionButtons() {
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(16);
    QPushButton *recordsBtn = new QPushButton();
    recordsBtn->setObjectName("actionButton");
    QVBoxLayout *recordsLayout = new QVBoxLayout(recordsBtn);
    recordsLayout->setContentsMargins(20, 16, 20, 16);
    recordsLayout->setSpacing(8);
    recordsLayout->setAlignment(Qt::AlignCenter);

    QSvgWidget *recordsIcon = new QSvgWidget();
    recordsIcon->load(QByteArray(recordsIconSvg));
    recordsIcon->setFixedSize(24, 24);

    QLabel *recordsTitle = new QLabel("查看病历");
    recordsTitle->setObjectName("actionButtonTitle");
    QLabel *recordsDesc = new QLabel("查看患者的完整病历记录");
    recordsDesc->setObjectName("actionButtonDesc");

    recordsLayout->addWidget(recordsIcon);
    recordsLayout->addWidget(recordsTitle);
    recordsLayout->addWidget(recordsDesc);

    QPushButton *ordersBtn = new QPushButton();
    ordersBtn->setObjectName("actionButton");
    QVBoxLayout *ordersLayout = new QVBoxLayout(ordersBtn);
    ordersLayout->setContentsMargins(20, 16, 20, 16);
    ordersLayout->setSpacing(8);
    ordersLayout->setAlignment(Qt::AlignCenter);

    QSvgWidget *ordersIcon = new QSvgWidget();
    ordersIcon->load(QByteArray(ordersIconSvg));
    ordersIcon->setFixedSize(24, 24);

    QLabel *ordersTitle = new QLabel("查看医嘱");
    ordersTitle->setObjectName("actionButtonTitle");
    QLabel *ordersDesc = new QLabel("查看患者的医嘱信息");
    ordersDesc->setObjectName("actionButtonDesc");

    ordersLayout->addWidget(ordersIcon);
    ordersLayout->addWidget(ordersTitle);
    ordersLayout->addWidget(ordersDesc);
    actionLayout->addWidget(recordsBtn);
    actionLayout->addWidget(ordersBtn);
    QWidget *actionWidget = new QWidget();
    actionWidget->setLayout(actionLayout);
    return actionWidget;
}

// **新增：实现搜索功能的槽函数**
void PatientManagementWidget::onSearchTriggered() {
    updatePatientList();
}

// **新增：更新患者列表的核心实现**
void PatientManagementWidget::updatePatientList() {
    QString searchText = searchInput->text().trimmed().toLower();

    // 清空现有列表项
    QLayoutItem* item;
    while ((item = patientListLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            QAbstractButton* button = qobject_cast<QAbstractButton*>(item->widget());
            if (button) {
                patientListGroup->removeButton(button);
            }
            delete item->widget();
        }
        delete item;
    }

    // 根据搜索文本过滤并重新填充列表
    for (const auto &patient : samplePatients) {
        bool matches = searchText.isEmpty() ||
                       patient.name.toLower().contains(searchText) ||
                       patient.patientId.toLower().contains(searchText);

        if (matches) {
            // **修改：使用 PatientListItemWidget**
            PatientListItemWidget *itemWidget = new PatientListItemWidget(patient);
            patientListLayout->addWidget(itemWidget);
            patientListGroup->addButton(itemWidget, patient.id);
        }
    }
    patientListLayout->addStretch();

    // 搜索后自动选中第一项，或清空详情
    QList<QAbstractButton*> buttons = patientListGroup->buttons();
    if (!buttons.isEmpty()) {
        buttons.first()->setChecked(true);
        onPatientSelected(patientListGroup->checkedId());
    } else {
        clearDetailPanel();
    }
}

void PatientManagementWidget::onPatientSelected(int id) {
    for (const auto &patient : samplePatients) {
        if (patient.id == id) {
            updateDetailPanel(patient);
            return;
        }
    }
}

void PatientManagementWidget::onBackButtonClicked() {
    emit backRequested();
}

void PatientManagementWidget::updateDetailPanel(const PatientData &data) {
    detailNameLabel->setText(data.name);
    detailIdLabel->setText("病历号: " + data.patientId);
    detailGenderLabel->setText("女"); // 示例数据
    detailAgeLabel->setText(QString::number(data.age) + "岁");
    detailBirthLabel->setText("1985-06-15"); // 示例数据
    detailIdCardLabel->setText("310105198506****"); // 示例数据
    detailDeptLabel->setText(data.department);
    detailBedLabel->setText("302床"); // 示例数据
    detailAdmitDateLabel->setText("2025-08-30"); // 示例数据
    detailStayDaysLabel->setText("3天"); // 示例数据

    statusInpatient->setVisible(data.isInpatient);
    statusInsurance->setVisible(data.isMedicalInsurance);
    statusKeyFocus->setVisible(data.isKeyFocus);
}

// **新增：清空详情面板的函数**
void PatientManagementWidget::clearDetailPanel() {
    detailNameLabel->clear();
    detailIdLabel->clear();
    detailGenderLabel->clear();
    detailAgeLabel->clear();
    detailBirthLabel->clear();
    detailIdCardLabel->clear();
    detailDeptLabel->clear();
    detailBedLabel->clear();
    detailAdmitDateLabel->clear();
    detailStayDaysLabel->clear();

    statusInpatient->hide();
    statusInsurance->hide();
    statusKeyFocus->hide();
}


void PatientManagementWidget::applyStyles() {
    this->setStyleSheet(R"(
        /* ... 样式表内容保持不变 ... */
        #patientManagementWidget { background-color: #f0f9ff; }
        #headerFrame { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #backButton { background-color: transparent; border: 1px solid #e2e8f0; border-radius: 8px; color: #64748b; font-size: 14px; }
        #backButton:hover { background-color: #f1f5f9; }
        #headerTitle { font-size: 20px; font-weight: bold; color: #1e293b; }
        #userFrame { background-color: #f8fafc; border-radius: 8px; }
        #userName { font-size: 14px; color: #475569; font-weight: 500; }
        #searchFrame { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #sectionTitle { font-size: 16px; font-weight: 600; color: #1e293b; }
        #searchSubtitle { font-size: 14px; color: #64748b; margin-top: -4px; }
        #searchInput { border: 1px solid #d1d5db; border-radius: 8px; padding: 12px 16px; font-size: 14px; background-color: white; }
        #searchInput:focus { border-color: #3b82f6; outline: none; }
        #searchButton { background-color: #3b82f6; color: white; border: none; border-radius: 8px; font-size: 14px; font-weight: 500; min-width: 100px; padding: 10px; }
        #searchButton:hover { background-color: #2563eb; }
        #patientListFrame { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #patientScrollArea { border: none; background-color: transparent; }

        /* PatientListItemWidget 样式 */
        #patientListItem { background-color: transparent; border: 1px solid transparent; border-radius: 8px; text-align: left; padding: 5px; }
        #patientListItem:hover { background-color: #f8fafc; }
        #patientListItem:checked { background-color: #dbeafe; border-color: #93c5fd; }
        #patientListItem #itemName { font-size: 14px; font-weight: 600; color: #1e293b; }
        #patientListItem #itemDetails { font-size: 12px; color: #64748b; }

        #detailCard { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #detailPatientName { font-size: 18px; font-weight: bold; color: #1e293b; }
        #detailPatientId { font-size: 14px; color: #64748b; }
        #statusInpatient { background-color: #dcfce7; color: #166534; font-size: 12px; font-weight: 500; padding: 4px 8px; border-radius: 6px; margin-left: 8px; }
        #statusInsurance { background-color: #dbeafe; color: #1d4ed8; font-size: 12px; font-weight: 500; padding: 4px 8px; border-radius: 6px; margin-right: 6px; }
        #statusKeyFocus { background-color: #fef3c7; color: #92400e; font-size: 12px; font-weight: 500; padding: 4px 8px; border-radius: 6px; }
        #cardTitle { font-size: 14px; font-weight: 600; color: #1e293b; }
        #actionButton { background-color: white; border: 1px solid #e2e8f0; border-radius: 12px; min-height: 120px; }
        #actionButton:hover { border-color: #3b82f6; background-color: #f8fafc; }
        #actionButtonTitle { font-size: 16px; font-weight: 600; color: #1e293b; text-align: center; }
        #actionButtonDesc { font-size: 12px; color: #64748b; text-align: center; }
    )");
}