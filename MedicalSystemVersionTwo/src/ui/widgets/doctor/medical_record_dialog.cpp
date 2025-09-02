#include "medical_record_dialog.h"
#include <QApplication>
#include <QScreen>
#include <QScrollArea>
#include <QDateTime>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include "patient_data_types.h"

#include <QMessageBox>
#include <QSignalBlocker>

// ===== 构造 =====
MedicalRecordDialog::MedicalRecordDialog(const PatientData &patientData, QWidget *parent)
        : QDialog(parent), m_patientData(patientData)
{
    // 推断 int 型 patientId（你的 PatientData 里有 id；API 用这个 int）
    m_pidInt = m_patientData.id;

    setWindowTitle("病历记录单");
    setModal(true);

    // 窗口 80%
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int width  = int(screenGeometry.width()  * 0.8);
    int height = int(screenGeometry.height() * 0.8);
    resize(width, height);
    move((screenGeometry.width() - width) / 2, (screenGeometry.height() - height) / 2);

    initUI();
    applyStyles();
    loadPatientData(m_patientData);
}

MedicalRecordDialog::MedicalRecordDialog(int patientId, const QString &patientName, QWidget *parent)
    : MedicalRecordDialog(
          [patientId, patientName]{
              PatientData p;
              p.id   = patientId;
              p.name = patientName;
              return p;
          }(),
          parent)
{
}


// ===== UI 组装 =====
void MedicalRecordDialog::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(18);
    mainLayout->setContentsMargins(30, 25, 30, 25);

    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createPatientInfo());
    mainLayout->addWidget(createToolbar());           // ✅ 工具栏
    mainLayout->addWidget(createMainContent(), 1);

    // 底部关闭
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_closeBtn = new QPushButton("关闭");
    m_closeBtn->setObjectName("closeButton");
    m_closeBtn->setMinimumSize(100, 35);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_closeBtn);
    mainLayout->addLayout(buttonLayout);

    // 默认只读
    setEditMode(false);
    setDirty(false);

    // 文本变化 → 脏标记
    auto hookDirty = [this]{
        setDirty(true);
    };
    connect(m_chiefComplaintEdit, &QTextEdit::textChanged, this, hookDirty);
    connect(m_presentIllnessEdit, &QTextEdit::textChanged, this, hookDirty);
    connect(m_pastHistoryEdit,    &QTextEdit::textChanged, this, hookDirty);
}

