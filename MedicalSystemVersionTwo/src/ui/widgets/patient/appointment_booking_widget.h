#ifndef APPOINTMENT_BOOKING_WIDGET_H
#define APPOINTMENT_BOOKING_WIDGET_H

#include <QWidget>

// 前置声明
class QVBoxLayout;

// 将 DoctorInfo 结构体移动到头文件中，以便在槽函数中作为参数使用
struct DoctorInfo {
    QString name;
    QString title;
    QString department;
    QString specialty;
    // 嵌套的预约时段结构体
    struct AppointmentSlot {
        QString date;
        QString statusText;
        int remaining;
    };
    QList<AppointmentSlot> timeSlots;
};


class AppointmentBookingWidget : public QWidget
{
Q_OBJECT

public:
    explicit AppointmentBookingWidget(QWidget *parent = nullptr);
    ~AppointmentBookingWidget();

signals:
    void backRequested(); // 返回首页的信号

private slots:
    void onSearchClicked(); // 点击“搜索医生”
    void onBookNowClicked(const DoctorInfo &doctor, const QString &timeSlot); // 点击“立即预约”
    // 新增：处理点击“医生详情”的槽函数
    void onDetailsClicked(const DoctorInfo &doctor);

private:
    // --- 后端交互 (伪代码) ---
    void loadAvailableDoctors();

    // --- UI 构建函数 ---
    void initUI();
    void initStyleSheets();
    QWidget* createHeader();
    QWidget* createFilterPanel();
    QWidget* createDoctorListPanel();

    QWidget* createDoctorEntryWidget(const DoctorInfo &doctor);
    QWidget* createSlotWidget(const DoctorInfo::AppointmentSlot &slot);

    // 布局，用于动态添加医生信息
    QVBoxLayout *doctorListLayout;
};

#endif // APPOINTMENT_BOOKING_WIDGET_H