#include "long_term_orders_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QSvgWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSpacerItem>
#include <QFont>
#include <QDebug>

// SVG图标定义
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 12H5"/><path d="M12 19l-7-7 7-7"/></svg>)";
static const char* printIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6,9 6,2 18,2 18,9"/><path d="M6,18H4a2,2,0,0,1-2-2V11a2,2,0,0,1,2,2H20a2,2,0,0,1,2-2v5a2,2,0,0,1-2,2H18"/><rect x="6" y="14" width="12" height="8"/></svg>)";
static const char* exportIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="7,10 12,15 17,10"/><line x1="12" y1="15" x2="12" y2="3"/></svg>)";
static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";

LongTermOrdersWidget::LongTermOrdersWidget(QWidget *parent)
        : QWidget(parent)
        , scrollArea(nullptr)
        , contentWidget(nullptr)
        , ordersTable(nullptr)
        , printBtn(nullptr)
        , exportBtn(nullptr)
        // 【已修复】初始化所有 QLabel 指针为 nullptr
        , nameLabel(nullptr)
        , genderLabel(nullptr)
        , ageLabel(nullptr)
        , recordLabel(nullptr)
        , deptLabel(nullptr)
        , bedLabel(nullptr)
        , dateLabel(nullptr)
        , diagnosisLabel(nullptr)
{
    qDebug() << "LongTermOrdersWidget 构造函数开始...";

    try {
        this->setObjectName("LongTermOrdersWidget");

        qDebug() << "开始设置数据...";
        setupData();

        qDebug() << "开始初始化UI...";
        initUI();

        qDebug() << "开始应用样式...";
        applyStyles();

        qDebug() << "LongTermOrdersWidget 构造完成";

    } catch (const std::exception& e) {
        qDebug() << "LongTermOrdersWidget 构造时发生标准异常:" << e.what();
    } catch (...) {
        qDebug() << "LongTermOrdersWidget 构造时发生未知异常";
    }
}

void LongTermOrdersWidget::setupData()
{
    // 设置示例长期医嘱数据
    orderData.patientName = "张患者";
    orderData.gender = "女";
    orderData.age = "38岁";
    orderData.recordNumber = "P20230501";
    orderData.department = "心血管内科";
    orderData.bedNumber = "302床";
    orderData.admissionDate = "2025-08-30";
    orderData.diagnosis = "高血压、冠心病";

    // 医嘱列表
    ordersList.clear();

    // 第一组医嘱
    MedicalOrderItem order1;
    order1.date = "2025-08-30";
    order1.time = "10:30";
    order1.orderContent = "1. 内科护理常规\n2. 一级护理\n3. 低盐低脂饮食\n4. 测血压 bid";
    order1.doctorName = "王医生";
    order1.signature = "";
    order1.isActive = true;
    ordersList.append(order1);

    // 第二组医嘱
    MedicalOrderItem order2;
    order2.date = "2025-08-30";
    order2.time = "10:30";
    order2.orderContent = "1. 硝苯地平缓释片 30mg po qd\n2. 阿司匹林肠溶片 100mg po qd\n3. 阿托伐他汀片 20mg po qn";
    order2.doctorName = "王医生";
    order2.signature = "";
    order2.isActive = true;
    ordersList.append(order2);

    // 添加空行用于显示
    for (int i = 0; i < 6; i++) {
        MedicalOrderItem emptyOrder;
        emptyOrder.date = "";
        emptyOrder.time = "";
        emptyOrder.orderContent = "";
        emptyOrder.doctorName = "";
        emptyOrder.signature = "";
        emptyOrder.isActive = false;
        ordersList.append(emptyOrder);
    }
}

