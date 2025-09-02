#ifndef PATIENT_LIST_ITEM_WIDGET_H
#define PATIENT_LIST_ITEM_WIDGET_H

#include <QPushButton>
#include <QJsonObject>
#include "patient_data_types.h"

class QLabel;
class QSvgWidget;
class QHBoxLayout;
class QContextMenuEvent;

class PatientListItemWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit PatientListItemWidget(const PatientData &data, QWidget *parent = nullptr);

    int getId() const;
    QString getPatientId() const { return m_patientId; }
    const PatientData& data() const { return patientData; }

public slots:
    // —— 一次性从 JSON 绑定或增量更新（有则覆盖，无则忽略）——
    void bindRuntimeData(const QJsonObject &payload);

    // —— 细粒度更新 ——
    void setName(const QString &name);
    void setAge(int age);
    void setDepartment(const QString &dept);
    void setPatientId(const QString &patientId);
    void setInpatient(bool v);
    void setMedicalInsurance(bool v);
    void setKeyFocus(bool v);

    // ✅ 新增：来自 API 的小结状态刷新
    void applyProfileSummary(const QJsonObject &profile); // 档案到达：芯片亮起，同时可顺便补全年龄/科室等
    void setRecordFound(bool found);                      // 病历是否存在
    void setOrdersFound(bool found);                      // 医嘱是否存在

signals:
    // 已有：选中/激活
    void itemActivated(int id, const QString &patientId);

    // ✅ 新增：右键菜单/双击的动作请求（冒泡给上层）
    void actionOpenProfile(int id);
    void actionOpenRecord(int id);
    void actionOpenOrders(int id);
    void actionRefresh(int id); // 触发重新拉取（profile/record/orders）

protected:
    QSize sizeHint() const override;
    void contextMenuEvent(QContextMenuEvent *e) override;     // ✅ 右键菜单
    void mouseDoubleClickEvent(QMouseEvent *e) override;      // ✅ 双击默认打开病历

private:
    void initUI();
    void applyStyles();
    void updateDetails();
    void rebuildTags();
    void updateIconByChecked(bool checked);
    void rebuildChips();                 // ✅ 芯片外观随着状态刷新
    void setChipState(QLabel *chip, bool ok, const QString &okText, const QString &offText, const QString &tip);

private:
    PatientData patientData;

    // 基础字段（与 UI 文本同步）
    QString m_name;
    QString m_patientId;
    int     m_age = 0;
    QString m_department;

    // 运行时状态
    bool m_isInpatient = false;
    bool m_isMedicalInsurance = false;
    bool m_isKeyFocus = false;

    // “档案/病历/医嘱”状态
    bool m_hasProfile = false;
    bool m_hasRecord  = false;
    bool m_hasOrders  = false;

    // UI 组件
    QSvgWidget *m_icon = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_detailsLabel = nullptr;

    QHBoxLayout *m_tagLayout = nullptr;
    QLabel *m_tagInpatient = nullptr;
    QLabel *m_tagMI = nullptr;
    QLabel *m_tagKey = nullptr;

    // ✅ 新增芯片
    QHBoxLayout *m_chipLayout = nullptr;
    QLabel *m_chipProfile = nullptr;
    QLabel *m_chipRecord  = nullptr;
    QLabel *m_chipOrders  = nullptr;
};

#endif // PATIENT_LIST_ITEM_WIDGET_H
