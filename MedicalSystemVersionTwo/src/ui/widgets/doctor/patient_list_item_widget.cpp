#include "patient_list_item_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSvgWidget>

// 嵌入式SVG图标
static const char* userIconGreen = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* userIconWhite = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="white" stroke="#3182CE" stroke-width="1"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";


PatientListItemWidget::PatientListItemWidget(const PatientData &data, QWidget *parent)
        : QPushButton(parent), patientData(data)
{
    this->setCheckable(true); // 设置为可检查按钮，用于实现单选效果
    this->setObjectName("patientListItem");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);

    // 我们用一个QSvgWidget来显示图标，但通过QSS来控制选中和未选中时的切换
    QSvgWidget* icon = new QSvgWidget();
    icon->setObjectName("itemIcon");
    icon->load(QByteArray(userIconGreen)); // 默认加载绿色图标
    icon->setFixedSize(32, 32);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);
    QLabel* nameLabel = new QLabel(data.name);
    nameLabel->setObjectName("itemName");

    QString details = QString("%1 | %2岁 | %3")
            .arg(data.patientId)
            .arg(data.age)
            .arg(data.department);
    QLabel* detailsLabel = new QLabel(details);
    detailsLabel->setObjectName("itemDetails");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(detailsLabel);

    layout->addWidget(icon);
    layout->addLayout(infoLayout);
}

int PatientListItemWidget::getId() const {
    return patientData.id;
}