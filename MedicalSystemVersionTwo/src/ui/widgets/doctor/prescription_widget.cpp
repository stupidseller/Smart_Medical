#include "prescription_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QFrame>
#include <QSvgWidget>
#include <QDateTime>

// —— 内嵌 SVG ——
static const char* backIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";
static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* trashIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="3 6 5 6 21 6"></polyline><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path><line x1="10" y1="11" x2="10" y2="17"></line><line x1="14" y1="11" x2="14" y2="17"></line></svg>)";
static const char* diagnosisIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14.5 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7.5L14.5 2z"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="12" y1="18" x2="12" y2="12"></line><line x1="9" y1="15" x2="15" y2="15"></line></svg>)";
static const char* drugIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="8" x2="12" y2="16"></line><line x1="8" y1="12" x2="16" y2="12"></line></svg>)";
static const char* adviceIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.44 11.05l-9.19 9.19a6 6 0 0 1-8.49-8.49l9.19-9.19a4 4 0 0 1 5.66 5.66l-9.2 9.19a2 2 0 0 1-2.83-2.83l8.49-8.48"></path></svg>)";

PrescriptionWidget::PrescriptionWidget(QWidget *parent)
        : QWidget(parent)
{
    initUI();
    applyStyles();
}

PrescriptionWidget::~PrescriptionWidget()
{
    for (auto *r : m_drugRows) delete r->row;
    m_drugRows.clear();
}

void PrescriptionWidget::initUI()
{
    this->setObjectName("prescriptionWidget");
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(createHeader());

    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(80, 30, 80, 50);
    contentLayout->setSpacing(30);

    contentLayout->addWidget(createPatientSelection());
    contentLayout->addWidget(createDiagnosisSection());
    contentLayout->addWidget(createDrugsSection());
    contentLayout->addWidget(createAdviceSection());
    contentLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);

    mainLayout->addWidget(createFooterButtons());
}

QWidget* PrescriptionWidget::createSectionTitle(const char* svgIcon, const QString& title) {
    auto *container = new QWidget();
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(10);
    auto* icon = new QSvgWidget();
    icon->load(QByteArray(svgIcon));
    icon->setFixedSize(22, 22);
    auto* label = new QLabel(title);
    label->setObjectName("sectionTitle");
    layout->addWidget(icon);
    layout->addWidget(label);
    layout->addStretch();
    return container;
}

QWidget* PrescriptionWidget::createHeader() {
    auto *header = new QWidget();
    header->setObjectName("header");
    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(20, 10, 20, 10);

    auto* backButton = new QPushButton(" ");
    backButton->setObjectName("backButton");
    auto* backIconWidget = new QSvgWidget();
    backIconWidget->load(QByteArray(backIcon));
    backIconWidget->setFixedSize(20, 20);
    auto* backLayout = new QHBoxLayout(backButton);
    backLayout->setContentsMargins(6,0,6,0);
    backLayout->addWidget(backIconWidget);
    connect(backButton, &QPushButton::clicked, this, &PrescriptionWidget::backRequested);

    auto* title = new QLabel("开具电子处方");
    title->setObjectName("titleLabel");

    auto* doctorInfo = new QWidget();
    auto* doctorLayout = new QHBoxLayout(doctorInfo);
    doctorLayout->setContentsMargins(0,0,0,0);
    auto* userIconWidget = new QSvgWidget();
    userIconWidget->load(QByteArray(userIconSvg));
    userIconWidget->setFixedSize(22,22);
    auto* doctorName = new QLabel("王医生");
    doctorName->setObjectName("doctorNameLabel");
    doctorLayout->addWidget(userIconWidget);
    doctorLayout->addWidget(doctorName);

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(doctorInfo);

    return header;
}

QWidget* PrescriptionWidget::createPatientSelection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);

    layout->addWidget(createSectionTitle(userIconSvg, "选择患者"));

    m_patientCombo = new QComboBox();
    m_patientCombo->addItem("请选择患者...", -1);
    connect(m_patientCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PrescriptionWidget::onPatientComboChanged);

    auto* patientInfoContainer = new QWidget();
    patientInfoContainer->setObjectName("patientInfoContainer");
    auto* infoLayout = new QHBoxLayout(patientInfoContainer);
    infoLayout->setContentsMargins(20, 15, 20, 15);
    infoLayout->setSpacing(40);
    m_labelName   = new QLabel("姓名：-");
    m_labelPid    = new QLabel("ID: -");
    m_labelGender = new QLabel("性别：-");
    m_labelAge    = new QLabel("年龄：-");
    infoLayout->addWidget(m_labelName);
    infoLayout->addWidget(m_labelPid);
    infoLayout->addWidget(m_labelGender);
    infoLayout->addWidget(m_labelAge);
    infoLayout->addStretch();

    layout->addWidget(m_patientCombo);
    layout->addWidget(patientInfoContainer);

    return container;
}

