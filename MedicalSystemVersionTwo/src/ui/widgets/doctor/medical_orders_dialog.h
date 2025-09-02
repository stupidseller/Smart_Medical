#pragma once
#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class MedicalOrdersDialog : public QDialog {
    Q_OBJECT
public:
    explicit MedicalOrdersDialog(int patientId,
                                 const QString &patientName,
                                 QWidget *parent = nullptr);
    // 可选：如果你从某条历史医嘱进来，带上 orderId 以直达该单
    explicit MedicalOrdersDialog(int patientId,
                                 int orderId,
                                 const QString &patientName,
                                 QWidget *parent = nullptr);

signals:
    // —— UI → 上层(Main 转给 Widget) —— //
    void requestLoadOrders(int patientId, int orderId /*=0*/);
    void requestSaveOrders(const QJsonObject &orderPayload);

public slots:
    // —— Widget → UI —— //
    void onOrdersLoaded(const QJsonObject &orderHeader,
                        const QJsonArray  &items,
                        bool found);
    void onOrdersSaved(bool ok, int orderId, const QString &msg);

protected:
    void showEvent(QShowEvent *e) override;

private slots:
    void onAddRow();
    void onRemoveRow();
    void onRefresh();
    void onSave();

private:
    // UI
    void buildUi();
    void applyStyles();
    void fillHeader(const QJsonObject &o);
    void fillItems(const QJsonArray &arr);
    void clearTable();
    void recalcTotal();

    // 序列化/反序列化
    QJsonObject buildPayload() const; // 生成提交给 Widget::saveMedicalOrders 的 JSON

    // 小工具
    static QString asString(const QJsonObject &o, const char *key);
    static double  asNumber(const QJsonObject &o, const char *key, double def = 0.0);

private:
    // 标识
    int         patientId_ = 0;
    int         orderId_   = 0;   // 服务器侧的医嘱单 ID（保存成功后会更新）
    QString     patientName_;

    // 顶部信息
    QLabel     *lblPatient_ = nullptr;  // “患者：xxx(ID:xxx)”
    QLabel     *lblOrderId_ = nullptr;  // “医嘱单：#1234 / 新建”
    QLineEdit  *edtDept_    = nullptr;  // 科室（可选）
    QLineEdit  *edtDoctor_  = nullptr;  // 开立医生（可选）
    QLineEdit  *edtNote_    = nullptr;  // 备注（单据级）

    // 列表
    QTableWidget *tbl_ = nullptr;

    // 底栏
    QLabel     *lblTotal_   = nullptr;
    QPushButton *btnAdd_    = nullptr;
    QPushButton *btnRemove_ = nullptr;
    QPushButton *btnRefresh_= nullptr;
    QPushButton *btnSave_   = nullptr;
    QPushButton *btnClose_  = nullptr;
};