void LongTermOrdersWidget::initUI()
{
    this->setObjectName("longTermOrdersWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(0);

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setObjectName("orderScrollArea");

    contentWidget = new QWidget();
    contentWidget->setObjectName("orderContentWidget");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    contentLayout->addWidget(createHeader());
    contentLayout->addSpacing(20);
    contentLayout->addWidget(createOrderHeader());
    contentLayout->addSpacing(10);
    contentLayout->addWidget(createPatientInfoTable());
    contentLayout->addSpacing(15);
    contentLayout->addWidget(createOrdersTable());
    contentLayout->addSpacing(10);
    contentLayout->addWidget(createNotesSection());
    contentLayout->addSpacing(30);

    scrollArea->setWidget(contentWidget);

    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(createActionButtons());
}

QWidget* LongTermOrdersWidget::createHeader()
{
    QFrame *header = new QFrame();
    header->setObjectName("headerFrame");

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);
    headerLayout->setSpacing(16);

    QPushButton *backBtn = new QPushButton("← 返回");
    backBtn->setObjectName("backButton");
    backBtn->setMinimumHeight(36);
    backBtn->setMinimumWidth(100);
    connect(backBtn, &QPushButton::clicked, this, &LongTermOrdersWidget::onBackButtonClicked);

    QLabel *title = new QLabel("长期医嘱单");
    title->setObjectName("headerTitle");

    QFrame *userFrame = new QFrame();
    userFrame->setObjectName("userFrame");
    QHBoxLayout *userLayout = new QHBoxLayout(userFrame);
    userLayout->setContentsMargins(12, 8, 12, 8);
    userLayout->setSpacing(8);

    QSvgWidget *userIcon = new QSvgWidget();
    userIcon->load(QByteArray(userIconSvg));
    userIcon->setFixedSize(24, 24);

    QLabel *userName = new QLabel(orderData.patientName);
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

QWidget* LongTermOrdersWidget::createOrderHeader()
{
    QFrame *orderHeader = new QFrame();
    orderHeader->setObjectName("orderHeaderFrame");

    QVBoxLayout *headerLayout = new QVBoxLayout(orderHeader);
    headerLayout->setContentsMargins(40, 30, 40, 20);
    headerLayout->setSpacing(10);
    headerLayout->setAlignment(Qt::AlignCenter);

    QLabel *hospitalName = new QLabel("智慧医院");
    hospitalName->setObjectName("hospitalName");
    hospitalName->setAlignment(Qt::AlignCenter);

    QLabel *orderTitle = new QLabel("长期医嘱单");
    orderTitle->setObjectName("orderTitle");
    orderTitle->setAlignment(Qt::AlignCenter);

    QFrame *titleLine = new QFrame();
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setObjectName("titleLine");
    titleLine->setFixedHeight(2);
    titleLine->setMinimumWidth(400);

    headerLayout->addWidget(hospitalName);
    headerLayout->addWidget(orderTitle);
    headerLayout->addSpacing(15);
    headerLayout->addWidget(titleLine, 0, Qt::AlignCenter);

    return orderHeader;
}

QWidget* LongTermOrdersWidget::createPatientInfoTable()
{
    QFrame *tableFrame = new QFrame();
    tableFrame->setObjectName("patientInfoTable");

    QGridLayout *tableLayout = new QGridLayout(tableFrame);
    tableLayout->setContentsMargins(40, 0, 40, 0);
    tableLayout->setSpacing(0);

    // 第一行
    tableLayout->addWidget(createTableCell("姓名"), 0, 0);
    nameLabel = new QLabel(orderData.patientName);
    nameLabel->setObjectName("cellValue");
    QFrame *nameFrame = new QFrame();
    nameFrame->setObjectName("tableCell");
    QHBoxLayout *nameLayout = new QHBoxLayout(nameFrame);
    nameLayout->setContentsMargins(12, 8, 12, 8);
    nameLayout->addWidget(nameLabel);
    tableLayout->addWidget(nameFrame, 0, 1);

    tableLayout->addWidget(createTableCell("性别"), 0, 2);
    genderLabel = new QLabel(orderData.gender);
    genderLabel->setObjectName("cellValue");
    QFrame *genderFrame = new QFrame();
    genderFrame->setObjectName("tableCell");
    QHBoxLayout *genderLayout = new QHBoxLayout(genderFrame);
    genderLayout->setContentsMargins(12, 8, 12, 8);
    genderLayout->addWidget(genderLabel);
    tableLayout->addWidget(genderFrame, 0, 3);

    tableLayout->addWidget(createTableCell("年龄"), 0, 4);
    ageLabel = new QLabel(orderData.age);
    ageLabel->setObjectName("cellValue");
    QFrame *ageFrame = new QFrame();
    ageFrame->setObjectName("tableCell");
    QHBoxLayout *ageLayout = new QHBoxLayout(ageFrame);
    ageLayout->setContentsMargins(12, 8, 12, 8);
    ageLayout->addWidget(ageLabel);
    tableLayout->addWidget(ageFrame, 0, 5);

    // 第二行
    tableLayout->addWidget(createTableCell("病历号"), 1, 0);
    recordLabel = new QLabel(orderData.recordNumber);
    recordLabel->setObjectName("cellValue");
    QFrame *recordFrame = new QFrame();
    recordFrame->setObjectName("tableCell");
    QHBoxLayout *recordLayout = new QHBoxLayout(recordFrame);
    recordLayout->setContentsMargins(12, 8, 12, 8);
    recordLayout->addWidget(recordLabel);
    tableLayout->addWidget(recordFrame, 1, 1);

    tableLayout->addWidget(createTableCell("科室"), 1, 2);
    deptLabel = new QLabel(orderData.department);
    deptLabel->setObjectName("cellValue");
    QFrame *deptFrame = new QFrame();
    deptFrame->setObjectName("tableCell");
    QHBoxLayout *deptLayout = new QHBoxLayout(deptFrame);
    deptLayout->setContentsMargins(12, 8, 12, 8);
    deptLayout->addWidget(deptLabel);
    tableLayout->addWidget(deptFrame, 1, 3);

    tableLayout->addWidget(createTableCell("床号"), 1, 4);
    bedLabel = new QLabel(orderData.bedNumber);
    bedLabel->setObjectName("cellValue");
    QFrame *bedFrame = new QFrame();
    bedFrame->setObjectName("tableCell");
    QHBoxLayout *bedLayout = new QHBoxLayout(bedFrame);
    bedLayout->setContentsMargins(12, 8, 12, 8);
    bedLayout->addWidget(bedLabel);
    tableLayout->addWidget(bedFrame, 1, 5);

    // 第三行
    tableLayout->addWidget(createTableCell("入院日期"), 2, 0);
    dateLabel = new QLabel(orderData.admissionDate);
    dateLabel->setObjectName("cellValue");
    QFrame *dateFrame = new QFrame();
    dateFrame->setObjectName("tableCell");
    QHBoxLayout *dateLayout = new QHBoxLayout(dateFrame);
    dateLayout->setContentsMargins(12, 8, 12, 8);
    dateLayout->addWidget(dateLabel);
    tableLayout->addWidget(dateFrame, 2, 1);

    tableLayout->addWidget(createTableCell("诊断"), 2, 2);
    diagnosisLabel = new QLabel(orderData.diagnosis);
    diagnosisLabel->setObjectName("cellValue");
    QFrame *diagnosisFrame = new QFrame();
    diagnosisFrame->setObjectName("tableCell");
    QHBoxLayout *diagnosisLayout = new QHBoxLayout(diagnosisFrame);
    diagnosisLayout->setContentsMargins(12, 8, 12, 8);
    diagnosisLayout->addWidget(diagnosisLabel);
    tableLayout->addWidget(diagnosisFrame, 2, 3, 1, 3);

    for (int i = 0; i < 6; i++) {
        tableLayout->setColumnStretch(i, 1);
    }

    return tableFrame;
}

QWidget* LongTermOrdersWidget::createOrdersTable()
{
    QFrame *tableFrame = new QFrame();
    tableFrame->setObjectName("ordersTableFrame");

    QVBoxLayout *tableLayout = new QVBoxLayout(tableFrame);
    tableLayout->setContentsMargins(40, 0, 40, 0);
    tableLayout->setSpacing(0);

    ordersTable = new QTableWidget();
    ordersTable->setObjectName("ordersTable");
    ordersTable->setRowCount(ordersList.size());
    ordersTable->setColumnCount(5);

    QStringList headers;
    headers << "日期" << "时间" << "医嘱内容" << "医生" << "签名";
    ordersTable->setHorizontalHeaderLabels(headers);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setSelectionMode(QAbstractItemView::NoSelection);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ordersTable->setAlternatingRowColors(false);
    ordersTable->setShowGrid(true);
    ordersTable->setGridStyle(Qt::SolidLine);

    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->horizontalHeader()->setStretchLastSection(true);
    ordersTable->horizontalHeader()->setDefaultSectionSize(80);

    ordersTable->setColumnWidth(0, 80);
    ordersTable->setColumnWidth(1, 60);
    ordersTable->setColumnWidth(2, 350);
    ordersTable->setColumnWidth(3, 80);
    ordersTable->setColumnWidth(4, 80);

    populateOrdersTable();

    int totalHeight = ordersTable->horizontalHeader()->height();
    for (int i = 0; i < ordersTable->rowCount(); i++) {
        totalHeight += ordersTable->rowHeight(i);
    }
    ordersTable->setFixedHeight(totalHeight + 2);

    tableLayout->addWidget(ordersTable);

    return tableFrame;
}

QWidget* LongTermOrdersWidget::createNotesSection()
{
    QFrame *notesFrame = new QFrame();
    notesFrame->setObjectName("notesFrame");

    QVBoxLayout *notesLayout = new QVBoxLayout(notesFrame);
    notesLayout->setContentsMargins(40, 0, 40, 20);
    notesLayout->setSpacing(8);

    QLabel *notesTitle = new QLabel("医嘱说明：");
    notesTitle->setObjectName("notesTitle");

    QLabel *note1 = new QLabel("1. 长期医嘱：有效时间24小时以上，医生未注明停止时间则继续执行");
    note1->setObjectName("noteText");

    QLabel *note2 = new QLabel("2. 临时医嘱：有效时间24小时以内，只执行一次");
    note2->setObjectName("noteText");

    QLabel *note3 = new QLabel("3. 护士签名：________    核对签名：________");
    note3->setObjectName("noteText");

    notesLayout->addWidget(notesTitle);
    notesLayout->addWidget(note1);
    notesLayout->addWidget(note2);
    notesLayout->addWidget(note3);

    return notesFrame;
}

QWidget* LongTermOrdersWidget::createActionButtons()
{
    QFrame *actionFrame = new QFrame();
    actionFrame->setObjectName("actionButtonFrame");

    QHBoxLayout *actionLayout = new QHBoxLayout(actionFrame);
    actionLayout->setContentsMargins(24, 16, 24, 16);
    actionLayout->setSpacing(16);

    printBtn = new QPushButton();
    printBtn->setObjectName("printButton");
    printBtn->setMinimumHeight(44);

    QHBoxLayout *printLayout = new QHBoxLayout(printBtn);
    printLayout->setContentsMargins(16, 8, 16, 8);
    printLayout->setSpacing(8);

    QSvgWidget *printIcon = new QSvgWidget();
    printIcon->load(QByteArray(printIconSvg));
    printIcon->setFixedSize(20, 20);

    QLabel *printText = new QLabel("打印医嘱");
    printText->setObjectName("buttonText");

    printLayout->addWidget(printIcon);
    printLayout->addWidget(printText);
    printLayout->addStretch();

    connect(printBtn, &QPushButton::clicked, this, &LongTermOrdersWidget::onPrintOrdersClicked);

    exportBtn = new QPushButton();
    exportBtn->setObjectName("exportButton");
    exportBtn->setMinimumHeight(44);

    QHBoxLayout *exportLayout = new QHBoxLayout(exportBtn);
    exportLayout->setContentsMargins(16, 8, 16, 8);
    exportLayout->setSpacing(8);

    QSvgWidget *exportIcon = new QSvgWidget();
    exportIcon->load(QByteArray(exportIconSvg));
    exportIcon->setFixedSize(20, 20);

    QLabel *exportText = new QLabel("导出PDF");
    exportText->setObjectName("buttonText");

    exportLayout->addWidget(exportIcon);
    exportLayout->addWidget(exportText);
    exportLayout->addStretch();

    connect(exportBtn, &QPushButton::clicked, this, &LongTermOrdersWidget::onExportOrdersClicked);

    actionLayout->addStretch();
    actionLayout->addWidget(printBtn);
    actionLayout->addWidget(exportBtn);
    actionLayout->addStretch();

    return actionFrame;
}

QFrame* LongTermOrdersWidget::createTableCell(const QString &text, bool isHeader, int span)
{
    QFrame *cell = new QFrame();
    if (isHeader) {
        cell->setObjectName("tableHeaderCell");
    } else {
        cell->setObjectName("tableCell");
    }

    QHBoxLayout *cellLayout = new QHBoxLayout(cell);
    cellLayout->setContentsMargins(12, 8, 12, 8);
    cellLayout->setSpacing(0);

    QLabel *textLabel = new QLabel(text);
    if (isHeader) {
        textLabel->setObjectName("cellHeaderText");
    } else {
        textLabel->setObjectName("cellLabel");
    }
    textLabel->setAlignment(Qt::AlignCenter);

    cellLayout->addWidget(textLabel);

    return cell;
}

void LongTermOrdersWidget::populateOrdersTable()
{
    for (int row = 0; row < ordersList.size(); row++) {
        const MedicalOrderItem &order = ordersList[row];

        QTableWidgetItem *dateItem = new QTableWidgetItem(order.date);
        dateItem->setTextAlignment(Qt::AlignCenter);
        ordersTable->setItem(row, 0, dateItem);

        QTableWidgetItem *timeItem = new QTableWidgetItem(order.time);
        timeItem->setTextAlignment(Qt::AlignCenter);
        ordersTable->setItem(row, 1, timeItem);

        QTableWidgetItem *contentItem = new QTableWidgetItem(order.orderContent);
        contentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        ordersTable->setItem(row, 2, contentItem);

        QTableWidgetItem *doctorItem = new QTableWidgetItem(order.doctorName);
        doctorItem->setTextAlignment(Qt::AlignCenter);
        ordersTable->setItem(row, 3, doctorItem);

        QTableWidgetItem *signItem = new QTableWidgetItem(order.signature);
        signItem->setTextAlignment(Qt::AlignCenter);
        ordersTable->setItem(row, 4, signItem);

        ordersTable->setRowHeight(row, 60);
    }
}

void LongTermOrdersWidget::setOrderData(const LongTermOrderData &data)
{
    orderData = data;
    if (nameLabel) nameLabel->setText(data.patientName);
    if (genderLabel) genderLabel->setText(data.gender);
    if (ageLabel) ageLabel->setText(data.age);
}

void LongTermOrdersWidget::onBackButtonClicked()
{
    emit backRequested();
}

void LongTermOrdersWidget::onPrintOrdersClicked()
{
    qDebug() << "打印长期医嘱单";
}

void LongTermOrdersWidget::onExportOrdersClicked()
{
    qDebug() << "导出长期医嘱单PDF";
}

void LongTermOrdersWidget::applyStyles()
{
    qDebug() << "开始应用长期医嘱单样式表...";

    try {
        this->setStyleSheet(R"(
            #longTermOrdersWidget {
                background-color: #f0f9ff;
            }
            #headerFrame {
                background-color: white;
                border-radius: 12px;
                border: 1px solid #e2e8f0;
                margin-bottom: 10px;
            }
            #backButton {
                background-color: transparent;
                border: 1px solid #e2e8f0;
                border-radius: 8px;
                color: #64748b;
                font-size: 14px;
                font-weight: 500;
                padding: 8px 16px;
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
            #orderScrollArea {
                background-color: white;
                border: 1px solid #e2e8f0;
                border-radius: 12px;
            }
            #orderContentWidget {
                background-color: white;
            }
            #orderHeaderFrame {
                background-color: white;
            }
            #hospitalName {
                font-size: 24px;
                font-weight: bold;
                color: #1e293b;
                margin-bottom: 5px;
            }
            #orderTitle {
                font-size: 18px;
                font-weight: 600;
                color: #374151;
                margin-bottom: 10px;
            }
            #titleLine {
                background-color: #000000;
                border: none;
            }
            #patientInfoTable {
                background-color: white;
            }
            #tableCell, #tableHeaderCell {
                border: 1px solid #000000;
                background-color: white;
                min-height: 35px;
            }
            #tableHeaderCell {
                background-color: #f8fafc;
                font-weight: 600;
            }
            #cellLabel {
                font-size: 14px;
                color: #374151;
                font-weight: 500;
                text-align: center;
            }
            #cellValue, #cellHeaderText {
                font-size: 14px;
                color: #1f2937;
            }
            #cellHeaderText {
                font-weight: 600;
                text-align: center;
            }
            #ordersTableFrame {
                background-color: white;
            }
            #ordersTable {
                border: 1px solid #000000;
                background-color: white;
                gridline-color: #000000;
            }
            #ordersTable::item {
                border: 1px solid #000000;
                padding: 5px;
                font-size: 13px;
            }
            #ordersTable QHeaderView::section {
                background-color: #f8fafc;
                border: 1px solid #000000;
                padding: 8px;
                font-weight: 600;
                font-size: 14px;
                text-align: center;
            }
            #notesFrame {
                background-color: white;
            }
            #notesTitle {
                font-size: 14px;
                font-weight: 600;
                color: #1f2937;
                margin-bottom: 5px;
            }
            #noteText {
                font-size: 12px;
                color: #374151;
                line-height: 1.4;
                margin-bottom: 3px;
            }
            #actionButtonFrame {
                background-color: white;
                border-radius: 12px;
                border: 1px solid #e2e8f0;
                margin-top: 10px;
            }
            #printButton {
                background-color: #3b82f6;
                color: white;
                border: none;
                border-radius: 8px;
                font-size: 14px;
                font-weight: 500;
                min-width: 120px;
            }
            #printButton:hover {
                background-color: #2563eb;
            }
            #exportButton {
                background-color: #10b981;
                color: white;
                border: none;
                border-radius: 8px;
                font-size: 14px;
                font-weight: 500;
                min-width: 120px;
            }
            #exportButton:hover {
                background-color: #059669;
            }
            #buttonText {
                font-size: 14px;
                font-weight: 500;
                color: inherit;
            }
        )");

        qDebug() << "长期医嘱单样式表应用成功";

    } catch (const std::exception& e) {
        qDebug() << "应用样式表时发生异常:" << e.what();
        this->setStyleSheet("");
    } catch (...) {
        qDebug() << "应用样式表时发生未知异常";
        this->setStyleSheet("");
    }
}