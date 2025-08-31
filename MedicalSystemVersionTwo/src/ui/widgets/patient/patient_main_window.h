#ifndef PATIENT_MAIN_WINDOW_H
#define PATIENT_MAIN_WINDOW_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QTimer>
#include <QSvgWidget>
#include <QLocale>
#include <QMouseEvent>
#include <QApplication>
#include <QAction>
#include <QEvent>
#include <QPoint>
#include <QMenu>
#include <QStackedWidget>
#include <QMainWindow>
class QLabel;
class QTimer;
class Widget;

class QLabel;
class QTimer;
class QPushButton;
class Widget;

// 只做前向声明，避免头文件互相包含引起的“看不到基类”问题
class ProfileWidget;
class AppointmentBookingWidget;
class DoctorInfoWidget;
class CommunicationWidget;
class HealthAssessmentWidget;
class MedicineSearchWidget;
class OnlinePaymentWidget;

class PatientMainWindow : public QMainWindow{
    Q_OBJECT
public:
    explicit PatientMainWindow(Widget* api, int patientId, const QString& name, QWidget* parent=nullptr);
    ~PatientMainWindow();

signals:
    void logoutRequested();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateDateTime();
    void showUserMenu();
    void onLogoutTriggered();

    void showProfileWidget();
    void showAppointmentBookingWidget();
    void showDoctorInfoWidget();
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
    QPushButton* createDashboardButton(const QString &svgIconData,
                                       const QString &title,
                                       const QString &subtitle,
                                       const QString &objectName);

private:
    Widget *api = nullptr;
    int     patientId_ = -1;
    QString userName_;

    QStackedWidget *mainStackedWidget = nullptr;
    QWidget *dashboardPage = nullptr;

    ProfileWidget *profilePage = nullptr;
    AppointmentBookingWidget *appointmentPage = nullptr;
    DoctorInfoWidget *doctorInfoPage = nullptr;
    CommunicationWidget *communicationPage = nullptr;
    HealthAssessmentWidget *healthAssessmentPage = nullptr;
    MedicineSearchWidget *medicineSearchPage = nullptr;
    OnlinePaymentWidget *paymentPage = nullptr;

    QLabel *dateTimeLabel = nullptr;
    QLabel *userNameLabel = nullptr;
    QMenu  *userMenu = nullptr;
    QTimer *timer = nullptr;
};

#endif // PATIENT_MAIN_WINDOW_H
