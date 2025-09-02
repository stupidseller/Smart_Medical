#ifndef DOCTOR_CARD_WIDGET_H
#define DOCTOR_CARD_WIDGET_H

#include <QFrame>
#include "../common/shared_data_types.h"

class DoctorCardWidget : public QFrame
{
Q_OBJECT

public:
    explicit DoctorCardWidget(const DoctorInfo &info, QWidget *parent = nullptr);
    QString doctorId() const;

signals:
    void clicked(const QString &doctorId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_doctorId;
};

#endif // DOCTOR_CARD_WIDGET_H