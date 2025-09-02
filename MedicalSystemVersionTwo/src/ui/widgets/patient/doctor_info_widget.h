#ifndef DOCTOR_INFO_WIDGET_H
#define DOCTOR_INFO_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>   // 如果后面有 toVariant() 之类操作就留着
#include <QDebug>
struct DoctorProfile {
    QString name;
    QString title;
    QString department;
    QString specialty;
    QString schedule;   // 例如："周一至周五上午"（服务器暂无该列，先留空）
    QString experience; // 例如："25年"
    QString education;  // 例如："北京医科大学医学博士"（服务器暂无该列，先留空）
    QString awards;     // 例如："省级医学科技进步奖二等奖"（服务器暂无该列，先留空）
    int doctorId = 0;
};
class DoctorInfoWidget : public QWidget
{
Q_OBJECT

public:
    explicit DoctorInfoWidget(QWidget *parent = nullptr);
    ~DoctorInfoWidget();
    //
    void loadDoctorList();
signals:
    void backRequested();
    void requestLoadDoctorList();
public slots:
    //
    void onLoadDoctorListOk(const QJsonArray &doctors);

private slots:
    void onSearchClicked();
    void onDepartmentChanged();
    void onViewDoctorProfile(const QString &doctorName);

private:
    void initUI();
    void initStyleSheets();


    // UI创建函数
    QWidget* createSearchPanel();
    QWidget* createDoctorListPanel();
    QWidget* createDoctorCard(const QString &name, const QString &title,
                              const QString &department, const QString &specialty,
                              const QString &schedule, const QString &experience);

private:
    QVector<DoctorProfile> model_;
        void refreshUi();
    QVBoxLayout *doctorListLayout;
    QComboBox *departmentCombo;
    QLineEdit *searchEdit;
    QScrollArea *scrollArea;
};

#endif // DOCTOR_INFO_WIDGET_H
