#ifndef MEDICINE_SEARCH_WIDGET_H
#define MEDICINE_SEARCH_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QTableWidget>
class Widget;

struct Medicine {
    QString name;
    QString description;
    QString type;        // "处方药" 或 "非处方药"
    double price;
    QString specifications;
    QString manufacturer;
    QString effects;
    QString dosage;
    QString iconColor;   // 药品图标颜色
    bool isPrescription; // true为处方药，false为非处方药
};

class MedicineDetailDialog : public QDialog
{
Q_OBJECT

public:
    explicit MedicineDetailDialog(const Medicine &medicine, QWidget *parent = nullptr);

signals:
    void purchaseRequested(const QString &medicineName);

private:
    void setupUI(const Medicine &medicine);
    void initStyles();
};

class MedicineSearchWidget : public QWidget
{
Q_OBJECT

public:
    explicit MedicineSearchWidget(Widget *api, int patientId, QWidget *parent = nullptr);
    ~MedicineSearchWidget();

signals:
    void backRequested();

private slots:
    void onSearchClicked();
    void onCategoryChanged();
    void onDetailClicked(const Medicine &medicine);
    void onPurchaseClicked(const Medicine &medicine);
    void onBatchPurchaseClicked();
    // 新增：API回调
    void onEnsureOrderReady(int orderId);
    void onOrderItemAdded(bool ok, const QString &msg, const QJsonObject &order);
    void onMedicinesLoaded(const QJsonArray &items); // 若你要用后端列表替代本地
private:
    void initUI();
    void initStyleSheets();
    void loadMedicineData();
    void updateMedicineList();
    void updateSelectedCount();

    // UI创建函数
    QWidget* createSearchPanel();
    QWidget* createMedicineListPanel();
    QWidget* createMedicineCard(const Medicine &medicine);
    QWidget* createBottomPanel();

    // 过滤函数
    QList<Medicine> getFilteredMedicines();

private:
    QLineEdit *searchInput;
    QButtonGroup *categoryGroup;
    QPushButton *allButton;
    QPushButton *prescriptionButton;
    QPushButton *nonPrescriptionButton;

    QScrollArea *medicineScrollArea;
    QVBoxLayout *medicineListLayout;

    QLabel *selectedCountLabel;
    QPushButton *batchPurchaseButton;

    QList<Medicine> allMedicines;
    QList<QCheckBox*> medicineCheckboxes;
    QString currentCategory;

    // 底部提示栏
    QFrame *warningFrame;
    Widget *m_api = nullptr;
    int     m_patientId = -1;
    int     m_orderId = -1;
};

#endif // MEDICINE_SEARCH_WIDGET_H
