#ifndef DOCTOR_MAIN_WINDOW_H
#define DOCTOR_MAIN_WINDOW_H

#include <QWidget>
#include <QFrame> // 新增：用于派生 UserProfileWidget

// 前置声明，减少头文件依赖
class QLabel;
class QTimer;
class QStackedWidget;
class QPushButton;
class DoctorProfileWidget;
class AttendanceWidget;
class PatientManagementWidget;

// --- 新增：自定义用户资料控件 ---
// 将用户头像、姓名和退出登录按钮封装在一个控件中，便于管理悬浮事件
class UserProfileWidget : public QFrame
{
Q_OBJECT
public:
    explicit UserProfileWidget(QWidget* parent = nullptr);

signals:
    void logoutClicked();

protected:
    // 重写事件以捕获鼠标悬停
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QPushButton* logoutButton;
};


class DoctorMainWindow : public QWidget
{
Q_OBJECT

public:
    explicit DoctorMainWindow(QWidget *parent = nullptr);
    ~DoctorMainWindow();

private slots:
    // 用于更新时钟的槽函数
    void updateClock();
    void showProfilePage();
    void showDashboardPage();
    void showAttendancePage();
    void showPatientManagementPage();
    // 新增：处理退出登录点击事件的槽函数
    void onLogoutClicked();

private:
    // 初始化UI界面
    void initUI();
    // 应用QSS样式表
    void applyStyles();

    // 模块化的UI构建函数
    QWidget* createHeaderWidget();
    QWidget* createGridWidget();
    QWidget* createFooterWidget();

    // 创建仪表盘按钮的辅助函数
    QWidget* createDashboardButton(const QString &svgIconData, const QString &title, const QString &subtitle, const QString &objectName);

    // 私有成员变量
    QStackedWidget *centralStack;
    QWidget *dashboardPage;
    DoctorProfileWidget *profilePage;
    PatientManagementWidget *patientManagementPage;
    AttendanceWidget *attendancePage;
    QLabel *timeLabel;
    QLabel *dateLabel;
    QTimer *timer;
};

#endif // DOCTOR_MAIN_WINDOW_H