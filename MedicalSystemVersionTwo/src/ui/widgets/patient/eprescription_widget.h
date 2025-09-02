#ifndef EPRESCRIPTION_WIDGET_H
#define EPRESCRIPTION_WIDGET_H

#include <QWidget>
#include <QList>

// 前置声明，减少头文件依赖
class QVBoxLayout;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QSpinBox;
class QLabel;

class EPrescriptionWidget : public QWidget
{
Q_OBJECT

public:
    explicit EPrescriptionWidget(QWidget *parent = nullptr);
    ~EPrescriptionWidget();

signals:
    // 返回主界面的信号
    void backRequested();

private slots:
    // 只保留 onAddDrugClicked，因为删除操作已由 Lambda 实现
    void onAddDrugClicked();

private:
    void initUI();
    void applyStyles();

    // UI 创建辅助函数
    QWidget* createHeader();
    QWidget* createPatientInfoPanel();
    QWidget* createDiagnosisPanel();
    QWidget* createDrugsPanel();
    QWidget* createAdvicePanel();
    QWidget* createBottomButtons();
    QWidget* createSectionHeader(const QString &svgIcon, const QString &title);

    // 动态添加和移除药品行
    void addDrugRow(bool isFirstRow = false);
    void removeDrugRow(QWidget* rowWidget);

    // 指向药品行布局的指针
    QVBoxLayout *drugRowsLayout;

    // 存储所有药品行的列表，便于管理
    QList<QWidget*> drugRowWidgets;

    // 其他主要UI组件的指针
    QLineEdit *mainDiagnosisEdit;
    QComboBox *departmentCombo;
    QTextEdit *medicationGuidanceEdit;
    QTextEdit *precautionsEdit;
};

#endif // EPRESCRIPTION_WIDGET_H