QWidget* PrescriptionWidget::createDiagnosisSection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(createSectionTitle(diagnosisIcon, "诊断信息"));

    auto* gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    m_mainDiagnosisEdit = new QLineEdit();
    m_mainDiagnosisEdit->setPlaceholderText("请输入主要诊断");
    gridLayout->addWidget(new QLabel("主要诊断"), 0, 0);
    gridLayout->addWidget(m_mainDiagnosisEdit, 0, 1);

    m_departmentCombo = new QComboBox();
    m_departmentCombo->addItems(QStringList() << "心血管内科" << "呼吸科" << "神经科" << "消化内科" << "骨科");
    gridLayout->addWidget(new QLabel("科室"), 0, 2);
    gridLayout->addWidget(m_departmentCombo, 0, 3);

    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(3, 1);

    layout->addLayout(gridLayout);
    return container;
}

QWidget* PrescriptionWidget::createDrugsSection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(createSectionTitle(drugIcon, "药品信息"));

    auto* header = new QWidget();
    header->setObjectName("drugsHeader");
    auto* headerLayout = new QGridLayout(header);
    headerLayout->addWidget(new QLabel("药品名称"), 0, 0, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("规格"), 0, 1, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("数量"), 0, 2, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("用法用量"), 0, 3, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("天数"), 0, 4, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("操作"), 0, 5, Qt::AlignCenter);
    headerLayout->setColumnStretch(0, 3);
    headerLayout->setColumnStretch(1, 2);
    headerLayout->setColumnStretch(2, 1);
    headerLayout->setColumnStretch(3, 3);
    headerLayout->setColumnStretch(4, 1);
    headerLayout->setColumnStretch(5, 1);

    layout->addWidget(header);

    m_drugsLayout = new QVBoxLayout();
    m_drugsLayout->setSpacing(10);
    layout->addLayout(m_drugsLayout);

    auto* addDrugButton = new QPushButton("+ 添加药品");
    addDrugButton->setObjectName("addDrugButton");
    connect(addDrugButton, &QPushButton::clicked, this, &PrescriptionWidget::onAddDrugClicked);

    auto* buttonContainer = new QHBoxLayout();
    buttonContainer->addWidget(addDrugButton);
    buttonContainer->addStretch();
    layout->addLayout(buttonContainer);

    return container;
}

QWidget* PrescriptionWidget::createAdviceSection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(createSectionTitle(adviceIcon, "医嘱与建议"));

    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);

    m_guidanceEdit = new QTextEdit();
    m_guidanceEdit->setPlaceholderText("请严格按时服药，不要随意停药或调整剂量。服药期间注意监测血压，如出现不适请及时就医。");
    m_guidanceEdit->setMinimumHeight(100);

    m_precautionsEdit = new QTextEdit();
    m_precautionsEdit->setPlaceholderText("定期复查肝肾功能和血脂水平，保持低盐低脂饮食，适量运动，控制体重。");
    m_precautionsEdit->setMinimumHeight(100);

    formLayout->addRow("用药指导", m_guidanceEdit);
    formLayout->addRow("注意事项", m_precautionsEdit);

    layout->addLayout(formLayout);
    return container;
}

QWidget* PrescriptionWidget::createFooterButtons() {
    auto* footer = new QWidget();
    footer->setObjectName("footer");
    auto* layout = new QHBoxLayout(footer);
    layout->setContentsMargins(0, 15, 80, 15);
    layout->addStretch();

    auto* cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, &QPushButton::clicked, this, &PrescriptionWidget::onCancelClicked);

    auto* saveButton = new QPushButton("暂存");
    saveButton->setObjectName("saveButton");
    connect(saveButton, &QPushButton::clicked, this, &PrescriptionWidget::onSaveClicked);

    auto* issueButton = new QPushButton("开具处方");
    issueButton->setObjectName("issueButton");
    connect(issueButton, &QPushButton::clicked, this, &PrescriptionWidget::onIssueClicked);

    layout->addWidget(cancelButton);
    layout->addWidget(saveButton);
    layout->addWidget(issueButton);

    return footer;
}

