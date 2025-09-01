#ifndef PATIENT_MAIN_WINDOW_H
#define PATIENT_MAIN_WINDOW_H
#include "appointment_booking_widget.h"
#include "doctor_info_widget.h"  // 添加这行
#include <QMainWindow>
#include <QStackedWidget>
#include "profile_widget.h"
#include "communication_widget.h"
#include "health_assessment_widget.h"
#include "medicine_search_widget.h"
#include "online_payment_widget.h"
#include <QMenu>
// 前置声明
class QLabel;
class QTimer;

class PatientMainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit PatientMainWindow(const QString &userName = "Jane", QWidget *parent = nullptr);
    ~PatientMainWindow();

signals:
    // 当用户请求登出时（例如点击"退出账号"），会发出此信号
    void logoutRequested();

protected:
    // 重写事件过滤器，用于捕获用户名的鼠标悬停事件
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateDateTime();
    void showUserMenu(); // 显示用户名下拉菜单
    void onLogoutTriggered(); // 处理"退出账号"动作
    void showProfileWidget();
    void showAppointmentBookingWidget();
    void showDoctorInfoWidget();  // 添加这行
    void showCommunicationWidget();
    void showHealthAssessmentWidget();
    void showMedicineSearchWidget();
    void showOnlinePaymentWidget();
private:
    void initUI();
    void initStyleSheets();

    QWidget* createHeaderWidget(const QString &userName);
    QWidget* createAdvicePanel();
    QWidget* createGridWidget();
    QWidget* createFooterWidget();
    QWidget* createDashboardButton(const QString &svgIconData, const QString &title, const QString &subtitle, const QString &objectName);

    QStackedWidget *mainStackedWidget; // 主堆叠窗口
    QWidget *dashboardPage; // 仪表盘页面
    ProfileWidget *profilePage; // 个人信息页面
    AppointmentBookingWidget *appointmentPage; // 挂号页面
    DoctorInfoWidget *doctorInfoPage; // 医生信息页面 - 添加这行

    // 成员变量
    QLabel *dateTimeLabel;
    HealthAssessmentWidget *healthAssessmentPage;
    MedicineSearchWidget *medicineSearchPage;
    OnlinePaymentWidget *paymentPage;
    CommunicationWidget *communicationPage;
    QTimer *timer;
    QLabel *userNameLabel; // 需要成为成员变量，以便在类中访问
    QMenu *userMenu;       // 用户名下拉菜单
};

#endif // PATIENT_MAIN_WINDOW_H