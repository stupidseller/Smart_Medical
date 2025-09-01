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

struct Medicine {
    QString name;
    QString description;
    QString type;
    bool    isPrescription = false;
    double  price = 0.0;
    QString specifications;
    QString manufacturer;
    QString effects;
    QString dosage;
    QString iconColor;
};

class MedicineDetailDialog : public QDialog
{
Q_OBJECT

public:
    explicit MedicineDetailDialog(const Medicine &medicine, QWidget *parent = nullptr);
    // xin jia de
    explicit MedicineDetailDialog(QWidget *parent = nullptr);
    void doOnPurchaseClicked(int patientId, const QJsonArray &cart, int orderId = 0);
signals:
    void purchaseRequested(const QString &medicineName);
    void requestLoadMedicineData(); //
    void onPurchaseClicked(int patientId, const QJsonArray &cart, int orderId);
private:
    void setupUI(const Medicine &medicine);
    void initStyles();
};

class MedicineSearchWidget : public QWidget
{
Q_OBJECT

public:
    explicit MedicineSearchWidget(QWidget *parent = nullptr);
    ~MedicineSearchWidget();
    void loadMedicineData();//
    void doOnPurchaseClicked(int patientId, const QJsonArray &cart, int orderId = 0);
signals:
    void backRequested();
    void requestLoadMedicineData();//
public slots:
    void onLoadMedicineDataOk(const QJsonArray &medicines);//
    void onPurchaseClickedOk(const QJsonObject &resp);
private slots:
    void onSearchClicked();
    void onCategoryChanged();
    void onDetailClicked(const Medicine &medicine);
    void onPurchaseClicked(const Medicine &medicine);
    void onBatchPurchaseClicked();
private:
    QVector<Medicine> allMedicines; // //你可以改为自己的 Model
        void refreshUi();
private:
    void initUI();
    void initStyleSheets();
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

    QList<QCheckBox*> medicineCheckboxes;
    QString currentCategory;

    // 底部提示栏
    QFrame *warningFrame;
};

#endif // MEDICINE_SEARCH_WIDGET_H