void PrescriptionWidget::applyStyles() {
    this->setStyleSheet(R"(
        #prescriptionWidget, #contentWidget { background-color: #F7FAFC; }
        #scrollArea { border: none; }
        #header { background-color: white; border-bottom: 1px solid #E2E8F0; }
        #backButton { background-color: transparent; border: none; color: #4A5568; font-size: 14px; padding: 8px 12px; }
        #backButton:hover { background-color: #F7FAFC; }
        #backButton QSvgWidget { stroke: #4A5568; }
        #titleLabel { font-size: 18px; font-weight: bold; color: #2D3748; }
        #doctorNameLabel { font-size: 14px; color: #4A5568; }
        #header QSvgWidget { stroke: #718096; }

        #sectionContainer { background-color: white; border: 1px solid #E2E8F0; border-radius: 8px; padding: 20px; }
        #sectionTitle { font-size: 16px; font-weight: bold; color: #2D3748; }
        #sectionContainer QSvgWidget { stroke: #3182CE; }

        #patientInfoContainer { background-color: #F7FAFC; border-radius: 6px; margin-top: 10px; }
        #patientInfoContainer QLabel { color: #4A5568; }

        QComboBox, QLineEdit, QTextEdit {
            border: 1px solid #CBD5E0; border-radius: 6px; padding: 8px 12px; background-color: white; font-size: 14px;
        }
        QComboBox:focus, QLineEdit:focus, QTextEdit:focus { border-color: #3182CE; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: none; }

        #drugsHeader { background-color: #EDF2F7; border-radius: 6px; padding: 10px 0; margin-bottom: 10px; }
        #drugsHeader QLabel { font-weight: bold; color: #4A5568; }

        #drugRow { padding: 5px 0; }
        #removeDrugButton { background-color: #FFF5F5; border: 1px solid #E53E3E; color: #E53E3E; border-radius: 6px; padding: 5px; }
        #removeDrugButton:hover { background-color: #E53E3E; color: white; }
        #removeDrugButton QSvgWidget { stroke: #E53E3E; }
        #removeDrugButton:hover QSvgWidget { stroke: white; }

        #addDrugButton { background-color: #E6FFFA; color: #2C7A7B; border: 1px dashed #38B2AC; font-weight: bold; max-width: 120px; }
        #addDrugButton:hover { background-color: #B2F5EA; }

        #footer { background-color: white; border-top: 1px solid #E2E8F0; }
        #footer QPushButton { padding: 10px 25px; font-size: 14px; border-radius: 6px; font-weight: bold; }
        #cancelButton { background-color: #E2E8F0; color: #4A5568; border: none; }
        #cancelButton:hover { background-color: #CBD5E0; }
        #saveButton { background-color: white; color: #3182CE; border: 1px solid #3182CE; }
        #saveButton:hover { background-color: #EBF8FF; }
        #issueButton { background-color: #3182CE; color: white; border: none; }
        #issueButton:hover { background-color: #2B6CB0; }
    )");
}

// ===================== 槽函数 =====================

void PrescriptionWidget::onAddDrugClicked() { addDrug(QJsonObject()); }

void PrescriptionWidget::onRemoveDrugClicked() {
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    for (auto *r : m_drugRows) if (r->removeBtn == btn) { deleteDrugRow(r); break; }
}

void PrescriptionWidget::onCancelClicked() { emit cancelRequested(); }

void PrescriptionWidget::onSaveClicked()  { emit saveRequested(buildPrescriptionJson()); }

void PrescriptionWidget::onIssueClicked() { emit issueRequested(buildPrescriptionJson()); }

void PrescriptionWidget::onPatientComboChanged(int index) {
    const int id = m_patientCombo->itemData(index).toInt();
    m_currentPatientId = id;
    updatePatientInfoLabels(id);
    emit currentPatientChanged(id);
}

// ===================== 数据绑定 =====================

void PrescriptionWidget::setPatientList(const QJsonArray &patients)
{
    m_patientMap.clear();
    m_patientCombo->clear();
    m_patientCombo->addItem("请选择患者...", -1);

    for (const auto &v : patients) {
        if (!v.isObject()) continue;
        const auto o = v.toObject();
        const int id = o.value("id").toInt(-1);
        if (id < 0) continue;
        m_patientMap.insert(id, o);

        const QString name = o.value("name").toString();
        const QString pid  = o.value("patient_id").toString(o.value("patientId").toString());
        m_patientCombo->addItem(QString("%1 (%2)").arg(name, pid), id);
    }

    int idx = m_patientCombo->findData(m_currentPatientId);
    if (idx < 0) idx = 0;
    m_patientCombo->setCurrentIndex(idx);
    updatePatientInfoLabels(m_currentPatientId);
}

void PrescriptionWidget::selectPatientById(int id)
{
    int idx = m_patientCombo->findData(id);
    if (idx >= 0) m_patientCombo->setCurrentIndex(idx);
}

void PrescriptionWidget::setDiagnosis(const QJsonObject &diagnosis)
{
    if (diagnosis.contains("main"))
        m_mainDiagnosisEdit->setText(diagnosis.value("main").toString());

    if (diagnosis.contains("department")) {
        const QString dep = diagnosis.value("department").toString();
        int i = m_departmentCombo->findText(dep);
        if (i < 0) m_departmentCombo->addItem(dep);
        i = m_departmentCombo->findText(dep);
        if (i >= 0) m_departmentCombo->setCurrentIndex(i);
    }
}

void PrescriptionWidget::setAdvice(const QJsonObject &advice)
{
    if (advice.contains("guidance"))
        m_guidanceEdit->setPlainText(advice.value("guidance").toString());
    if (advice.contains("precautions"))
        m_precautionsEdit->setPlainText(advice.value("precautions").toString());
}

// === 新增：药品目录 ===
void PrescriptionWidget::setMedicineCatalog(const QJsonArray &medicines)
{
    m_medsByName.clear();
    m_medsById.clear();
    m_medNames.clear();

    for (const auto &v : medicines) {
        if (!v.isObject()) continue;
        const auto o = v.toObject();
        const int id = o.value("id").toInt(-1);
        const QString name = o.value("name").toString();
        if (id < 0 || name.isEmpty()) continue;
        m_medsByName.insert(name, o);
        m_medsById.insert(id, o);
        m_medNames << name;
    }

    // 更新所有已存在行的下拉框内容（保留当前文本）
    for (auto *r : m_drugRows) {
        const QString keep = r->name->currentText();
        r->name->clear();
        if (!m_medNames.isEmpty()) r->name->addItems(m_medNames);
        else                       r->name->addItems(QStringList() << "硝苯地平控释片" << "阿司匹林肠溶片");
        if (!keep.isEmpty()) r->name->setCurrentText(keep);
    }
}

void PrescriptionWidget::setDrugs(const QJsonArray &drugs)
{
    clearDrugs();
    for (const auto &v : drugs) if (v.isObject()) addDrug(v.toObject());
}

void PrescriptionWidget::addDrug(const QJsonObject &drug)
{
    auto *row = createDrugRow(&drug);
    m_drugsLayout->addWidget(row->row);
    m_drugRows.push_back(row);
}

void PrescriptionWidget::clearDrugs()
{
    for (auto *r : m_drugRows) delete r->row;
    m_drugRows.clear();
}

// ===================== 辅助实现 =====================

PrescriptionWidget::DrugRow* PrescriptionWidget::createDrugRow(const QJsonObject *preset)
{
    auto* r = new DrugRow();
    r->row = new QWidget();
    r->row->setObjectName("drugRow");
    auto* layout = new QGridLayout(r->row);
    layout->setContentsMargins(0,0,0,0);

    r->name = new QComboBox();
    // 用服务端药品目录；没有时退回默认两项
    if (!m_medNames.isEmpty()) r->name->addItems(m_medNames);
    else                       r->name->addItems(QStringList() << "硝苯地平控释片" << "阿司匹林肠溶片");
    r->name->setEditable(true);

    r->spec     = new QLineEdit();
    r->quantity = new QLineEdit();
    r->dosage   = new QLineEdit();
    r->days     = new QLineEdit();

    if (preset) {
        if (preset->contains("name"))     r->name->setCurrentText(preset->value("name").toString());
        if (preset->contains("spec"))     r->spec->setText(preset->value("spec").toString());
        if (preset->contains("quantity")) r->quantity->setText(preset->value("quantity").toVariant().toString());
        if (preset->contains("dosage"))   r->dosage->setText(preset->value("dosage").toString());
        if (preset->contains("days"))     r->days->setText(preset->value("days").toVariant().toString());
    } else {
        r->spec->setText("30mgx7片");
        r->quantity->setText("4");
        r->dosage->setText("每日1次, 每次1片");
        r->days->setText("28");
    }

    r->removeBtn = new QPushButton();
    r->removeBtn->setObjectName("removeDrugButton");
    auto* trashIconWidget = new QSvgWidget(r->removeBtn);
    trashIconWidget->load(QByteArray(trashIcon));
    trashIconWidget->setFixedSize(16,16);
    trashIconWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    auto* btnLayout = new QHBoxLayout(r->removeBtn);
    btnLayout->setContentsMargins(6,2,6,2);
    btnLayout->addWidget(trashIconWidget);
    connect(r->removeBtn, &QPushButton::clicked, this, &PrescriptionWidget::onRemoveDrugClicked);

    layout->addWidget(r->name,     0, 0);
    layout->addWidget(r->spec,     0, 1);
    layout->addWidget(r->quantity, 0, 2);
    layout->addWidget(r->dosage,   0, 3);
    layout->addWidget(r->days,     0, 4);
    layout->addWidget(r->removeBtn,0, 5, Qt::AlignCenter);

    layout->setColumnStretch(0, 3);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 1);
    layout->setColumnStretch(3, 3);
    layout->setColumnStretch(4, 1);
    layout->setColumnStretch(5, 1);
    return r;
}

