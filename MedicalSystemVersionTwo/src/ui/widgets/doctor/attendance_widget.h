#ifndef ATTENDANCE_WIDGET_H
#define ATTENDANCE_WIDGET_H

#include <QWidget>

class QLabel;
class QTimer;

class AttendanceWidget : public QWidget
{
Q_OBJECT

public:
    explicit AttendanceWidget(QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void updateClock();

private:
    void initUI();
    void applyStyles();

    // 模块化构建函数
    QWidget* createHeader();
    QWidget* createStatusBanner();
    QWidget* createClockInPanel();
    QWidget* createLeaveApplicationPanel();
    QWidget* createLeaveRecordsPanel();

    QTimer *clockTimer;
    QLabel *clockTimeLabel;
    QLabel *clockDateLabel;
};

#endif // ATTENDANCE_WIDGET_H
