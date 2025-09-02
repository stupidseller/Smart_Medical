#include "patient_list_item_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSvgWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QStyle>          // ★ 必须
#include <QStyleOption>    // （可选，但常用时一起带上）
// （图标同你现有的）
static const char* userIconGreen = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24"
viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1">
<path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path>
<circle cx="12" cy="7" r="4"></circle></svg>)";

static const char* userIconWhite = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24"
viewBox="0 0 24 24" fill="white" stroke="#3182CE" stroke-width="1">
<path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path>
<circle cx="12" cy="7" r="4"></circle></svg>)";
namespace {
inline void repolish(QWidget *w) {
    if (!w) return;
    if (auto *s = w->style()) {
        s->unpolish(w);
        s->polish(w);
    }
    w->update();
}
}
PatientListItemWidget::PatientListItemWidget(const PatientData &data, QWidget *parent)
        : QPushButton(parent), patientData(data)
{
    setCheckable(true);
    setObjectName("patientListItem");
    setCursor(Qt::PointingHandCursor);

    m_name       = data.name;
    m_patientId  = data.patientId;
    m_age        = data.age;
    m_department = data.department;

    m_isInpatient        = data.isInpatient;
    m_isMedicalInsurance = data.isMedicalInsurance;
    m_isKeyFocus         = data.isKeyFocus;

    initUI();
    applyStyles();
    updateDetails();
    rebuildTags();
    rebuildChips();

    connect(this, &QPushButton::toggled, this, &PatientListItemWidget::updateIconByChecked);
    updateIconByChecked(isChecked());

    connect(this, &QPushButton::clicked, this, [this]{
        emit itemActivated(patientData.id, m_patientId);
    });
}

void PatientListItemWidget::initUI()
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 12, 15, 12);
    layout->setSpacing(12);

    m_icon = new QSvgWidget();
    m_icon->setObjectName("itemIcon");
    m_icon->setFixedSize(32, 32);
    m_icon->load(QByteArray(userIconGreen));

    auto *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(4);
    infoLayout->setContentsMargins(0,0,0,0);

    m_nameLabel = new QLabel(m_name);
    m_nameLabel->setObjectName("itemName");

    m_detailsLabel = new QLabel();
    m_detailsLabel->setObjectName("itemDetails");

    // 标签行
    m_tagLayout = new QHBoxLayout();
    m_tagLayout->setSpacing(6);
    m_tagLayout->setContentsMargins(0,0,0,0);

    m_tagInpatient = new QLabel("住院");
    m_tagInpatient->setObjectName("tagLabel");
    m_tagMI = new QLabel("医保");
    m_tagMI->setObjectName("tagLabel");
    m_tagKey = new QLabel("重点");
    m_tagKey->setObjectName("tagLabel");

    m_tagLayout->addWidget(m_tagInpatient);
    m_tagLayout->addWidget(m_tagMI);
    m_tagLayout->addWidget(m_tagKey);
    m_tagLayout->addStretch();

    // ✅ 新增：状态芯片行
    m_chipLayout = new QHBoxLayout();
    m_chipLayout->setSpacing(6);
    m_chipLayout->setContentsMargins(0,0,0,0);
    m_chipProfile = new QLabel("档案");
    m_chipProfile->setObjectName("chip");
    m_chipRecord  = new QLabel("病历");
    m_chipRecord->setObjectName("chip");
    m_chipOrders  = new QLabel("医嘱");
    m_chipOrders->setObjectName("chip");

    m_chipLayout->addWidget(m_chipProfile);
    m_chipLayout->addWidget(m_chipRecord);
    m_chipLayout->addWidget(m_chipOrders);
    m_chipLayout->addStretch();

    infoLayout->addWidget(m_nameLabel);
    infoLayout->addWidget(m_detailsLabel);
    infoLayout->addLayout(m_tagLayout);
    infoLayout->addLayout(m_chipLayout);

    layout->addWidget(m_icon);
    layout->addLayout(infoLayout);
}

void PatientListItemWidget::applyStyles()
{
    setStyleSheet(R"(
#patientListItem {
    background: #ffffff;
    border: 1px solid #E5E7EB;
    border-radius: 10px;
    text-align: left;
}
#patientListItem:hover {
    border-color: #BFDBFE;
    background: #F8FAFF;
}
#patientListItem:checked {
    background: #2563eb;
    border-color: #1d4ed8;
}
#itemName {
    font-size: 15px;
    font-weight: 600;
    color: #1f2937;
}
#patientListItem:checked #itemName { color: #ffffff; }
#itemDetails {
    font-size: 12px;
    color: #6b7280;
}
#patientListItem:checked #itemDetails { color: #E5E7EB; }
#tagLabel {
    font-size: 11px;
    padding: 2px 8px;
    border-radius: 999px;
    background: #EEF2FF;
    color: #3730A3;
    border: 1px solid #E0E7FF;
}
#patientListItem:checked #tagLabel {
    background: rgba(255,255,255,0.18);
    color: #ffffff;
    border-color: rgba(255,255,255,0.35);
}

/* ✅ 状态芯片 */
#chip {
    font-size: 11px;
    padding: 2px 8px;
    border-radius: 6px;
    border: 1px solid #E5E7EB;
    color: #6b7280;
    background: #F3F4F6;
}
#patientListItem:checked #chip {
    border-color: rgba(255,255,255,0.35);
    color: #E5E7EB;
    background: rgba(255,255,255,0.18);
}
/* 亮起态 */
#chip[ok="1"] {
    background: #DCFCE7;
    color: #166534;
    border-color: #BBF7D0;
}
#patientListItem:checked #chip[ok="1"] {
    background: rgba(255,255,255,0.18);
    color: #ffffff;
    border-color: rgba(255,255,255,0.35);
}
)");
}