void PrescriptionWidget::deleteDrugRow(DrugRow* r)
{
    if (!r) return;
    m_drugRows.removeOne(r);
    r->row->deleteLater();
}

QJsonArray PrescriptionWidget::serializeDrugs() const
{
    QJsonArray arr;
    for (auto *r : m_drugRows) {
        QJsonObject o{
            {"name",     r->name->currentText()},
            {"spec",     r->spec->text()},
            {"quantity", r->quantity->text()},
            {"dosage",   r->dosage->text()},
            {"days",     r->days->text()}
        };
        arr.append(o);
    }
    return arr;
}

void PrescriptionWidget::updatePatientInfoLabels(int patientId)
{
    const auto it = m_patientMap.constFind(patientId);
    if (it == m_patientMap.constEnd()) {
        m_labelName->setText("姓名：-");
        m_labelPid->setText("ID: -");
        m_labelGender->setText("性别：-");
        m_labelAge->setText("年龄：-");
        return;
    }
    const auto &o = it.value();
    const QString name   = o.value("name").toString("-");
    const QString pid    = o.value("patient_id").toString(o.value("patientId").toString("-"));
    const QString gender = o.value("gender").toString("-");
    const QJsonValue vAge = o.value("age");
    const QString ageStr = vAge.isDouble()
            ? QString::number(int(vAge.toDouble()))
            : vAge.toString("-");
    m_labelName->setText("姓名：" + name);
    m_labelPid->setText("ID: " + pid);
    m_labelGender->setText("性别：" + gender);
    m_labelAge->setText("年龄：" + ageStr + "岁");
}

