#include "patient_management_widget.h"
#include "patient_data_types.h"
#include "patient_list_item_widget.h"
#include "medical_record_dialog.h"
#include "medical_orders_dialog.h"

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
#include <QAbstractButton>


// SVG图标定义
static const char* patientAvatarSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="40" height="40" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";
static const char* patientAvatarLargeSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="60" height="60" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";
static const char* recordsIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#3B82F6" stroke="white" stroke-width="1.5"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14,2 14,8 20,8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg>)";
static const char* ordersIconSvg  = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#10B981" stroke="white" stroke-width="1.5"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"/><rect x="8" y="2" width="8" height="4" rx="1" ry="1"/></svg>)";

PatientManagementWidget::PatientManagementWidget(QWidget *parent)
        : QWidget(parent)
{
    // —— 初始示例数据（可被 setPatientList 覆盖）——
    m_patients << PatientData{0, "张三", "P202305001", 38, "心血管内科", true,  true,  true}
               << PatientData{1, "李四", "P202305002", 45, "神经内科",   false, true,  false}
               << PatientData{2, "王五", "P202305003", 62, "消化内科",   true,  false, true}
               << PatientData{3, "刘六", "P202305004", 29, "呼吸内科",   false, true,  false}
               << PatientData{4, "陈七", "P202305005", 52, "骨科",       false, true,  false};

    initUI();
    applyStyles();
    updatePatientList();
}

void PatientManagementWidget::initUI() {
    this->setObjectName("patientManagementWidget");
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(16);

    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createSearchPanel());
    mainLayout->addWidget(createContentArea(), 1);
}

QWidget* PatientManagementWidget::createHeader() {
    auto *header = new QFrame();
    header->setObjectName("headerFrame");
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);

    auto *backBtn = new QPushButton("返回首页");
    backBtn->setObjectName("backButton");
    backBtn->setMinimumHeight(36);
    backBtn->setMinimumWidth(100);
    connect(backBtn, &QPushButton::clicked, this, &PatientManagementWidget::onBackButtonClicked);

    auto *title = new QLabel("患者信息管理");
    title->setObjectName("headerTitle");

    auto *userFrame = new QFrame();
    userFrame->setObjectName("userFrame");
    auto *userLayout = new QHBoxLayout(userFrame);
    userLayout->setContentsMargins(12, 8, 12, 8);
    userLayout->setSpacing(8);

    auto *userIcon = new QSvgWidget();
    userIcon->load(QByteArray(patientAvatarSvg));
    userIcon->setFixedSize(24, 24);

    auto *userName = new QLabel("王医生");
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
    auto *searchFrame = new QFrame();
    searchFrame->setObjectName("searchFrame");
    auto *layout = new QVBoxLayout(searchFrame);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    auto *titleLayout = new QHBoxLayout();
    auto *searchTitle = new QLabel("患者搜索");
    searchTitle->setObjectName("sectionTitle");
    titleLayout->addWidget(searchTitle);
    titleLayout->addStretch();
    auto *searchSubtitle = new QLabel("患者姓名/病历号");
    searchSubtitle->setObjectName("searchSubtitle");

    auto *searchInputLayout = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setObjectName("searchInput");
    searchInput->setPlaceholderText("请输入患者姓名或病历号...");

    auto *searchBtn = new QPushButton("搜索");
    searchBtn->setObjectName("searchButton");

    searchInputLayout->addWidget(searchInput, 1);
    searchInputLayout->addWidget(searchBtn);
    searchInputLayout->setSpacing(12);

    connect(searchBtn,   &QPushButton::clicked, this, &PatientManagementWidget::onSearchTriggered);
    connect(searchInput, &QLineEdit::returnPressed, this, &PatientManagementWidget::onSearchTriggered);

    layout->addLayout(titleLayout);
    layout->addWidget(searchSubtitle);
    layout->addLayout(searchInputLayout);

    return searchFrame;
}

QWidget* PatientManagementWidget::createContentArea() {
    auto *contentFrame = new QFrame();
    auto *contentLayout = new QHBoxLayout(contentFrame);
    contentLayout->setSpacing(16);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    contentLayout->addWidget(createPatientListPanel(), 3);
    contentLayout->addWidget(createPatientDetailPanel(), 7);

    return contentFrame;
}