void PatientListItemWidget::updateIconByChecked(bool checked)
{
    m_icon->load(QByteArray(checked ? userIconWhite : userIconGreen));
}

void PatientListItemWidget::updateDetails()
{
    const QString detail = QString("%1 | %2岁 | %3")
            .arg(m_patientId.isEmpty() ? "-" : m_patientId)
            .arg(m_age)
            .arg(m_department.isEmpty() ? "-" : m_department);
    m_detailsLabel->setText(detail);
}

void PatientListItemWidget::rebuildTags()
{
    m_tagInpatient->setVisible(m_isInpatient);
    m_tagMI->setVisible(m_isMedicalInsurance);
    m_tagKey->setVisible(m_isKeyFocus);
}

void PatientListItemWidget::rebuildChips()
{
    setChipState(m_chipProfile, m_hasProfile, "档案✓", "档案", "患者档案");
    setChipState(m_chipRecord,  m_hasRecord,  "病历✓", "病历", "是否已有病历记录");
    setChipState(m_chipOrders,  m_hasOrders,  "医嘱✓", "医嘱", "是否已有医嘱");
}

void PatientListItemWidget::setChipState(QLabel *chip, bool ok, const QString &okText,
                                         const QString &offText, const QString &tip)
{
    chip->setProperty("ok", ok ? "1" : "0");
    chip->setText(ok ? okText : offText);
    chip->setToolTip(tip + (ok ? "（已存在）" : "（暂无）"));
    repolish(chip);

}

int PatientListItemWidget::getId() const { return patientData.id; }

QSize PatientListItemWidget::sizeHint() const { return { 360, 88 }; }

// ===================== 动态绑定（与你原来一致） =====================

void PatientListItemWidget::bindRuntimeData(const QJsonObject &o)
{
    if (o.contains("name"))         setName(o.value("name").toString());
    if (o.contains("patient_id"))   setPatientId(o.value("patient_id").toString());

    if (o.contains("age")) {
        const auto v = o.value("age");
        if (v.isDouble()) setAge(int(v.toDouble()));
        else setAge(v.toString().toInt());
    }
    if (o.contains("department"))   setDepartment(o.value("department").toString());

    if (o.contains("is_inpatient"))         setInpatient(o.value("is_inpatient").toBool());
    if (o.contains("is_medical_insurance")) setMedicalInsurance(o.value("is_medical_insurance").toBool());
    if (o.contains("is_key_focus"))         setKeyFocus(o.value("is_key_focus").toBool());
}

// ===================== 细粒度槽 =====================

void PatientListItemWidget::setName(const QString &name) {
    m_name = name; m_nameLabel->setText(m_name);
}
void PatientListItemWidget::setAge(int age) {
    m_age = age; updateDetails();
}
void PatientListItemWidget::setDepartment(const QString &dept) {
    m_department = dept; updateDetails();
}
void PatientListItemWidget::setPatientId(const QString &patientId) {
    m_patientId = patientId; patientData.patientId = patientId; updateDetails();
}
void PatientListItemWidget::setInpatient(bool v) {
    m_isInpatient = v; rebuildTags();
}
void PatientListItemWidget::setMedicalInsurance(bool v) {
    m_isMedicalInsurance = v; rebuildTags();
}
void PatientListItemWidget::setKeyFocus(bool v) {
    m_isKeyFocus = v; rebuildTags();
}

// ✅ 来自 API 的小结状态刷新
void PatientListItemWidget::applyProfileSummary(const QJsonObject &profile)
{
    // 兼容冗余键名
    if (profile.contains("age")) setAge(profile.value("age").toInt(m_age));
    if (profile.contains("department")) setDepartment(profile.value("department").toString(m_department));
    if (profile.contains("is_inpatient")) setInpatient(profile.value("is_inpatient").toBool(m_isInpatient));
    if (profile.contains("is_medical_insurance")) setMedicalInsurance(profile.value("is_medical_insurance").toBool(m_isMedicalInsurance));
    if (profile.contains("is_key_focus")) setKeyFocus(profile.value("is_key_focus").toBool(m_isKeyFocus));

    m_hasProfile = true;
    rebuildChips();
}

void PatientListItemWidget::setRecordFound(bool found)
{
    m_hasRecord = found;
    rebuildChips();
}

void PatientListItemWidget::setOrdersFound(bool found)
{
    m_hasOrders = found;
    rebuildChips();
}

// ===================== 交互：右键菜单 & 双击 =====================

void PatientListItemWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(this);
    QAction *aProfile = menu.addAction("查看档案");
    QAction *aRecord  = menu.addAction("查看病历");
    QAction *aOrders  = menu.addAction("查看医嘱");
    menu.addSeparator();
    QAction *aRefresh = menu.addAction("刷新状态");

    QAction *chosen = menu.exec(e->globalPos());
    if (!chosen) return;

    const int id = getId();
    if (chosen == aProfile) emit actionOpenProfile(id);
    else if (chosen == aRecord) emit actionOpenRecord(id);
    else if (chosen == aOrders) emit actionOpenOrders(id);
    else if (chosen == aRefresh) emit actionRefresh(id);
}

void PatientListItemWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    QPushButton::mouseDoubleClickEvent(e);
    emit actionOpenRecord(getId()); // 双击默认看病历
}