QWidget* MedicalRecordDialog::createHeader()
{
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(headerWidget);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *hospitalLabel = new QLabel("智慧医院");
    hospitalLabel->setObjectName("hospitalTitle");

    QLabel *titleLabel = new QLabel("病历记录单");
    titleLabel->setObjectName("recordTitle");

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("titleLine");

    layout->addWidget(hospitalLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(line);

    return headerWidget;
}

QWidget* MedicalRecordDialog::createPatientInfo()
{
    QFrame *infoFrame = new QFrame();
    infoFrame->setObjectName("patientInfoFrame");

    QGridLayout *grid = new QGridLayout(infoFrame);
    grid->setSpacing(15);
    grid->setContentsMargins(20, 15, 20, 15);

    grid->addWidget(new QLabel("姓名"), 0, 0);
    m_nameLabel = new QLabel();
    m_nameLabel->setObjectName("dataLabel");
    grid->addWidget(m_nameLabel, 0, 1);

    grid->addWidget(new QLabel("性别"), 0, 2);
    m_genderLabel = new QLabel();
    m_genderLabel->setObjectName("dataLabel");
    grid->addWidget(m_genderLabel, 0, 3);

    grid->addWidget(new QLabel("年龄"), 0, 4);
    m_ageLabel = new QLabel();
    m_ageLabel->setObjectName("dataLabel");
    grid->addWidget(m_ageLabel, 0, 5);

    grid->addWidget(new QLabel("住院号"), 1, 0);
    m_patientIdLabel = new QLabel();
    m_patientIdLabel->setObjectName("dataLabel");
    grid->addWidget(m_patientIdLabel, 1, 1);

    grid->addWidget(new QLabel("科室"), 1, 2);
    m_deptLabel = new QLabel();
    m_deptLabel->setObjectName("dataLabel");
    grid->addWidget(m_deptLabel, 1, 3);

    grid->addWidget(new QLabel("床号"), 1, 4);
    m_bedLabel = new QLabel();
    m_bedLabel->setObjectName("dataLabel");
    grid->addWidget(m_bedLabel, 1, 5);

    grid->addWidget(new QLabel("入院日期"), 2, 0);
    m_admitDateLabel = new QLabel();
    m_admitDateLabel->setObjectName("dataLabel");
    grid->addWidget(m_admitDateLabel, 2, 1);

    grid->addWidget(new QLabel("记录日期"), 2, 2);
    m_recordDateLabel = new QLabel();
    m_recordDateLabel->setObjectName("dataLabel");
    grid->addWidget(m_recordDateLabel, 2, 3);

    grid->addWidget(new QLabel("记录时间"), 2, 4);
    m_recordTimeLabel = new QLabel();
    m_recordTimeLabel->setObjectName("dataLabel");
    grid->addWidget(m_recordTimeLabel, 2, 5);

    return infoFrame;
}

QWidget* MedicalRecordDialog::createToolbar()
{
    QWidget *bar = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(bar);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(10);

    m_editBtn = new QPushButton("编辑");
    m_saveBtn = new QPushButton("保存");
    m_refreshBtn = new QPushButton("刷新");

    m_editBtn->setMinimumWidth(90);
    m_saveBtn->setMinimumWidth(90);
    m_refreshBtn->setMinimumWidth(90);

    lay->addStretch();
    lay->addWidget(m_editBtn);
    lay->addWidget(m_saveBtn);
    lay->addWidget(m_refreshBtn);

    connect(m_editBtn, &QPushButton::clicked, this, [this]{
        setEditMode(!m_editMode);
    });

    connect(m_refreshBtn, &QPushButton::clicked, this, [this]{
        if (m_pidInt >= 0) emit requestLoadRecord(m_pidInt);
    });

    connect(m_saveBtn, &QPushButton::clicked, this, [this]{
        QJsonObject payload;
        if (!validateAndBuildPayload(payload)) return;
        emit requestSaveRecord(payload);
    });

    return bar;
}

QWidget* MedicalRecordDialog::createMainContent()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 20, 20, 20);

    // 主诉
    {
        QLabel *chiefTitle = new QLabel("主诉");
        chiefTitle->setObjectName("sectionTitle");
        QFrame *chiefLine = new QFrame();
        chiefLine->setFrameShape(QFrame::HLine);
        chiefLine->setFrameShadow(QFrame::Sunken);
        chiefLine->setObjectName("sectionLine");

        m_chiefComplaintEdit = new QTextEdit();
        m_chiefComplaintEdit->setObjectName("contentText");
        m_chiefComplaintEdit->setMaximumHeight(100);

        layout->addWidget(chiefTitle);
        layout->addWidget(chiefLine);
        layout->addWidget(m_chiefComplaintEdit);
    }

    // 现病史
    {
        QLabel *presentTitle = new QLabel("现病史");
        presentTitle->setObjectName("sectionTitle");
        QFrame *presentLine = new QFrame();
        presentLine->setFrameShape(QFrame::HLine);
        presentLine->setFrameShadow(QFrame::Sunken);
        presentLine->setObjectName("sectionLine");

        m_presentIllnessEdit = new QTextEdit();
        m_presentIllnessEdit->setObjectName("contentText");
        m_presentIllnessEdit->setMaximumHeight(140);

        layout->addWidget(presentTitle);
        layout->addWidget(presentLine);
        layout->addWidget(m_presentIllnessEdit);
    }

    // 既往史
    {
        QLabel *pastTitle = new QLabel("既往史");
        pastTitle->setObjectName("sectionTitle");
        QFrame *pastLine = new QFrame();
        pastLine->setFrameShape(QFrame::HLine);
        pastLine->setFrameShadow(QFrame::Sunken);
        pastLine->setObjectName("sectionLine");

        m_pastHistoryEdit = new QTextEdit();
        m_pastHistoryEdit->setObjectName("contentText");
        m_pastHistoryEdit->setMaximumHeight(120);

        layout->addWidget(pastTitle);
        layout->addWidget(pastLine);
        layout->addWidget(m_pastHistoryEdit);
    }

    // 诊断
    layout->addWidget(createDiagnosisSection());

    scrollArea->setWidget(contentWidget);
    return scrollArea;
}

