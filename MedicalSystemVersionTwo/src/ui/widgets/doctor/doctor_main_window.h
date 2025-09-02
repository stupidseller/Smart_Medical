#ifndef DOCTOR_MAIN_WINDOW_H
#define DOCTOR_MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>

class Widget;                 // 前置声明
class QLabel;
class QTimer;
class QStackedWidget;
class QPushButton;
class QMenu;                  // ★ 前置声明，避免头部包含开销
class QEvent;                 // ★ 前置声明
class DoctorProfileWidget;
class AttendanceWidget;
class PatientManagementWidget;
class CommunicationWidget;    // ★ 与 .cpp 保持一致的沟通页类名（项目里已有）
class PrescriptionWidget;

class DoctorMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit DoctorMainWindow(Widget* api, int doctorId, const QString& name, QWidget *parent = nullptr);
    ~DoctorMainWindow();

signals:
    void logoutRequested();

private:
    void applyStyles();
    void showUserMenu();

private slots:
    void updateClock();
    void showProfilePage();
    void showDashboardPage();
    void showAttendancePage();
    void showPatientManagementPage();
    void showCommunicationPage();
    void showPrescriptionPage();
    void onLogoutClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void initUI();

    QWidget*     createHeaderWidget();
    QWidget*     createGridWidget();
    QWidget*     createFooterWidget();
    QPushButton* createDashboardButton(const QString &svgIconData,
                                       const QString &title,
                                       const QString &subtitle,
                                       const QString &objectName);

    // ★ 补充声明（之前 .cpp 有定义或调用）
    void openMedicalRecordDialog(int patientId, const QString &patientName);
    void openMedicalOrdersDialog(int patientId, const QString &patientName);

    // ---- 成员 ----
    Widget* api_ = nullptr;
    int     doctorId_ = -1;
    QString name_;

    QStackedWidget *centralStack = nullptr;
    QWidget *dashboardPage = nullptr;
    DoctorProfileWidget *profilePage = nullptr;
    PatientManagementWidget *patientManagementPage = nullptr;
    AttendanceWidget *attendancePage = nullptr;

    CommunicationWidget* communicationWidget{nullptr}; // ★ 统一成员名
    PrescriptionWidget*  prescriptionPage{nullptr};

    QLabel *timeLabel = nullptr;
    QLabel *dateLabel = nullptr;
    QLabel *userNameLabel = nullptr;                   // ★ 缺失成员补齐
    QTimer *timer = nullptr;

    QMenu* userMenu{nullptr};
};

#endif // DOCTOR_MAIN_WINDOW_H