QWidget* PatientManagementWidget::createPatientListPanel() {
    auto *panel = new QFrame();
    panel->setObjectName("patientListFrame");

    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(16, 16, 16, 16);
    vbox->setSpacing(8);

    auto *scroll = new QScrollArea();
    scroll->setObjectName("patientScrollArea");
    scroll->setWidgetResizable(true);

    auto *content = new QWidget();
    patientListLayout = new QVBoxLayout(content);
    patientListLayout->setSpacing(8);
    patientListLayout->setContentsMargins(0, 0, 0, 0);
    scroll->setWidget(content);

    vbox->addWidget(scroll);

    // 初始化分组，只在这里做一次
    if (!patientListGroup) {
        patientListGroup = new QButtonGroup(this);
        patientListGroup->setExclusive(true);
        connect(patientListGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                this, &PatientManagementWidget::onPatientSelected);
    }

    return panel;
}

void PatientManagementWidget::updateListItemProfile(const QJsonObject &profile)
{
    const int id = profile.value("id").toInt(profile.value("patient_id").toInt(-1));
    if (id < 0 || !m_itemById.contains(id)) return;
    m_itemById.value(id)->applyProfileSummary(profile);
}

void PatientManagementWidget::updateListItemRecordFlag(const QJsonObject &record, bool found)
{
    // 假设 record 里有 patient_id
    const int id = record.value("patient_id").toInt(-1);
    if (id < 0 || !m_itemById.contains(id)) return;
    m_itemById.value(id)->setRecordFound(found);
}

void PatientManagementWidget::updateListItemOrdersFlag(const QJsonObject &order, const QJsonArray &, bool found)
{
    // 假设 order 里有 patient_id
    const int id = order.value("patient_id").toInt(-1);
    if (id < 0 || !m_itemById.contains(id)) return;
    m_itemById.value(id)->setOrdersFound(found);
}
QWidget* PatientManagementWidget::createPatientDetailPanel() {
    auto *detailContainer = new QWidget();
    auto *mainLayout = new QVBoxLayout(detailContainer);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(createPatientSummaryCard());
    mainLayout->addWidget(createDetailInfoGrid(), 1);
    mainLayout->addWidget(createActionButtons());

    return detailContainer;
}

QWidget* PatientManagementWidget::createPatientSummaryCard() {
    auto *summaryCard = new QFrame();
    summaryCard->setObjectName("detailCard");
    auto *summaryLayout = new QHBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(24, 20, 24, 20);
    summaryLayout->setSpacing(16);

    auto *largeAvatar = new QSvgWidget();
    largeAvatar->load(QByteArray(patientAvatarLargeSvg));
    largeAvatar->setFixedSize(60, 60);

    auto *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8);

    auto *nameStatusLayout = new QHBoxLayout();
    detailNameLabel = new QLabel();
    detailNameLabel->setObjectName("detailPatientName");
    statusInpatient = new QLabel("在院");
    statusInpatient->setObjectName("statusInpatient");
    nameStatusLayout->addWidget(detailNameLabel);
    nameStatusLayout->addWidget(statusInpatient);
    nameStatusLayout->addStretch();

    detailIdLabel = new QLabel();
    detailIdLabel->setObjectName("detailPatientId");

    auto *tagsLayout = new QHBoxLayout();
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
    auto *gridContainer = new QHBoxLayout();
    gridContainer->setSpacing(16);

    auto *basicInfoCard = new QFrame();
    basicInfoCard->setObjectName("detailCard");
    auto *basicLayout = new QVBoxLayout(basicInfoCard);
    basicLayout->setContentsMargins(20, 16, 20, 16);
    basicLayout->setSpacing(12);

    auto *basicTitleLayout = new QHBoxLayout();
    auto *basicTitle = new QLabel("👤 基本信息");
    basicTitle->setObjectName("cardTitle");
    basicTitleLayout->addWidget(basicTitle);
    basicTitleLayout->addStretch();

    auto *basicGrid = new QGridLayout();
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

    auto *hospitalInfoCard = new QFrame();
    hospitalInfoCard->setObjectName("detailCard");
    auto *hospitalLayout = new QVBoxLayout(hospitalInfoCard);
    hospitalLayout->setContentsMargins(20, 16, 20, 16);
    hospitalLayout->setSpacing(12);

    auto *hospitalTitleLayout = new QHBoxLayout();
    auto *hospitalTitle = new QLabel("🏥 住院信息");
    hospitalTitle->setObjectName("cardTitle");
    hospitalTitleLayout->addWidget(hospitalTitle);
    hospitalTitleLayout->addStretch();

    auto *hospitalGrid = new QGridLayout();
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
    auto *gridWidget = new QWidget();
    gridWidget->setLayout(gridContainer);
    return gridWidget;
}