// ===================== 序列化导出 =====================

QJsonObject PrescriptionWidget::buildPrescriptionJson() const
{
    QJsonObject patient;
    if (m_patientMap.contains(m_currentPatientId))
        patient = m_patientMap.value(m_currentPatientId);

    QJsonObject diagnosis{
        {"main", m_mainDiagnosisEdit ? m_mainDiagnosisEdit->text() : QString()},
        {"department", m_departmentCombo ? m_departmentCombo->currentText() : QString()}
    };

    QJsonObject advice{
        {"guidance",    m_guidanceEdit ? m_guidanceEdit->toPlainText() : QString()},
        {"precautions", m_precautionsEdit ? m_precautionsEdit->toPlainText() : QString()}
    };

    QJsonObject root{
        {"timestamp", QDateTime::currentDateTime().toString(Qt::ISODate)},
        {"patient", patient},
        {"diagnosis", diagnosis},
        {"drugs", serializeDrugs()},
        {"advice", advice}
    };
    return root;
}

// === 新增：从当前 UI 直接生成购物车条目 ===
QJsonArray PrescriptionWidget::cartFromCurrentSelection() const
{
    QJsonArray cart;
    for (auto *r : m_drugRows) {
        const QString name = r->name->currentText().trimmed();
        const auto medIt = m_medsByName.constFind(name);
        QJsonObject med = (medIt == m_medsByName.constEnd()) ? QJsonObject() : medIt.value();

        // 兼容不同键名的价格字段：price / unit_price / amount
        double unitPrice = 0.0;
        if (!med.isEmpty()) {
            unitPrice = med.value("price").toDouble(
                        med.value("unit_price").toDouble(
                        med.value("amount").toDouble(0.0)));
        }

        QJsonObject item{
            {"medicine_id", med.value("id").toInt(-1)},
            {"name",        name},
            {"spec",        r->spec->text()},
            {"qty",         r->quantity->text().toInt()},
            {"dosage",      r->dosage->text()},
            {"days",        r->days->text().toInt()},
            {"amount",      unitPrice}
        };
        cart.append(item);
    }
    return cart;
}
