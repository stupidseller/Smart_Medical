#include "hospitalization_info_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QSvgWidget>
#include <QSpacerItem>

// SVG图标定义
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 12H5"/><path d="M12 19l-7-7 7-7"/></svg>)";

static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";

static const char* infoIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="#3B82F6" stroke="white" stroke-width="1.5"><circle cx="12" cy="12" r="10"/><line x1="12" y1="16" x2="12" y2="12"/><line x1="12" y1="8" x2="12.01" y2="8"/></svg>)";

static const char* billIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="#10B981" stroke="white" stroke-width="1.5"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"/><line x1="16" y1="2" x2="16" y2="6"/><line x1="8" y1="2" x2="8" y2="6"/><line x1="3" y1="10" x2="21" y2="10"/></svg>)";

static const char* orderIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="#8B5CF6" stroke="white" stroke-width="1.5"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14,2 14,8 20,8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg>)";

HospitalizationInfoWidget::HospitalizationInfoWidget(QWidget *parent)
        : QWidget(parent)
{
    setupData();
    initUI();
    applyStyles();
}

void HospitalizationInfoWidget::setupData()
{
    // 设置住院信息示例数据
    hospData.patientNumber = "ZY20230831001";
    hospData.admissionDate = "2023-08-25";
    hospData.attendingDoctor = "王医生";
    hospData.wardAndBed = "内科楼 5楼 502室 3床";
    hospData.department = "心血管内科";
    hospData.diagnosis = "高血压、冠心病";

    // 费用信息
    hospData.bedFee = 800.00;
    hospData.treatmentFee = 1250.00;
    hospData.examFee = 980.00;
    hospData.medicineFee = 1560.00;
    hospData.otherFee = 320.00;
    hospData.totalFee = 4910.00;

    // 医嘱信息
    medicalOrders.clear();
    medicalOrders << MedicalOrder{"2023-08-28", "长期医嘱", "硝苯地平缓释片 30mg 口服 每日一次\n阿司匹林肠溶片 100mg 口服 每日一次\n低盐低脂饮食，监测血压", "王医生", "执行中"};
    medicalOrders << MedicalOrder{"2023-08-27", "临时医嘱", "血常规检查\n心电图检查\n心脏彩超检查", "王医生", "已执行"};
    medicalOrders << MedicalOrder{"2023-08-26", "长期医嘱", "卧床休息\n低盐饮食\n监测生命体征", "王医生", "执行中"};
}

void HospitalizationInfoWidget::initUI()
{
    this->setObjectName("hospitalizationInfoWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(20);

    // 添加各个部分
    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createCurrentStatusCard());
    mainLayout->addWidget(createInfoAndBillSection(), 1);
    mainLayout->addWidget(createMedicalOrdersCard());
}

QWidget* HospitalizationInfoWidget::createHeader()
{
    QFrame *header = new QFrame();
    header->setObjectName("headerFrame");

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);
    headerLayout->setSpacing(16);

    // 返回按钮
    QPushButton *backBtn = new QPushButton("← 返回首页");
    backBtn->setObjectName("backButton");
    backBtn->setMinimumHeight(36);
    backBtn->setMinimumWidth(120);
    connect(backBtn, &QPushButton::clicked, this, &HospitalizationInfoWidget::onBackButtonClicked);

    // 标题
    QLabel *title = new QLabel("住院信息");
    title->setObjectName("headerTitle");

    // 用户信息
    QFrame *userFrame = new QFrame();
    userFrame->setObjectName("userFrame");
    QHBoxLayout *userLayout = new QHBoxLayout(userFrame);
    userLayout->setContentsMargins(12, 8, 12, 8);
    userLayout->setSpacing(8);

    QSvgWidget *userIcon = new QSvgWidget();
    userIcon->load(QByteArray(userIconSvg));
    userIcon->setFixedSize(24, 24);

    QLabel *userName = new QLabel("张患者");
    userName->setObjectName("userName");

    userLayout->addWidget(userIcon);
    userLayout->addWidget(userName);

    headerLayout->addWidget(backBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(userFrame);

    return header;
}