QWidget* PatientManagementWidget::createActionButtons() {
    auto *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(16);

    // 查看病历
    recordsBtn = new QPushButton();
    recordsBtn->setObjectName("actionButton");
    auto *recordsLayout = new QVBoxLayout(recordsBtn);
    recordsLayout->setContentsMargins(20, 16, 20, 16);
    recordsLayout->setSpacing(8);
    recordsLayout->setAlignment(Qt::AlignCenter);

    auto *recordsIcon = new QSvgWidget();
    recordsIcon->load(QByteArray(recordsIconSvg));
    recordsIcon->setFixedSize(24, 24);

    auto *recordsTitle = new QLabel("查看病历");
    recordsTitle->setObjectName("actionButtonTitle");
    auto *recordsDesc = new QLabel("查看患者的完整病历记录");
    recordsDesc->setObjectName("actionButtonDesc");

    recordsLayout->addWidget(recordsIcon);
    recordsLayout->addWidget(recordsTitle);
    recordsLayout->addWidget(recordsDesc);

    // 查看医嘱
    ordersBtn = new QPushButton();
    ordersBtn->setObjectName("actionButton");
    auto *ordersLayout = new QVBoxLayout(ordersBtn);
    ordersLayout->setContentsMargins(20, 16, 20, 16);
    ordersLayout->setSpacing(8);
    ordersLayout->setAlignment(Qt::AlignCenter);

    auto *ordersIcon = new QSvgWidget();
    ordersIcon->load(QByteArray(ordersIconSvg));
    ordersIcon->setFixedSize(24, 24);

    auto *ordersTitle = new QLabel("查看医嘱");
    ordersTitle->setObjectName("actionButtonTitle");
    auto *ordersDesc = new QLabel("查看患者的医嘱信息");
    ordersDesc->setObjectName("actionButtonDesc");

    ordersLayout->addWidget(ordersIcon);
    ordersLayout->addWidget(ordersTitle);
    ordersLayout->addWidget(ordersDesc);

    connect(recordsBtn, &QPushButton::clicked, this, &PatientManagementWidget::onViewMedicalRecordClicked);
    connect(ordersBtn,   &QPushButton::clicked, this, &PatientManagementWidget::onViewMedicalOrdersClicked);

    actionLayout->addWidget(recordsBtn);
    actionLayout->addWidget(ordersBtn);
    auto *actionWidget = new QWidget();
    actionWidget->setLayout(actionLayout);
    return actionWidget;
}

void PatientManagementWidget::onSearchTriggered() {
    updatePatientList();
}

void PatientManagementWidget::updatePatientList() {
    const QString searchText = searchInput->text().trimmed().toLower(); // 修复：toLower()

    // 清空旧项
    QLayoutItem* li;
    while ((li = patientListLayout->takeAt(0)) != nullptr) {
        if (li->widget()) {
            if (auto *btn = qobject_cast<QAbstractButton*>(li->widget())) {
                patientListGroup->removeButton(btn);
            }
            delete li->widget();
        }
        delete li;
    }

    // 重新生成
    int firstShownId = -1;
    for (const auto &p : m_patients) {
        bool matches = searchText.isEmpty()
                       || p.name.toLower().contains(searchText)
                       || p.patientId.toLower().contains(searchText);
        if (!matches) continue;

        auto *itemWidget = new PatientListItemWidget(p);
        patientListLayout->addWidget(itemWidget);
        patientListGroup->addButton(itemWidget, p.id);

        if (firstShownId == -1) firstShownId = p.id;
    }
    patientListLayout->addStretch();

    // 恢复/设置选中
    QList<QAbstractButton*> btns = patientListGroup->buttons();
    if (btns.isEmpty()) {
        clearDetailPanel();
        return;
    }

    int targetId = (m_currentSelectedPatientId != -1 && patientListGroup->button(m_currentSelectedPatientId))
                   ? m_currentSelectedPatientId : firstShownId;

    if (auto *btn = patientListGroup->button(targetId)) {
        btn->setChecked(true);
        onPatientSelected(targetId); // 触发详情刷新
    }
}

