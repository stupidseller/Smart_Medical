#ifndef APPOINTMENT_BOOKING_WIDGET_H
#define APPOINTMENT_BOOKING_WIDGET_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonArray>
// 前置声明
class QVBoxLayout;
struct AppointmentSlot {
    QString label;   // 例如： "08-31 上午"
    QString tip;     // 例如： "余号: 5" 或 "已约满"
    int     remain = 0;
};

struct DoctorInfo {
    int doctorId = 0;
    QString name;
    QString title;
    QString department;
    QString specialty;
    QList<AppointmentSlot> timeSlots;
};
class AppointmentBookingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentBookingWidget(QWidget *parent = nullptr);
    ~AppointmentBookingWidget();
    //
    void loadAvailableDoctors();
signals:
    void backRequested(); // 返回仪表盘的信号
    //
    void requestLoadAvailableDoctors();

private slots:
    void onSearchClicked(); // 点击“搜索医生”
    void onBookNowClicked(const QString &doctorName, const QString &timeSlot); // 点击“立即预约”

public slots:
    //
    void onLoadAvailableDoctorsOk(const QJsonArray &doctors);

private:

    // --- UI 构建函数 ---
    void initUI();
    void initStyleSheets();
    QWidget* createHeader();
    QWidget* createFilterPanel();
    QWidget* createDoctorListPanel();

    QWidget* createDoctorEntryWidget(const struct DoctorInfo &doctor);
    QWidget* createSlotWidget(const struct AppointmentSlot &slot);
    QVector<DoctorInfo> model_;
    // 布局，用于动态添加医生信息
    QVBoxLayout *doctorListLayout;
    void refreshUi();
};

#endif // APPOINTMENT_BOOKING_WIDGET_H