QWidget* HospitalizationInfoWidget::createCurrentStatusCard()
{
    QFrame *statusCard = new QFrame();
    statusCard->setObjectName("currentStatusCard");

    QHBoxLayout *statusLayout = new QHBoxLayout(statusCard);
    statusLayout->setContentsMargins(24, 20, 24, 20);
    statusLayout->setSpacing(20);

    // 左侧信息
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(8);

    QLabel *statusTitle = new QLabel("当前住院状态");
    statusTitle->setObjectName("statusTitle");

    QGridLayout *statusGrid = new QGridLayout();
    statusGrid->setSpacing(16);
    statusGrid->setColumnStretch(1, 1);
    statusGrid->setColumnStretch(3, 1);

    // 住院号
    statusGrid->addWidget(new QLabel("住院号"), 0, 0);
    statusNumberLabel = new QLabel(hospData.patientNumber);
    statusNumberLabel->setObjectName("statusValue");
    statusGrid->addWidget(statusNumberLabel, 0, 1);

    // 入院日期
    statusGrid->addWidget(new QLabel("入院日期"), 0, 2);
    statusDateLabel = new QLabel(hospData.admissionDate);
    statusDateLabel->setObjectName("statusValue");
    statusGrid->addWidget(statusDateLabel, 0, 3);

    // 主治医生
    statusGrid->addWidget(new QLabel("主治医生"), 1, 0);
    statusDoctorLabel = new QLabel(hospData.attendingDoctor);
    statusDoctorLabel->setObjectName("statusValue");
    statusGrid->addWidget(statusDoctorLabel, 1, 1);

    // 病房
    statusGrid->addWidget(new QLabel("病房"), 1, 2);
    statusWardLabel = new QLabel(hospData.wardAndBed);
    statusWardLabel->setObjectName("statusValue");
    statusGrid->addWidget(statusWardLabel, 1, 3);

    leftLayout->addWidget(statusTitle);
    leftLayout->addLayout(statusGrid);
    leftLayout->addStretch();

    // 右侧住院中状态
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setAlignment(Qt::AlignCenter);

    inpatientStatusLabel = new QLabel("住院中");
    inpatientStatusLabel->setObjectName("inpatientStatus");

    rightLayout->addWidget(inpatientStatusLabel);

    statusLayout->addLayout(leftLayout, 1);
    statusLayout->addLayout(rightLayout);

    return statusCard;
}

QWidget* HospitalizationInfoWidget::createInfoAndBillSection()
{
    QHBoxLayout *sectionLayout = new QHBoxLayout();
    sectionLayout->setSpacing(16);

    sectionLayout->addWidget(createBasicInfoCard());
    sectionLayout->addWidget(createBillInfoCard());

    QWidget *sectionWidget = new QWidget();
    sectionWidget->setLayout(sectionLayout);
    return sectionWidget;
}

