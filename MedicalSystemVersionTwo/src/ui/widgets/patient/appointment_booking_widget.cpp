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

// 预约时段结构体
struct AppointmentSlot {
    QString date;       // 例如: "08-31 上午"
    QString statusText; // 例如: "余号: 5" 或 "已约满"
    int remaining;      // 例如: 5 或 0
};

struct DoctorInfo {
    QString name;
    QString title;
    QString department;
    QString specialty;
    QList<AppointmentSlot> timeSlots;  // 重命名为 timetimeSlots
};


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

    // 顶部全局标题栏（为了匹配示例图）
    QWidget* globalHeader = new QWidget();
    globalHeader->setObjectName("globalHeader");
    QHBoxLayout* globalHeaderLayout = new QHBoxLayout(globalHeader);
    QPushButton *backButton = new QPushButton("返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &AppointmentBookingWidget::backRequested);
    QLabel *title = new QLabel("挂号服务");
    title->setObjectName("pageTitle");
    // ... 此处可以添加右侧的用户头像和名字 ...
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
// 这是 appointment_booking_widget.cpp 中的一个函数
void AppointmentBookingWidget::loadAvailableDoctors()
{
    //
    // 后端交互函数：根据筛选条件获取可预约的医生列表
    //

    // 1. 清空现有列表
    if (!doctorListLayout) return;
    QLayoutItem* item;
    while ((item = doctorListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

// 修复后的 loadAvailableDoctors() 函数中的数据初始化部分
    QList<DoctorInfo> doctors;

    DoctorInfo doctor1;
    doctor1.name = "王医生";
    doctor1.title = "主任医师 | 心血管内科";
    doctor1.department = "内科";
    doctor1.specialty = "擅长: 高血压、冠心病、心力衰竭的诊断与治疗";
    doctor1.timeSlots.append(AppointmentSlot{"08-31 上午", "余号: 5", 5});
    doctor1.timeSlots.append(AppointmentSlot{"09-01 下午", "余号: 3", 3});
    doctor1.timeSlots.append(AppointmentSlot{"09-02 上午", "已约满", 0});

    DoctorInfo doctor2;
    doctor2.name = "李医生";
    doctor2.title = "副主任医师 | 消化内科";
    doctor2.department = "内科";
    doctor2.specialty = "擅长: 胃炎、胃溃疡、消化道出血的诊断与治疗";
    doctor2.timeSlots.append(AppointmentSlot{"08-31 下午", "余号: 2", 2});
    doctor2.timeSlots.append(AppointmentSlot{"09-02 上午", "余号: 4", 4});
    doctor2.timeSlots.append(AppointmentSlot{"09-03 下午", "余号: 6", 6});

    DoctorInfo doctor3;
    doctor3.name = "张医生";
    doctor3.title = "主治医师 | 骨科";
    doctor3.department = "外科";
    doctor3.specialty = "擅长: 骨折、关节炎、运动损伤的诊断与治疗";
    doctor3.timeSlots.append(AppointmentSlot{"09-01 上午", "余号: 1", 1});
    doctor3.timeSlots.append(AppointmentSlot{"09-03 上午", "余号: 3", 3});

    doctors.append(doctor1);
    doctors.append(doctor2);
    doctors.append(doctor3);

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
    formLayout->addRow("科室", new QComboBox());
    formLayout->addRow("医生职称", new QComboBox());
    formLayout->addRow("预约日期", new QDateEdit());
    formLayout->addRow("时间段", new QComboBox());

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

    // 左侧医生头像
    QLabel *avatar = new QLabel();
    avatar->setObjectName("doctorAvatar");
    avatar->setFixedSize(64, 64);

    // 右侧详细信息
    QVBoxLayout *detailsLayout = new QVBoxLayout();
    detailsLayout->setSpacing(10);

    // 信息第一行: 名字、职称、科室标签
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

    // 信息第二行: 擅长
    QLabel *specialtyLabel = new QLabel(doctor.specialty);
    specialtyLabel->setObjectName("doctorSpecialty");

    // 信息第三行: 可预约时间网格
    QGridLayout *timeSlotsLayout = new QGridLayout();
    timeSlotsLayout->setSpacing(10);
    int row = 0, col = 0;
    for (const auto &slot : doctor.timeSlots) {
        timeSlotsLayout->addWidget(createSlotWidget(slot), row, col);
        col++;
        if (col >= 3) { // 每行最多3个
            col = 0;
            row++;
        }
    }

    // 信息第四行: 操作按钮
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *bookButton = new QPushButton("立即预约");
    bookButton->setObjectName("bookNowButton");
    connect(bookButton, &QPushButton::clicked, this, [this, doctor](){
        onBookNowClicked(doctor.name, doctor.timeSlots[0].date); // 传递示例信息
    });
    QPushButton *detailsButton = new QPushButton("医生详情");
    detailsButton->setObjectName("detailsButton");
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(bookButton);
    buttonsLayout->addWidget(detailsButton);

    detailsLayout->addLayout(line1Layout);
    detailsLayout->addWidget(specialtyLabel);
    detailsLayout->addLayout(timeSlotsLayout);
    detailsLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(avatar);
    mainLayout->addLayout(detailsLayout);

    return entry;
}

QWidget* AppointmentBookingWidget::createSlotWidget(const AppointmentSlot &slot) {
    QFrame *widget = new QFrame();
    widget->setObjectName("slotWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(15, 10, 15, 10);

    QLabel *dateLabel = new QLabel(slot.date);
    dateLabel->setObjectName("slotDate");

    // 关键修正：使用正确的成员变量名 `statusText`
    QLabel *remainingLabel = new QLabel(slot.statusText);
    remainingLabel->setObjectName("slotRemaining");

    // 如果已约满，设置特殊属性用于QSS识别
    if (slot.remaining == 0) {
        remainingLabel->setProperty("full", true);
    }

    layout->addWidget(dateLabel);
    layout->addWidget(remainingLabel);

    return widget;
}


// --- 槽函数实现 ---
void AppointmentBookingWidget::onSearchClicked() {
    loadAvailableDoctors();
}

void AppointmentBookingWidget::onBookNowClicked(const QString &doctorName, const QString &timeSlot) {
    AppointmentDialog dialog(doctorName, "主任医师", "心血管内科", timeSlot, "¥ 25.00", this);
    if(dialog.exec() == QDialog::Accepted) {
        // 预约成功后，刷新医生列表以更新“余号”
        loadAvailableDoctors();
    }
}


void AppointmentBookingWidget::initStyleSheets() {
    this->setStyleSheet(R"(
        #AppointmentBookingWidget, #doctorScrollArea { background-color: #F0F4F8; }
        #globalHeader { background-color: white; padding: 10px 40px; }
        #pageTitle { font-size: 20px; font-weight: bold; color: #2D3748; }
        #backButton { font-size: 14px; color: #4A5568; border: none; background: transparent; }
        #filterPanel {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
            max-width: 300px;
        }
        #filterTitle { font-size: 18px; font-weight: bold; margin-bottom: 10px; color: #2D3748; }
        QFormLayout > QLabel { font-size: 14px; color: #4A5568; }
        #searchButton { background-color: #38A169; color: white; border: none; border-radius: 8px; padding: 12px; font-weight: bold; font-size: 15px; }
        #searchButton:hover { background-color: #2F855A; }
        #doctorEntry {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
            padding: 20px;
        }
        #doctorAvatar { background-color: #3182CE; border-radius: 32px; }
        .doctorName { font-size: 18px; font-weight: bold; color: #1A202C; }
        .doctorTitle { font-size: 14px; color: #718096; margin-left: 10px; }
        #departmentTag {
            font-size: 12px; color: #3182CE; background-color: #EBF8FF;
            padding: 3px 8px; border-radius: 6px;
        }
        .doctorSpecialty { font-size: 14px; color: #4A5568; }
        #slotWidget { background-color: #F7FAFC; border: 1px solid #E2E8F0; border-radius: 8px; }
        QLabel#slotDate       { font-size: 14px; font-weight: bold; color: #2D3748; }
        QLabel#slotRemaining  { font-size: 13px; color: #4A5568; }
        QLabel#slotRemaining[full="true"] { color: #E53E3E; font-weight: bold; }
        #bookNowButton { background-color: #3182CE; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-size: 14px; }
        #bookNowButton:hover { background-color: #2B6CB0; }
        #detailsButton { background-color: transparent; color: #3182CE; border: 1px solid #3182CE; border-radius: 8px; padding: 8px 16px; font-size: 14px; }
        #detailsButton:hover { background-color: #EBF8FF; }
    )");
}