QWidget* MedicalRecordDialog::createDiagnosisSection()
{
    QWidget *diagnosisWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(diagnosisWidget);
    layout->setSpacing(10);

    QLabel *diagnosisTitle = new QLabel("诊断");
    diagnosisTitle->setObjectName("sectionTitle");
    QFrame *diagnosisLine = new QFrame();
    diagnosisLine->setFrameShape(QFrame::HLine);
    diagnosisLine->setFrameShadow(QFrame::Sunken);
    diagnosisLine->setObjectName("sectionLine");

    QFrame *diagnosisFrame = new QFrame();
    diagnosisFrame->setObjectName("diagnosisTable");
    QGridLayout *diagnosisGrid = new QGridLayout(diagnosisFrame);
    diagnosisGrid->setSpacing(1);
    diagnosisGrid->setContentsMargins(1, 1, 1, 1);

    QLabel *typeHeader = new QLabel("诊断类型");
    typeHeader->setObjectName("tableHeader");
    QLabel *contentHeader = new QLabel("诊断内容");
    contentHeader->setObjectName("tableHeader");
    QLabel *icdHeader = new QLabel("ICD编码");
    icdHeader->setObjectName("tableHeader");

    diagnosisGrid->addWidget(typeHeader,   0, 0);
    diagnosisGrid->addWidget(contentHeader,0, 1);
    diagnosisGrid->addWidget(icdHeader,    0, 2);

    QLabel *mainType = new QLabel("主要诊断");
    mainType->setObjectName("tableCell");
    m_diagnosisLabel = new QLabel();
    m_diagnosisLabel->setObjectName("tableCell");
    m_icdCodeLabel = new QLabel();
    m_icdCodeLabel->setObjectName("tableCell");

    diagnosisGrid->addWidget(mainType,          1, 0);
    diagnosisGrid->addWidget(m_diagnosisLabel,  1, 1);
    diagnosisGrid->addWidget(m_icdCodeLabel,    1, 2);

    diagnosisGrid->setColumnStretch(0, 1);
    diagnosisGrid->setColumnStretch(1, 3);
    diagnosisGrid->setColumnStretch(2, 1);

    layout->addWidget(diagnosisTitle);
    layout->addWidget(diagnosisLine);
    layout->addWidget(diagnosisFrame);

    return diagnosisWidget;
}

void MedicalRecordDialog::loadPatientData(const PatientData &data)
{
    m_nameLabel->setText(data.name);
    if (data.age > 0) m_ageLabel->setText(QString::number(data.age) + "岁"); else m_ageLabel->clear();
    m_patientIdLabel->setText(data.patientId);
    m_deptLabel->setText(data.department);

    m_genderLabel->clear();
    m_bedLabel->clear();
    m_admitDateLabel->clear();
    m_recordDateLabel->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_recordTimeLabel->setText(QTime::currentTime().toString("HH:mm"));

    const QSignalBlocker b1(m_chiefComplaintEdit);
    const QSignalBlocker b2(m_presentIllnessEdit);
    const QSignalBlocker b3(m_pastHistoryEdit);
    m_chiefComplaintEdit->clear();
    m_presentIllnessEdit->clear();
    m_pastHistoryEdit->clear();
    m_diagnosisLabel->clear();
    m_icdCodeLabel->clear();
}

