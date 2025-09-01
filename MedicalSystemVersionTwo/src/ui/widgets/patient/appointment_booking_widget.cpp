#include "appointment_booking_widget.h"
#include "appointment_dialog.h" // 包含弹窗头文件
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QVariant>
#include <QFormLayout>
#include <QMessageBox> // 新增：用于显示医生详情
#include <QDate>       // 新增：用于获取当前日期

AppointmentBookingWidget::AppointmentBookingWidget(QWidget *parent)
        : QWidget(parent), doctorListLayout(nullptr)
{
    setObjectName("AppointmentBookingWidget");
    initUI();
    initStyleSheets();
    loadAvailableDoctors(); // 初始加载一次医生列表
}

AppointmentBookingWidget::~AppointmentBookingWidget() {}

void AppointmentBookingWidget::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 整体无边距
    mainLayout->setSpacing(0);

    // 顶部全局标题栏
    QWidget* globalHeader = new QWidget();
    globalHeader->setObjectName("globalHeader");
    QHBoxLayout* globalHeaderLayout = new QHBoxLayout(globalHeader);
    QPushButton *backButton = new QPushButton("返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &AppointmentBookingWidget::backRequested);
    QLabel *title = new QLabel("挂号服务");
    title->setObjectName("pageTitle");
    globalHeaderLayout->addWidget(backButton);
    globalHeaderLayout->addSpacing(20);
    globalHeaderLayout->addWidget(title);
    globalHeaderLayout->addStretch();

    mainLayout->addWidget(globalHeader);

    // 内容区域
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(40, 20, 40, 40);
    contentLayout->setSpacing(30);
    contentLayout->addWidget(createFilterPanel(), 1);
    contentLayout->addWidget(createDoctorListPanel(), 3);

    mainLayout->addLayout(contentLayout);
}