QWidget* HospitalizationInfoWidget::createBasicInfoCard()
{
    QFrame *basicCard = new QFrame();
    basicCard->setObjectName("infoCard");

    QVBoxLayout *basicLayout = new QVBoxLayout(basicCard);
    basicLayout->setContentsMargins(20, 16, 20, 16);
    basicLayout->setSpacing(16);

    // 标题
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QSvgWidget *infoIcon = new QSvgWidget();
    infoIcon->load(QByteArray(infoIconSvg));
    infoIcon->setFixedSize(20, 20);

    QLabel *title = new QLabel("住院基本信息");
    title->setObjectName("cardTitle");

    titleLayout->addWidget(infoIcon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();

    // 信息网格
    QGridLayout *infoGrid = new QGridLayout();
    infoGrid->setSpacing(12);
    infoGrid->setColumnStretch(1, 1);

    // 住院号
    infoGrid->addWidget(new QLabel("住院号"), 0, 0);
    basicNumberLabel = new QLabel(hospData.patientNumber);
    basicNumberLabel->setObjectName("infoValue");
    infoGrid->addWidget(basicNumberLabel, 0, 1);

    // 入院日期
    infoGrid->addWidget(new QLabel("入院日期"), 1, 0);
    basicDateLabel = new QLabel(hospData.admissionDate);
    basicDateLabel->setObjectName("infoValue");
    infoGrid->addWidget(basicDateLabel, 1, 1);

    // 主治医生
    infoGrid->addWidget(new QLabel("主治医生"), 2, 0);
    basicDoctorLabel = new QLabel(hospData.attendingDoctor);
    basicDoctorLabel->setObjectName("infoValue");
    infoGrid->addWidget(basicDoctorLabel, 2, 1);

    // 科室
    infoGrid->addWidget(new QLabel("科室"), 3, 0);
    basicDeptLabel = new QLabel(hospData.department);
    basicDeptLabel->setObjectName("infoValue");
    infoGrid->addWidget(basicDeptLabel, 3, 1);

    // 病房
    infoGrid->addWidget(new QLabel("病房"), 4, 0);
    basicWardLabel = new QLabel(hospData.wardAndBed);
    basicWardLabel->setObjectName("infoValue");
    infoGrid->addWidget(basicWardLabel, 4, 1);

    // 入院诊断
    infoGrid->addWidget(new QLabel("入院诊断"), 5, 0);
    basicDiagnosisLabel = new QLabel(hospData.diagnosis);
    basicDiagnosisLabel->setObjectName("infoValue");
    basicDiagnosisLabel->setWordWrap(true);
    infoGrid->addWidget(basicDiagnosisLabel, 5, 1);

    basicLayout->addLayout(titleLayout);
    basicLayout->addLayout(infoGrid);
    basicLayout->addStretch();

    return basicCard;
}

QWidget* HospitalizationInfoWidget::createBillInfoCard()
{
    QFrame *billCard = new QFrame();
    billCard->setObjectName("billCard");

    QVBoxLayout *billLayout = new QVBoxLayout(billCard);
    billLayout->setContentsMargins(20, 16, 20, 16);
    billLayout->setSpacing(16);

    // 标题和明细按钮
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QSvgWidget *billIcon = new QSvgWidget();
    billIcon->load(QByteArray(billIconSvg));
    billIcon->setFixedSize(20, 20);

    QLabel *title = new QLabel("费用信息");
    title->setObjectName("cardTitle");

    QPushButton *detailBtn = new QPushButton("明细");
    detailBtn->setObjectName("detailButton");

    titleLayout->addWidget(billIcon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    titleLayout->addWidget(detailBtn);

    // 费用列表
    QVBoxLayout *feeLayout = new QVBoxLayout();
    feeLayout->setSpacing(12);

    // 床位费
    QHBoxLayout *bedFeeLayout = new QHBoxLayout();
    bedFeeLayout->addWidget(new QLabel("床位费"));
    bedFeeLabel = new QLabel(QString("¥%1").arg(hospData.bedFee, 0, 'f', 2));
    bedFeeLabel->setObjectName("feeValue");
    bedFeeLayout->addStretch();
    bedFeeLayout->addWidget(bedFeeLabel);

    // 诊疗费
    QHBoxLayout *treatmentFeeLayout = new QHBoxLayout();
    treatmentFeeLayout->addWidget(new QLabel("诊疗费"));
    treatmentFeeLabel = new QLabel(QString("¥%1").arg(hospData.treatmentFee, 0, 'f', 2));
    treatmentFeeLabel->setObjectName("feeValue");
    treatmentFeeLayout->addStretch();
    treatmentFeeLayout->addWidget(treatmentFeeLabel);

    // 检查费
    QHBoxLayout *examFeeLayout = new QHBoxLayout();
    examFeeLayout->addWidget(new QLabel("检查费"));
    examFeeLabel = new QLabel(QString("¥%1").arg(hospData.examFee, 0, 'f', 2));
    examFeeLabel->setObjectName("feeValue");
    examFeeLayout->addStretch();
    examFeeLayout->addWidget(examFeeLabel);

    // 药品费
    QHBoxLayout *medicineFeeLayout = new QHBoxLayout();
    medicineFeeLayout->addWidget(new QLabel("药品费"));
    medicineFeeLabel = new QLabel(QString("¥%1").arg(hospData.medicineFee, 0, 'f', 2));
    medicineFeeLabel->setObjectName("feeValue");
    medicineFeeLayout->addStretch();
    medicineFeeLayout->addWidget(medicineFeeLabel);

    // 其他费用
    QHBoxLayout *otherFeeLayout = new QHBoxLayout();
    otherFeeLayout->addWidget(new QLabel("其他费用"));
    otherFeeLabel = new QLabel(QString("¥%1").arg(hospData.otherFee, 0, 'f', 2));
    otherFeeLabel->setObjectName("feeValue");
    otherFeeLayout->addStretch();
    otherFeeLayout->addWidget(otherFeeLabel);

    feeLayout->addLayout(bedFeeLayout);
    feeLayout->addLayout(treatmentFeeLayout);
    feeLayout->addLayout(examFeeLayout);
    feeLayout->addLayout(medicineFeeLayout);
    feeLayout->addLayout(otherFeeLayout);

    // 分割线
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setObjectName("separator");

    // 当前合计
    QHBoxLayout *totalLayout = new QHBoxLayout();
    QLabel *totalLabel = new QLabel("当前合计");
    totalLabel->setObjectName("totalLabel");
    totalFeeLabel = new QLabel(QString("¥%1").arg(hospData.totalFee, 0, 'f', 2));
    totalFeeLabel->setObjectName("totalValue");
    totalLayout->addWidget(totalLabel);
    totalLayout->addStretch();
    totalLayout->addWidget(totalFeeLabel);

    // 立即缴费按钮
    printBillBtn = new QPushButton("立即缴费");
    printBillBtn->setObjectName("payButton");
    connect(printBillBtn, &QPushButton::clicked, this, &HospitalizationInfoWidget::onPrintBillClicked);

    billLayout->addLayout(titleLayout);
    billLayout->addLayout(feeLayout);
    billLayout->addWidget(line);
    billLayout->addLayout(totalLayout);
    billLayout->addStretch();
    billLayout->addWidget(printBillBtn);

    return billCard;
}

QWidget* HospitalizationInfoWidget::createMedicalOrdersCard()
{
    QFrame *ordersCard = new QFrame();
    ordersCard->setObjectName("ordersCard");

    QVBoxLayout *ordersLayout = new QVBoxLayout(ordersCard);
    ordersLayout->setContentsMargins(20, 16, 20, 16);
    ordersLayout->setSpacing(16);

    // 标题和查看全部按钮
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QSvgWidget *orderIcon = new QSvgWidget();
    orderIcon->load(QByteArray(orderIconSvg));
    orderIcon->setFixedSize(20, 20);

    QLabel *title = new QLabel("医嘱信息");
    title->setObjectName("cardTitle");

    viewAllOrdersBtn = new QPushButton("全部");
    viewAllOrdersBtn->setObjectName("viewAllButton");
    connect(viewAllOrdersBtn, &QPushButton::clicked, this, &HospitalizationInfoWidget::onViewAllOrdersClicked);

    titleLayout->addWidget(orderIcon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    titleLayout->addWidget(viewAllOrdersBtn);

    // 医嘱列表
    ordersListLayout = new QVBoxLayout();
    ordersListLayout->setSpacing(12);

    // 添加医嘱项
    for (const auto &order : medicalOrders) {
        QFrame *orderItem = new QFrame();
        orderItem->setObjectName("orderItem");

        QVBoxLayout *itemLayout = new QVBoxLayout(orderItem);
        itemLayout->setContentsMargins(16, 12, 16, 12);
        itemLayout->setSpacing(8);

        // 医嘱头部
        QHBoxLayout *headerLayout = new QHBoxLayout();
        QLabel *typeLabel = new QLabel(order.type);
        typeLabel->setObjectName("orderType");

        QLabel *dateLabel = new QLabel(order.date);
        dateLabel->setObjectName("orderDate");

        headerLayout->addWidget(typeLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(dateLabel);

        // 医嘱内容
        QLabel *contentLabel = new QLabel(order.content);
        contentLabel->setObjectName("orderContent");
        contentLabel->setWordWrap(true);

        // 医生和状态
        QHBoxLayout *footerLayout = new QHBoxLayout();
        QLabel *doctorLabel = new QLabel(order.doctor);
        doctorLabel->setObjectName("orderDoctor");

        footerLayout->addStretch();
        footerLayout->addWidget(doctorLabel);

        itemLayout->addLayout(headerLayout);
        itemLayout->addWidget(contentLabel);
        itemLayout->addLayout(footerLayout);

        ordersListLayout->addWidget(orderItem);
    }

    ordersLayout->addLayout(titleLayout);
    ordersLayout->addLayout(ordersListLayout);

    return ordersCard;
}

void HospitalizationInfoWidget::onBackButtonClicked()
{
    emit backRequested();
}

void HospitalizationInfoWidget::onPrintBillClicked()
{
    // 处理缴费按钮点击
    // 这里可以添加缴费相关的逻辑
}

void HospitalizationInfoWidget::onViewAllOrdersClicked()
{
    // 处理查看全部医嘱按钮点击
    // 这里可以添加显示所有医嘱的逻辑
}

void HospitalizationInfoWidget::applyStyles()
{
    this->setStyleSheet(R"(
        #hospitalizationInfoWidget {
            background-color: #f0f9ff;
        }

        /* 头部样式 */
        #headerFrame {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
        }
        #backButton {
            background-color: transparent;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            color: #64748b;
            font-size: 14px;
            font-weight: 500;
            padding: 8px 16px;
            text-align: left;
        }
        #backButton:hover {
            background-color: #f1f5f9;
            border-color: #cbd5e1;
        }
        #headerTitle {
            font-size: 20px;
            font-weight: bold;
            color: #1e293b;
        }
        #userFrame {
            background-color: #f8fafc;
            border-radius: 8px;
        }
        #userName {
            font-size: 14px;
            color: #475569;
            font-weight: 500;
        }

        /* 当前状态卡片 */
        #currentStatusCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #6366f1, stop:1 #8b5cf6);
            border-radius: 16px;
            color: white;
            margin-bottom: 4px;
        }
        #statusTitle {
            font-size: 18px;
            font-weight: bold;
            color: white;
            margin-bottom: 8px;
        }
        #statusValue {
            font-size: 14px;
            color: rgba(255, 255, 255, 0.9);
            font-weight: 500;
        }
        #inpatientStatus {
            background-color: rgba(255, 255, 255, 0.2);
            border: 2px solid rgba(255, 255, 255, 0.3);
            border-radius: 25px;
            padding: 8px 20px;
            font-size: 14px;
            font-weight: bold;
            color: white;
        }

        /* 信息卡片通用样式 */
        #infoCard, #billCard, #ordersCard {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
        }
        #cardTitle {
            font-size: 16px;
            font-weight: 600;
            color: #1e293b;
        }
        #infoValue {
            font-size: 14px;
            color: #64748b;
        }

        /* 费用信息样式 */
        #billCard QLabel {
            font-size: 14px;
            color: #374151;
        }
        #feeValue {
            font-size: 14px;
            color: #6b7280;
            text-align: right;
        }
        #totalLabel {
            font-size: 16px;
            font-weight: 600;
            color: #1f2937;
        }
        #totalValue {
            font-size: 18px;
            font-weight: bold;
            color: #1f2937;
        }
        #separator {
            background-color: #e5e7eb;
            border: none;
            height: 1px;
        }
        #detailButton {
            background-color: transparent;
            border: 1px solid #3b82f6;
            border-radius: 6px;
            color: #3b82f6;
            font-size: 12px;
            padding: 4px 12px;
        }
        #detailButton:hover {
            background-color: #eff6ff;
        }
        #payButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 500;
            padding: 12px;
            min-height: 44px;
        }
        #payButton:hover {
            background-color: #2563eb;
        }
        #viewAllButton {
            background-color: transparent;
            border: 1px solid #8b5cf6;
            border-radius: 6px;
            color: #8b5cf6;
            font-size: 12px;
            padding: 4px 12px;
        }
        #viewAllButton:hover {
            background-color: #f3e8ff;
        }

        /* 医嘱项样式 */
        #orderItem {
            background-color: #f8fafc;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            border-left: 4px solid #3b82f6;
        }
        #orderType {
            background-color: #dbeafe;
            color: #1d4ed8;
            font-size: 11px;
            font-weight: 500;
            padding: 2px 8px;
            border-radius: 4px;
        }
        #orderDate {
            font-size: 12px;
            color: #6b7280;
        }
        #orderContent {
            font-size: 14px;
            color: #374151;
            line-height: 1.5;
        }
        #orderDoctor {
            font-size: 12px;
            color: #6b7280;
        }
    )");
}