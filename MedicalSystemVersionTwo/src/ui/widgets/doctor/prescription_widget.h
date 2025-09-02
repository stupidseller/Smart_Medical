#ifndef PRESCRIPTION_WIDGET_H
#define PRESCRIPTION_WIDGET_H

#include <QWidget>

// 前置声明
class QVBoxLayout;
class QScrollArea;

class PrescriptionWidget : public QWidget
{
Q_OBJECT

public:
    explicit PrescriptionWidget(QWidget *parent = nullptr);
    ~PrescriptionWidget();

signals:
    void backRequested();

private slots:
    void onAddDrugClicked();
    void onRemoveDrugClicked();

private:
    void initUI();
    void applyStyles();

    QWidget* createHeader();
    QWidget* createPatientSelection();
    QWidget* createDiagnosisSection();
    QWidget* createDrugsSection();
    QWidget* createAdviceSection();
    QWidget* createFooterButtons();

    QWidget* createFormRow(const QString& labelText, QWidget* field);
    QWidget* createDrugRow(); // 创建一个新的药品信息行

    QVBoxLayout* m_drugsLayout; // 用于动态添加/删除药品行的布局
};

#endif // PRESCRIPTION_WIDGET_H