// --- 后端交互函数 (伪代码实现) ---
void AppointmentBookingWidget::loadAvailableDoctors()
{
    // 1. 清空现有列表
    if (!doctorListLayout) return;
    QLayoutItem* item;
    while ((item = doctorListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 2. 模拟从后端获取的数据
    QList<DoctorInfo> doctors;
    doctors.append({
                           "王医生", "主任医师", "心血管内科", "擅长: 高血压、冠心病、心力衰竭的诊断与治疗",
                           {
                                   {"08-31 上午", "余号: 5", 5},
                                   {"09-01 下午", "余号: 3", 3},
                                   {"09-02 上午", "已约满", 0}
                           }
                   });
    doctors.append({
                           "李医生", "副主任医师", "消化内科", "擅长: 胃炎、胃溃疡、消化道出血的诊断与治疗",
                           {
                                   {"08-31 下午", "余号: 2", 2},
                                   {"09-02 上午", "余号: 4", 4},
                                   {"09-03 下午", "余号: 6", 6}
                           }
                   });
    doctors.append({
                           "张医生", "主治医师", "骨科", "擅长: 骨折、关节炎、运动损伤的诊断与治疗",
                           {
                                   {"09-01 上午", "余号: 1", 1},
                                   {"09-03 上午", "余号: 3", 3}
                           }
                   });

    // 3. 根据模拟数据创建UI
    for(const auto &doctor : doctors) {
        doctorListLayout->addWidget(createDoctorEntryWidget(doctor));
    }

    doctorListLayout->addStretch();
}

// --- UI 构建函数实现 ---

QWidget* AppointmentBookingWidget::createFilterPanel() {
    QFrame* panel = new QFrame();
    panel->setObjectName("filterPanel");
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(20);

    QLabel* title = new QLabel("筛选条件");
    title->setObjectName("filterTitle");

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    // **已修改：为下拉框添加筛选项**
    QComboBox* deptComboBox = new QComboBox();
    deptComboBox->addItems({"所有科室", "心血管内科", "消化内科", "骨科", "呼吸科"});

    QComboBox* titleComboBox = new QComboBox();
    titleComboBox->addItems({"所有职称", "主任医师", "副主任医师", "主治医师", "医师"});

    QDateEdit* dateEdit = new QDateEdit();
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    // **已修改：限制只能选择今天及以后的日期**
    dateEdit->setMinimumDate(QDate::currentDate());

    QComboBox* timePeriodComboBox = new QComboBox();
    timePeriodComboBox->addItems({"全部时段", "上午", "下午"});


    formLayout->addRow("科室", deptComboBox);
    formLayout->addRow("医生职称", titleComboBox);
    formLayout->addRow("预约日期", dateEdit);
    formLayout->addRow("时间段", timePeriodComboBox);

    QPushButton* searchButton = new QPushButton("搜索医生");
    searchButton->setObjectName("searchButton");
    connect(searchButton, &QPushButton::clicked, this, &AppointmentBookingWidget::onSearchClicked);

    layout->addWidget(title);
    layout->addLayout(formLayout);
    layout->addStretch();
    layout->addWidget(searchButton);

    return panel;
}

QWidget* AppointmentBookingWidget::createDoctorListPanel() {
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("doctorScrollArea");

    QWidget* container = new QWidget();
    doctorListLayout = new QVBoxLayout(container);
    doctorListLayout->setContentsMargins(10, 10, 10, 10);
    doctorListLayout->setSpacing(20);

    scrollArea->setWidget(container);
    return scrollArea;
}

QWidget* AppointmentBookingWidget::createDoctorEntryWidget(const DoctorInfo &doctor) {
    QFrame *entry = new QFrame();
    entry->setObjectName("doctorEntry");

    QHBoxLayout *mainLayout = new QHBoxLayout(entry);
    mainLayout->setSpacing(20);

    QLabel *avatar = new QLabel();
    avatar->setObjectName("doctorAvatar");
    avatar->setFixedSize(64, 64);

    QVBoxLayout *detailsLayout = new QVBoxLayout();
    detailsLayout->setSpacing(10);

    QHBoxLayout *line1Layout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel(doctor.name);
    nameLabel->setObjectName("doctorName");
    QLabel *titleLabel = new QLabel(doctor.title);
    titleLabel->setObjectName("doctorTitle");
    QLabel *deptTag = new QLabel(doctor.department);
    deptTag->setObjectName("departmentTag");
    line1Layout->addWidget(nameLabel);
    line1Layout->addWidget(titleLabel);
    line1Layout->addStretch();
    line1Layout->addWidget(deptTag);

    QLabel *specialtyLabel = new QLabel(doctor.specialty);
    specialtyLabel->setObjectName("doctorSpecialty");

    QGridLayout *timeSlotsLayout = new QGridLayout();
    timeSlotsLayout->setSpacing(10);
    int row = 0, col = 0;
    for (const auto &slot : doctor.timeSlots) {
        timeSlotsLayout->addWidget(createSlotWidget(slot), row, col);
        col++;
        if (col >= 4) { // 每行最多4个
            col = 0;
            row++;
        }
    }

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *bookButton = new QPushButton("立即预约");
    bookButton->setObjectName("bookNowButton");
    // 假设总是预约第一个可选时间段，实际项目中需要更复杂的逻辑
    connect(bookButton, &QPushButton::clicked, this, [this, doctor](){
        onBookNowClicked(doctor, doctor.timeSlots.isEmpty() ? "" : doctor.timeSlots[0].date);
    });

    QPushButton *detailsButton = new QPushButton("医生详情");
    detailsButton->setObjectName("detailsButton");
    // **已修改：连接到新的 onDetailsClicked 槽**
    connect(detailsButton, &QPushButton::clicked, this, [this, doctor](){
        onDetailsClicked(doctor);
    });

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(detailsButton);
    buttonsLayout->addWidget(bookButton);


    detailsLayout->addLayout(line1Layout);
    detailsLayout->addWidget(specialtyLabel);
    detailsLayout->addLayout(timeSlotsLayout);
    detailsLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(avatar);
    mainLayout->addLayout(detailsLayout);

    return entry;
}

QWidget* AppointmentBookingWidget::createSlotWidget(const DoctorInfo::AppointmentSlot &slot) {
    QFrame *widget = new QFrame();
    widget->setObjectName("slotWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(15, 10, 15, 10);

    QLabel *dateLabel = new QLabel(slot.date);
    dateLabel->setObjectName("slotDate");
    QLabel *remainingLabel = new QLabel(slot.statusText);
    remainingLabel->setObjectName("slotRemaining");

    if (slot.remaining == 0) {
        widget->setProperty("full", true);
        remainingLabel->setProperty("full", true);
    }

    layout->addWidget(dateLabel, 0, Qt::AlignCenter);
    layout->addWidget(remainingLabel, 0, Qt::AlignCenter);

    return widget;
}


// --- 槽函数实现 ---
void AppointmentBookingWidget::onSearchClicked() {
    // 实际项目中，这里会收集筛选条件并发送给后端
    loadAvailableDoctors();
}

void AppointmentBookingWidget::onBookNowClicked(const DoctorInfo &doctor, const QString &timeSlot) {
    // 弹出现有预约对话框
    AppointmentDialog dialog(doctor.name, doctor.title, doctor.department, timeSlot, "¥ 25.00", this);
    if(dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "预约成功", "您已成功预约，详情请在“我的预约”中查看。");
        // 预约成功后，刷新医生列表以更新“余号”
        loadAvailableDoctors();
    }
}

// **新增：实现医生详情弹窗的槽函数**
void AppointmentBookingWidget::onDetailsClicked(const DoctorInfo &doctor) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("医生详情");
    msgBox.setIcon(QMessageBox::Information);

    QString details = QString(
            "<b>姓名:</b> %1<br><br>"
            "<b>职称:</b> %2<br><br>"
            "<b>科室:</b> %3<br><br>"
            "<b>擅长领域:</b><p>%4</p>"
    ).arg(doctor.name, doctor.title, doctor.department, doctor.specialty);

    msgBox.setText(details);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


void AppointmentBookingWidget::initStyleSheets() {
    this->setStyleSheet(R"(
        #AppointmentBookingWidget, #doctorScrollArea { background-color: #F7FAFC; }
        #globalHeader { background-color: white; padding: 10px 40px; border-bottom: 1px solid #E2E8F0; }
        #pageTitle { font-size: 20px; font-weight: bold; color: #2D3748; }
        #backButton { font-size: 14px; color: #4A5568; border: none; background: transparent; padding: 8px; }
        #backButton:hover { background-color: #F1F5F9; border-radius: 6px; }
        #filterPanel {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
            max-width: 320px;
        }
        #filterTitle { font-size: 18px; font-weight: bold; margin-bottom: 10px; color: #2D3748; }
        QFormLayout > QLabel { font-size: 14px; color: #4A5568; }
        QComboBox, QDateEdit { border: 1px solid #CBD5E0; border-radius: 6px; padding: 8px; }
        #searchButton { background-color: #3182CE; color: white; border: none; border-radius: 8px; padding: 12px; font-weight: bold; font-size: 15px; }
        #searchButton:hover { background-color: #2B6CB0; }
        #doctorEntry {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
            padding: 20px;
        }
        #doctorEntry:hover { border-color: #3182CE; }
        #doctorAvatar { background-color: #A0AEC0; border-radius: 32px; /* In real app, use: border-image */ }
        #doctorName { font-size: 18px; font-weight: bold; color: #1A202C; }
        #doctorTitle { font-size: 14px; color: #718096; margin-left: 10px; }
        #departmentTag {
            font-size: 12px; color: #3182CE; background-color: #EBF8FF;
            padding: 3px 8px; border-radius: 6px; font-weight: 500;
        }
        #doctorSpecialty { font-size: 14px; color: #4A5568; }
        #slotWidget { background-color: #F7FAFC; border: 1px solid #E2E8F0; border-radius: 8px; cursor: pointer; }
        #slotWidget:hover { background-color: #EBF8FF; border-color: #3182CE; }
        #slotWidget[full="true"] { background-color: #F1F5F9; }
        #slotWidget[full="true"]:hover { background-color: #F1F5F9; border-color: #E2E8F0; cursor: default; }
        #slotDate { font-size: 14px; font-weight: bold; color: #2D3748; }
        #slotRemaining { font-size: 13px; color: #38A169; }
        #slotRemaining[full="true"] { color: #A0AEC0; }
        #bookNowButton { background-color: #3182CE; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-size: 14px; font-weight: bold; }
        #bookNowButton:hover { background-color: #2B6CB0; }
        #detailsButton { background-color: transparent; color: #4A5568; border: 1px solid #CBD5E0; border-radius: 8px; padding: 8px 16px; font-size: 14px; }
        #detailsButton:hover { background-color: #F7FAFC; border-color: #4A5568; }
    )");
}