void PatientManagementWidget::onPatientSelected(int id) {
    m_currentSelectedPatientId = id;
    const PatientData *p = findPatientById(id);
    if (p) {
        updateDetailPanel(*p);
        recordsBtn->setEnabled(true);
        ordersBtn->setEnabled(true);
        emit currentPatientChanged(id);

        // ✅ 选中即请求加载服务端档案（用 Widget 作为指挥官）
        emit requestLoadPatientProfile(id);
    } else {
        clearDetailPanel();
    }
}

void PatientManagementWidget::onBackButtonClicked() {
    emit backRequested();
}

void PatientManagementWidget::onViewMedicalRecordClicked() {
    const PatientData *p = findPatientById(m_currentSelectedPatientId);
    if (!p) { qDebug() << "No patient selected for medical record view"; return; }
    auto *dialog = new MedicalRecordDialog(*p, this);
    dialog->exec();
    dialog->deleteLater();
}

void PatientManagementWidget::onViewMedicalOrdersClicked() {
    const PatientData *p = findPatientById(m_currentSelectedPatientId);
    if (!p) { qDebug() << "No patient selected for medical orders view"; return; }

    // 原：auto *dialog = new MedicalOrdersDialog(*p, this);
    auto *dialog = new MedicalOrdersDialog(p->id, p->name, this);

    dialog->exec();
    dialog->deleteLater();
}


void PatientManagementWidget::updateDetailPanel(const PatientData &data) {
    detailNameLabel->setText(data.name);
    detailIdLabel->setText("病历号: " + data.patientId);
    detailGenderLabel->setText("女");
    detailAgeLabel->setText(QString::number(data.age) + "岁");
    detailBirthLabel->setText("1985-06-15");
    detailIdCardLabel->setText("310105198506****");
    detailDeptLabel->setText(data.department);
    detailBedLabel->setText("302床");
    detailAdmitDateLabel->setText("2025-08-30");
    detailStayDaysLabel->setText("3天");

    statusInpatient->setVisible(data.isInpatient);
    statusInsurance->setVisible(data.isMedicalInsurance);
    statusKeyFocus->setVisible(data.isKeyFocus);
}

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

    recordsBtn->setEnabled(false);
    ordersBtn->setEnabled(false);
    m_currentSelectedPatientId = -1;
}

