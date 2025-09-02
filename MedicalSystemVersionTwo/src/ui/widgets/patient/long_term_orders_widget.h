#ifndef LONG_TERM_ORDERS_WIDGET_H
#define LONG_TERM_ORDERS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QTableWidget>
#include <QTableWidgetItem>

// 长期医嘱数据结构
struct LongTermOrderData {
    // 基本信息
    QString patientName;         // 姓名
    QString gender;              // 性别
    QString age;                 // 年龄
    QString recordNumber;        // 病历号
    QString department;          // 科室
    QString bedNumber;           // 床号
    QString admissionDate;       // 入院日期
    QString diagnosis;           // 诊断
};

// 医嘱条目结构
struct MedicalOrderItem {
    QString date;                // 日期
    QString time;                // 时间
    QString orderContent;        // 医嘱内容
    QString doctorName;          // 医生
    QString signature;           // 签名
    bool isActive;               // 是否有效
};

class LongTermOrdersWidget : public QWidget
{
Q_OBJECT

public:
    explicit LongTermOrdersWidget(QWidget *parent = nullptr);
    void setOrderData(const LongTermOrderData &data);

signals:
    void backRequested();

private slots:
    void onBackButtonClicked();
    void onPrintOrdersClicked();
    void onExportOrdersClicked();

private:
    void initUI();
    void setupData();
    void applyStyles();

    // UI创建方法
    QWidget* createHeader();
    QWidget* createOrderHeader();
    QWidget* createPatientInfoTable();
    QWidget* createOrdersTable();
    QWidget* createNotesSection();
    QWidget* createActionButtons();

    // 工具方法
    QFrame* createTableCell(const QString &text, bool isHeader = false, int span = 1);
    void populateOrdersTable();

    // 数据成员
    LongTermOrderData orderData;
    QList<MedicalOrderItem> ordersList;

    // UI控件
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QTableWidget *ordersTable;
    QPushButton *printBtn;
    QPushButton *exportBtn;

    // 患者信息标签
    QLabel *nameLabel;
    QLabel *genderLabel;
    QLabel *ageLabel;
    QLabel *recordLabel;
    QLabel *deptLabel;
    QLabel *bedLabel;
    QLabel *dateLabel;
    QLabel *diagnosisLabel;
};

#endif // LONG_TERM_ORDERS_WIDGET_H