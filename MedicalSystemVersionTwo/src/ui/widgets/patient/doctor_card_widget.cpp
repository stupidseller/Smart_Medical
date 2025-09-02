#include "doctor_card_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSvgWidget>
#include <QMouseEvent>
#include <QFontInfo>
// SVG Icon (可以考虑也放入一个公共文件中)
static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24" fill="#409EFF" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";

DoctorCardWidget::DoctorCardWidget(const SharedTypes::DoctorInfo &info, QWidget *parent)
         : QFrame(parent), m_doctorId(info.id)
 {
    setObjectName("doctorCard");
    setCursor(Qt::PointingHandCursor);

    auto layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignCenter);

    auto iconBg = new QFrame(this);
    iconBg->setObjectName("doctorIconBg");
    iconBg->setFixedSize(56, 56);
    auto iconLayout = new QHBoxLayout(iconBg);
    auto icon = new QSvgWidget();
    icon->load(QByteArray(userIconSvg));
    icon->setFixedSize(32, 32);
    iconLayout->addWidget(icon, 0, Qt::AlignCenter);

    auto nameLabel = new QLabel(info.name);
    nameLabel->setObjectName("doctorNameLabel");
    nameLabel->setAlignment(Qt::AlignCenter);

    auto deptLabel = new QLabel(info.department);
    deptLabel->setObjectName("doctorDeptLabel");
    deptLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(iconBg);
    layout->addWidget(nameLabel);
    layout->addWidget(deptLabel);
}

QString DoctorCardWidget::doctorId() const
{
    return m_doctorId;
}

void DoctorCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_doctorId);
    }
    QFrame::mousePressEvent(event);
}
