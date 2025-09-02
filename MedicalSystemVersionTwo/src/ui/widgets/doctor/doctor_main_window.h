#ifndef DOCTOR_MAIN_WINDOW_H
#define DOCTOR_MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>

class Widget;                 // 前置声明
class QLabel;
class QTimer;
class QStackedWidget;
class QPushButton;            // ★ 新增
class DoctorProfileWidget;
class AttendanceWidget;
class PatientManagementWidget;

class DoctorMainWindow : public QMainWindow {   // ★ 基类改为 QMainWindow
    Q_OBJECT
public:
    explicit DoctorMainWindow(Widget* api, int doctorId, const QString& name, QWidget *parent = nullptr);
    ~DoctorMainWindow();

signals:
    void logoutRequested();

private slots:
    void updateClock();
    void showProfilePage();
    void showDashboardPage();
    void showAttendancePage();
    void showPatientManagementPage();

private:
    void initUI();
    void applyStyles();

    QWidget*     createHeaderWidget();
    QWidget*     createGridWidget();
    QWidget*     createFooterWidget();
    QPushButton* createDashboardButton(const QString &svgIconData,
                                       const QString &title,
                                       const QString &subtitle,
                                       const QString &objectName);

    // ---- 成员 ----
    Widget* api_ = nullptr;
    int     doctorId_ = -1;
    QString name_;

    QStackedWidget *centralStack = nullptr;
    QWidget *dashboardPage = nullptr;
    DoctorProfileWidget *profilePage = nullptr;
    PatientManagementWidget *patientManagementPage = nullptr;
    AttendanceWidget *attendancePage = nullptr;

    QLabel *timeLabel = nullptr;
    QLabel *dateLabel = nullptr;
    QTimer *timer = nullptr;
};

#endif // DOCTOR_MAIN_WINDOW_H