// ===== 运行期：绑定/更新 =====
void MedicalRecordDialog::bindRuntimeData(const QJsonObject &o)
{
    if (o.contains("gender"))     m_genderLabel->setText(o.value("gender").toString());
    if (o.contains("bed_no"))     m_bedLabel->setText(o.value("bed_no").toString());
    if (o.contains("admit_date")) m_admitDateLabel->setText(pickDate(o, "admit_date"));
    if (o.contains("record_date")) m_recordDateLabel->setText(pickDate(o, "record_date"));
    if (o.contains("record_time")) m_recordTimeLabel->setText(pickTime(o, "record_time"));

    {
        const QSignalBlocker b1(m_chiefComplaintEdit);
        const QSignalBlocker b2(m_presentIllnessEdit);
        const QSignalBlocker b3(m_pastHistoryEdit);

        if (o.contains("chief_complaint")) setChiefComplaint(o.value("chief_complaint").toString());
        if (o.contains("present_illness")) setPresentIllness(o.value("present_illness").toString());
        if (o.contains("past_history"))    setPastHistory(o.value("past_history").toString());
    }

    if (o.contains("diagnosis") || o.contains("icd_code")) {
        setDiagnosis(o.value("diagnosis").toString(), o.value("icd_code").toString());
    }
}

void MedicalRecordDialog::setChiefComplaint(const QString &t) { m_chiefComplaintEdit->setText(t); }
void MedicalRecordDialog::setPresentIllness(const QString &t) { m_presentIllnessEdit->setText(t); }
void MedicalRecordDialog::setPastHistory(const QString &t)    { m_pastHistoryEdit->setText(t); }
void MedicalRecordDialog::setDiagnosis(const QString &diag, const QString &icd) {
    m_diagnosisLabel->setText(diag);
    m_icdCodeLabel->setText(icd);
}
void MedicalRecordDialog::setRecordTimestamp(const QDate &date, const QTime &time) {
    if (date.isValid()) m_recordDateLabel->setText(date.toString("yyyy-MM-dd"));
    if (time.isValid()) m_recordTimeLabel->setText(time.toString("HH:mm"));
}

// ======== Widget 回执：加载/保存 ========
void MedicalRecordDialog::onRecordLoaded(const QJsonObject &record, bool found)
{
    // 记下 record_id（如果有）
    m_recordId = record.value("record_id").toInt(record.value("id").toInt(0));

    if (found) {
        bindRuntimeData(record);
        setEditMode(false);
        setDirty(false);
    } else {
        // 新建模式
        loadPatientData(m_patientData);
        setEditMode(true);
        setDirty(false);
    }
}

void MedicalRecordDialog::onRecordSaved(bool ok, int recordId, const QString &msg)
{
    if (ok) {
        m_recordId = recordId;
        setEditMode(false);
        setDirty(false);
        QMessageBox::information(this, "保存成功", "病历已保存。");
        // 保存后刷新一次
        if (m_pidInt >= 0) emit requestLoadRecord(m_pidInt);
    } else {
        QMessageBox::warning(this, "保存失败", msg.isEmpty() ? "保存失败" : msg);
    }
}