void PatientManagementWidget::applyStyles() {
    this->setStyleSheet(R"(
        #patientManagementWidget { background-color: #f0f9ff; }
        #headerFrame { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #backButton {
            background-color: transparent;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            color: #64748b;
            font-size: 14px; font-weight: 500;
            padding: 8px 16px; text-align: left;
            min-width: 120px; min-height: 36px;
        }
        #backButton:hover { background-color: #f1f5f9; border-color: #cbd5e1; }
        #backButton:pressed { background-color: #e2e8f0; }
        #headerTitle { font-size: 20px; font-weight: bold; color: #1e293b; }
        #userFrame { background-color: #f8fafc; border-radius: 8px; }
        #userName { font-size: 14px; color: #475569; font-weight: 500; }
        #searchFrame { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #sectionTitle { font-size: 16px; font-weight: 600; color: #1e293b; }
        #searchSubtitle { font-size: 14px; color: #64748b; margin-top: -4px; }
        #searchInput {
            border: 1px solid #d1d5db; border-radius: 8px;
            padding: 12px 16px; font-size: 14px; background-color: white;
        }
        #searchInput:focus { border-color: #3b82f6; outline: none; }
        #searchButton {
            background-color: #3b82f6; color: white; border: none; border-radius: 8px;
            font-size: 14px; font-weight: 500; min-width: 100px; padding: 10px;
        }
        #searchButton:hover { background-color: #2563eb; }
        #patientListFrame { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #patientScrollArea { border: none; background-color: transparent; }

        #patientListItem {
            background-color: white; border: 1px solid #e2e8f0;
            border-radius: 8px; padding: 0px; margin: 2px 0px;
            text-align: left; min-height: 70px; max-height: 80px;
        }
        #patientListItem:hover { background-color: #f8fafc; border-color: #cbd5e1; }
        #patientListItem:checked { background-color: #dbeafe; border-color: #3b82f6; }
        #patientListItem #itemName { font-size: 14px; font-weight: 600; color: #1e293b; }
        #patientListItem #itemDetails { font-size: 12px; color: #64748b; margin-top: 2px; }

        #detailCard { background-color: white; border-radius: 12px; border: 1px solid #e2e8f0; }
        #detailPatientName { font-size: 18px; font-weight: bold; color: #1e293b; }
        #detailPatientId { font-size: 14px; color: #64748b; }
        #statusInpatient { background-color: #dcfce7; color: #166534; font-size: 12px; font-weight: 500; padding: 4px 8px; border-radius: 6px; margin-left: 8px; }
        #statusInsurance { background-color: #dbeafe; color: #1d4ed8; font-size: 12px; font-weight: 500; padding: 4px 8px; border-radius: 6px; margin-right: 6px; }
        #statusKeyFocus { background-color: #fef3c7; color: #92400e; font-size: 12px; font-weight: 500; padding: 4px 8px; border-radius: 6px; }
        #cardTitle { font-size: 14px; font-weight: 600; color: #1e293b; }
        #actionButton { background-color: white; border: 1px solid #e2e8f0; border-radius: 12px; min-height: 120px; }
        #actionButton:hover { border-color: #3b82f6; background-color: #f8fafc; }
        #actionButton:disabled { background-color: #f1f5f9; border-color: #e2e8f0; opacity: 0.6; }
        #actionButtonTitle { font-size: 16px; font-weight: 600; color: #1e293b; text-align: center; }
        #actionButtonDesc { font-size: 12px; color: #64748b; text-align: center; }
    )");
}

// ===================== 运行时数据绑定/操作 =====================

void PatientManagementWidget::setPatientList(const QJsonArray &arr)
{
    QList<PatientData> newList;
    newList.reserve(arr.size());
    for (const auto &v : arr) {
        if (!v.isObject()) continue;
        newList.push_back(parsePatientJson(v.toObject()));
    }
    // 尽量保留当前选中 id
    const int keepId = m_currentSelectedPatientId;
    m_patients.swap(newList);
    if (findPatientById(keepId)) m_currentSelectedPatientId = keepId;
    else m_currentSelectedPatientId = -1;

    updatePatientList();
}

void PatientManagementWidget::upsertPatient(const QJsonObject &o)
{
    if (!o.contains("id")) return;
    const int id = o.value("id").toInt();
    const PatientData data = parsePatientJson(o);

    const int idx = findPatientIndexById(id);
    if (idx >= 0) {
        m_patients[idx] = data;
    } else {
        m_patients.push_back(data);
    }
    if (m_currentSelectedPatientId == -1) m_currentSelectedPatientId = id;
    updatePatientList();
}

void PatientManagementWidget::removePatientById(int id)
{
    const int idx = findPatientIndexById(id);
    if (idx < 0) return;
    m_patients.removeAt(idx);

    if (m_currentSelectedPatientId == id) m_currentSelectedPatientId = -1;
    updatePatientList();
}

void PatientManagementWidget::selectPatientById(int id)
{
    if (!patientListGroup) { m_currentSelectedPatientId = id; return; }
    if (auto *btn = patientListGroup->button(id)) {
        btn->setChecked(true);
        onPatientSelected(id);
    } else {
        // 若当前 UI 不在列表中，先记录 id，等下次刷新列表时会自动应用
        m_currentSelectedPatientId = id;
        updatePatientList();
    }
}

