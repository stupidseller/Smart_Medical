#ifndef MEDICAL_ORDERS_WIDGET_H
#define MEDICAL_ORDERS_WIDGET_H

#include <QWidget>

// 前置声明
class QLabel;
class QTableWidget;

class MedicalOrdersWidget : public QWidget
{
Q_OBJECT

public:
    explicit MedicalOrdersWidget(QWidget *parent = nullptr);
    ~MedicalOrdersWidget();

signals:
    // 用于通知主窗口返回仪表盘
    void backRequested();

private:
    // --- UI 构建函数 ---
    void initUI();
    void initStyleSheets();
    QWidget* createHeader();
    QWidget* createPatientInfoPanel();
    QWidget* createOrdersTablePanel();
    QWidget* createFooterPanel();

    // --- 数据加载 ---
    void loadData(); // 模拟加载医嘱数据

    // --- UI 控件成员变量 ---
    // 患者信息
    QLabel* nameLabel;
    QLabel* sexLabel;
    QLabel* ageLabel;
    QLabel* recordIdLabel;
    QLabel* departmentLabel;
    QLabel* bedNumberLabel;
    QLabel* admissionDateLabel;
    QLabel* diagnosisLabel;

    // 医嘱表格
    QTableWidget* ordersTable;
};

#endif // MEDICAL_ORDERS_WIDGET_H