// ======== 样式 ========
void MedicalRecordDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog { background-color: white; }
        #hospitalTitle { font-size: 24px; font-weight: bold; color: #1a1a1a; margin: 10px 0; }
        #recordTitle { font-size: 20px; font-weight: 600; color: #333; margin-bottom: 10px; }
        #titleLine { background-color: #333; max-height: 2px; margin: 10px 0; }
        #patientInfoFrame { border: 2px solid #333; background-color: #fafafa; margin: 10px 0; }
        QLabel { font-size: 13px; color: #333; font-weight: 500; }
        #dataLabel { font-weight: normal; color: #555; padding: 2px 5px; }
        #sectionTitle { font-size: 16px; font-weight: bold; color: #1a1a1a; margin: 15px 0 5px 0; }
        #sectionLine { background-color: #666; max-height: 1px; margin: 5px 0 15px 0; }
        #contentText { border: 1px solid #ccc; background-color: #fdfdfd; font-size: 13px; line-height: 1.6; padding: 10px; }
        #diagnosisTable { border: 2px solid #333; background-color: white; }
        #tableHeader { background-color: #f5f5f5; border: 1px solid #333; font-weight: bold; padding: 8px; text-align: center; }
        #tableCell { border: 1px solid #333; padding: 8px; background-color: white; }
        QPushButton { height: 32px; padding: 0 12px; }
        QPushButton#closeButton { background-color: #3b82f6; color: white; border: none; border-radius: 6px; font-size: 14px; font-weight: 500; padding: 8px 16px; }
        QPushButton#closeButton:hover { background-color: #2563eb; }
    )");
}

// ======== 工具：日期/时间抓取 ========
QString MedicalRecordDialog::pickDate(const QJsonObject &o, const char *key, const char *fmt)
{
    const auto v = o.value(key);
    if (v.isString()) {
        QDateTime dt = QDateTime::fromString(v.toString(), Qt::ISODate);
        if (!dt.isValid()) {
            QDate d = QDate::fromString(v.toString(), "yyyy-MM-dd");
            if (d.isValid()) return d.toString(fmt);
        } else {
            return dt.date().toString(fmt);
        }
    }
    return {};
}

QString MedicalRecordDialog::pickTime(const QJsonObject &o, const char *key, const char *fmt)
{
    const auto v = o.value(key);
    if (v.isString()) {
        QDateTime dt = QDateTime::fromString(v.toString(), Qt::ISODate);
        if (!dt.isValid()) {
            QTime t = QTime::fromString(v.toString(), "HH:mm");
            if (t.isValid()) return t.toString(fmt);
        } else {
            return dt.time().toString(fmt);
        }
    }
    return {};
}

// ======== 编辑/校验/打包 ========
void MedicalRecordDialog::setEditMode(bool on)
{
    m_editMode = on;
    m_editBtn->setText(on ? "退出编辑" : "编辑");
    m_saveBtn->setEnabled(on && m_dirty);

    m_chiefComplaintEdit->setReadOnly(!on);
    m_presentIllnessEdit->setReadOnly(!on);
    m_pastHistoryEdit->setReadOnly(!on);
}

void MedicalRecordDialog::setDirty(bool on)
{
    m_dirty = on;
    m_saveBtn->setEnabled(m_editMode && m_dirty);
}

bool MedicalRecordDialog::validateAndBuildPayload(QJsonObject &out) const
{
    if (m_pidInt < 0) {
        QMessageBox::warning(nullptr, "提示", "缺少 patient_id，无法保存。");
        return false;
    }
    const QString chief = m_chiefComplaintEdit->toPlainText().trimmed();
    if (chief.isEmpty()) {
        QMessageBox::warning(nullptr, "提示", "请填写主诉。");
        return false;
    }
    // 可按需增加更多校验…

    out = QJsonObject{
        {"patient_id", m_pidInt},
        {"chief_complaint", chief},
        {"present_illness", m_presentIllnessEdit->toPlainText()},
        {"past_history",    m_pastHistoryEdit->toPlainText()},
        {"diagnosis",       m_diagnosisLabel->text()},
        {"icd_code",        m_icdCodeLabel->text()},
        {"record_date",     m_recordDateLabel->text()},
        {"record_time",     m_recordTimeLabel->text()},
    };
    if (m_recordId > 0) out.insert("record_id", m_recordId); // 更新时带回
    return true;
}

// ======== 首次显示即加载 ========
void MedicalRecordDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    if (!m_firstShowLoaded && m_pidInt >= 0) {
        m_firstShowLoaded = true;
        emit requestLoadRecord(m_pidInt);
        emit refreshRequested(m_patientData.patientId); // 可选给外部用
    }
}