void PatientManagementWidget::setSearchText(const QString &text)
{
    if (!searchInput) return;
    searchInput->setText(text);
    updatePatientList();
}
void PatientManagementWidget::setPatientProfile(const QJsonObject &profile)
{
    // 兼容不同键名
    int id = profile.value("id").toInt(profile.value("patient_id").toInt(-1));
    if (id < 0) return;

    m_profileById.insert(id, profile);

    // 只更新当前选中的患者详情
    if (id != m_currentSelectedPatientId) return;

    // 安全取字段的小工具
    auto str = [](const QJsonObject &o, const char *k1, const char *k2 = nullptr, const char *fallback = "-"){
        if (o.contains(k1)) return o.value(k1).toVariant().toString();
        if (k2 && o.contains(k2)) return o.value(k2).toVariant().toString();
        return QString::fromUtf8(fallback);
    };
    auto num = [](const QJsonObject &o, const char *k1, const char *k2 = nullptr, int fallback = 0){
        if (o.contains(k1)) return o.value(k1).toVariant().toInt();
        if (k2 && o.contains(k2)) return o.value(k2).toVariant().toInt();
        return fallback;
    };
    auto flag = [](const QJsonObject &o, const char *k1, const char *k2 = nullptr, bool fallback = false){
        if (o.contains(k1)) return o.value(k1).toBool();
        if (k2 && o.contains(k2)) return o.value(k2).toBool();
        return fallback;
    };

    // 基本信息（优先 profile，缺省退回列表数据）
    const PatientData *p = findPatientById(id);
    const QString name  = str(profile, "name",  nullptr, p ? p->name.toUtf8().constData() : "-");
    const QString pid   = str(profile, "patient_id", "patientId", p ? p->patientId.toUtf8().constData() : "-");
    const QString gender= str(profile, "gender", nullptr, "-");
    const int     age   = num(profile, "age", nullptr, p ? p->age : 0);
    const QString birth = str(profile, "birth_date", "birthday", "-");
    const QString idNo  = str(profile, "id_card", "idNo", "****");
    const QString dept  = str(profile, "department", nullptr, p ? p->department.toUtf8().constData() : "-");
    const QString bed   = str(profile, "bed_no", "bedNo", "-");
    const QString admit = str(profile, "admit_date", "admitDate", "-");
    const int     stay  = num(profile, "stay_days", "stayDays", 0);

    const bool inpatient = flag(profile, "is_inpatient", "isInpatient", p ? p->isInpatient : false);
    const bool insured   = flag(profile, "is_medical_insurance", "isMedicalInsurance", p ? p->isMedicalInsurance : false);
    const bool keyfocus  = flag(profile, "is_key_focus", "isKeyFocus", p ? p->isKeyFocus : false);

    // 刷新 UI
    detailNameLabel->setText(name);
    detailIdLabel->setText("病历号: " + pid);
    detailGenderLabel->setText(gender.isEmpty() ? "-" : gender);
    detailAgeLabel->setText(age > 0 ? QString::number(age) + "岁" : "-");
    detailBirthLabel->setText(birth);
    detailIdCardLabel->setText(idNo);
    detailDeptLabel->setText(dept);
    detailBedLabel->setText(bed);
    detailAdmitDateLabel->setText(admit);
    detailStayDaysLabel->setText(stay > 0 ? QString::number(stay) + "天" : "-");

    statusInpatient->setVisible(inpatient);
    statusInsurance->setVisible(insured);
    statusKeyFocus->setVisible(keyfocus);
}
// ===================== 工具函数 =====================

PatientData PatientManagementWidget::parsePatientJson(const QJsonObject &o)
{
    PatientData p;
    p.id         = o.value("id").toInt();
    p.name       = o.value("name").toString();
    p.patientId  = o.value("patient_id").toString(o.value("patientId").toString());
    p.age        = o.value("age").toInt();
    p.department = o.value("department").toString();
    p.isInpatient        = o.value("is_inpatient").toBool(o.value("isInpatient").toBool());
    p.isMedicalInsurance = o.value("is_medical_insurance").toBool(o.value("isMedicalInsurance").toBool());
    p.isKeyFocus         = o.value("is_key_focus").toBool(o.value("isKeyFocus").toBool());
    return p;
}

const PatientData* PatientManagementWidget::findPatientById(int id) const
{
    for (const auto &p : m_patients)
        if (p.id == id) return &p;
    return nullptr;
}

int PatientManagementWidget::findPatientIndexById(int id) const
{
    for (int i = 0; i < m_patients.size(); ++i)
        if (m_patients[i].id == id) return i;
    return -1;
}
