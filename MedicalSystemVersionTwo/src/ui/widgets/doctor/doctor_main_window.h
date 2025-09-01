#ifndef DOCTOR_MAIN_WINDOW_H
#define DOCTOR_MAIN_WINDOW_H

#include <QWidget>

// 前置声明，减少头文件依赖
class QLabel;
class QTimer;
class QStackedWidget; // <--- 新增
class DoctorProfileWidget;
class AttendanceWidget;
class PatientManagementWidget;
class DoctorMainWindow : public QWidget

{
Q_OBJECT

public:
    explicit DoctorMainWindow(QWidget *parent = nullptr);
    ~DoctorMainWindow();

private slots:
    // 用于更新时钟的槽函数
    void updateClock();
    void showProfilePage(); // <--- 新增
    void showDashboardPage();
    void showAttendancePage();
    void showPatientManagementPage();
private:
    // 初始化UI界面
    void initUI();
    // 应用QSS样式表
    void applyStyles();

    // 模块化的UI构建函数
    QWidget* createHeaderWidget();
    QWidget* createGridWidget();
    QWidget* createFooterWidget();

    // 创建仪表盘按钮的辅助函数，借鉴了你的优秀设计
    QWidget* createDashboardButton(const QString &svgIconData, const QString &title, const QString &subtitle, const QString &objectName);
    QStackedWidget *centralStack; // <--- 新增
    QWidget *dashboardPage;       // <--- 新增
    DoctorProfileWidget *profilePage;
    PatientManagementWidget *patientManagementPage;
    // 私有成员变量
    AttendanceWidget *attendancePage;
    QLabel *timeLabel;
    QLabel *dateLabel;
    QTimer *timer;
};

#endif // DOCTOR_MAIN_WINDOW_H