#include "medical_orders_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>

// 定义医嘱数据结构，方便管理
struct MedicalOrder {
    QString date;
    QString time;
    QString content;
    QString doctor;
};

MedicalOrdersWidget::MedicalOrdersWidget(QWidget *parent)
        : QWidget(parent)
{
    setObjectName("MedicalOrdersWidget");
    initUI();
    initStyleSheets();
    loadData();
}

MedicalOrdersWidget::~MedicalOrdersWidget() {}

void MedicalOrdersWidget::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 20, 40, 40);
    mainLayout->setSpacing(25);

    // 为了在小尺寸屏幕上也能完整显示，使用滚动区
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("scrollArea");

    QWidget *mainContentWidget = new QWidget();
    mainContentWidget->setObjectName("mainContentWidget");
    QVBoxLayout *contentLayout = new QVBoxLayout(mainContentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(25);

    contentLayout->addWidget(createHeader());
    contentLayout->addWidget(createPatientInfoPanel());
    contentLayout->addWidget(createOrdersTablePanel(), 1); // 占据剩余空间
    contentLayout->addWidget(createFooterPanel());

    scrollArea->setWidget(mainContentWidget);
    mainLayout->addWidget(scrollArea);
}

QWidget* MedicalOrdersWidget::createHeader() {
    QWidget *headerContainer = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(headerContainer);
    layout->setSpacing(5);
    layout->setContentsMargins(0, 0, 0, 10);

    // 返回按钮
    QPushButton *backButton = new QPushButton("返回仪表盘");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &MedicalOrdersWidget::backRequested);

    // 主标题和副标题
    QLabel* titleLabel = new QLabel("智慧医院");
    titleLabel->setObjectName("mainTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel* subtitleLabel = new QLabel("长期医嘱单");
    subtitleLabel->setObjectName("subtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout* backButtonLayout = new QHBoxLayout();
    backButtonLayout->addWidget(backButton, 0, Qt::AlignLeft);
    backButtonLayout->addStretch();

    layout->addLayout(backButtonLayout);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    return headerContainer;
}

QWidget* MedicalOrdersWidget::createPatientInfoPanel() {
    QFrame* panel = new QFrame();
    panel->setObjectName("infoPanel");

    QGridLayout* layout = new QGridLayout(panel);
    layout->setSpacing(0);

    // 初始化标签
    nameLabel = new QLabel();
    sexLabel = new QLabel();
    ageLabel = new QLabel();
    recordIdLabel = new QLabel();
    departmentLabel = new QLabel();
    bedNumberLabel = new QLabel();
    admissionDateLabel = new QLabel();
    diagnosisLabel = new QLabel();

    // 布局 (第0行)
    layout->addWidget(new QLabel("姓名"), 0, 0);
    layout->addWidget(nameLabel, 0, 1);
    layout->addWidget(new QLabel("性别"), 0, 2);
    layout->addWidget(sexLabel, 0, 3);
    layout->addWidget(new QLabel("年龄"), 0, 4);
    layout->addWidget(ageLabel, 0, 5);
    // (第1行)
    layout->addWidget(new QLabel("病历号"), 1, 0);
    layout->addWidget(recordIdLabel, 1, 1);
    layout->addWidget(new QLabel("科室"), 1, 2);
    layout->addWidget(departmentLabel, 1, 3);
    layout->addWidget(new QLabel("床号"), 1, 4);
    layout->addWidget(bedNumberLabel, 1, 5);
    // (第2行)
    layout->addWidget(new QLabel("入院日期"), 2, 0);
    layout->addWidget(admissionDateLabel, 2, 1, 1, 2); // 跨2列
    layout->addWidget(new QLabel("诊断"), 2, 3);
    layout->addWidget(diagnosisLabel, 2, 4, 1, 2); // 跨2列

    // 设置列的拉伸因子，让空间均匀分布
    for(int i = 0; i < 6; ++i) {
        layout->setColumnStretch(i, 1);
    }

    return panel;
}

QWidget* MedicalOrdersWidget::createOrdersTablePanel() {
    ordersTable = new QTableWidget();
    ordersTable->setObjectName("ordersTable");
    ordersTable->setColumnCount(5);
    ordersTable->setHorizontalHeaderLabels({"日期", "时间", "医嘱内容", "医生", "签名"});

    // 样式设置
    ordersTable->setAlternatingRowColors(true);
    ordersTable->setSelectionMode(QAbstractItemView::NoSelection);
    ordersTable->setFocusPolicy(Qt::NoFocus);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->horizontalHeader()->setStretchLastSection(true);

    // 设置列宽
    ordersTable->setColumnWidth(0, 120);
    ordersTable->setColumnWidth(1, 100);
    ordersTable->setColumnWidth(3, 120);
    ordersTable->setColumnWidth(4, 120);
    ordersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);


    return ordersTable;
}

QWidget* MedicalOrdersWidget::createFooterPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(15);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel* notesTitle = new QLabel("医嘱说明:");
    notesTitle->setObjectName("notesTitle");

    QLabel* note1 = new QLabel("1. 长期医嘱：有效时间24小时以上，医生注明停止时间后即失效");
    QLabel* note2 = new QLabel("2. 临时医嘱：有效时间24小时以内，只执行一次");
    QLabel* note3 = new QLabel("3. 护士签名: ______________   核对签名: ______________");

    note1->setObjectName("notesText");
    note2->setObjectName("notesText");
    note3->setObjectName("notesText");

    layout->addWidget(notesTitle);
    layout->addWidget(note1);
    layout->addWidget(note2);
    layout->addSpacing(20);
    layout->addWidget(note3);

    return panel;
}

void MedicalOrdersWidget::loadData() {
    // 1. 填充患者信息
    nameLabel->setText("张患者");
    sexLabel->setText("女");
    ageLabel->setText("38 岁");
    recordIdLabel->setText("P202305001");
    departmentLabel->setText("心血管内科");
    bedNumberLabel->setText("302 床");
    admissionDateLabel->setText("2025-08-30");
    diagnosisLabel->setText("高血压、冠心病");

    // 2. 填充医嘱表格数据
    QList<MedicalOrder> orders;
    orders.append({"2025-08-30", "10:30", "1. 内科护理常规\n2. 二级护理\n3. 低盐低脂饮食\n4. 测血压 bid", "王医生"});
    orders.append({"2025-08-30", "10:30", "1. 硝苯地平控释片 30mg po qd\n2. 阿司匹林肠溶片 100mg po qd\n3. 阿托伐他汀钙片 20mg po qn", "王医生"});

    ordersTable->setRowCount(orders.size());
    for(int row = 0; row < orders.size(); ++row) {
        const auto& order = orders[row];
        ordersTable->setItem(row, 0, new QTableWidgetItem(order.date));
        ordersTable->setItem(row, 1, new QTableWidgetItem(order.time));
        ordersTable->setItem(row, 2, new QTableWidgetItem(order.content));
        ordersTable->setItem(row, 3, new QTableWidgetItem(order.doctor));
        ordersTable->setItem(row, 4, new QTableWidgetItem("")); // 签名列为空

        // 居中对齐
        ordersTable->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        ordersTable->item(row, 1)->setTextAlignment(Qt::AlignCenter);
        ordersTable->item(row, 3)->setTextAlignment(Qt::AlignCenter);
    }

    // 自动调整行高以适应多行文本
    ordersTable->resizeRowsToContents();
}


void MedicalOrdersWidget::initStyleSheets() {
    this->setStyleSheet(R"(
        #MedicalOrdersWidget, #mainContentWidget {
            background-color: #F7FAFC;
        }
        #scrollArea {
            border: none;
        }
        #backButton {
            background-color: #FFFFFF;
            color: #4A5568;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 8px 16px;
            font-size: 14px;
        }
        #backButton:hover {
            background-color: #EDF2F7;
        }
        #mainTitle {
            font-size: 28px;
            font-weight: bold;
            color: #1A202C;
        }
        #subtitle {
            font-size: 20px;
            font-weight: 500;
            color: #2D3748;
            margin-top: -5px;
        }

        /* 患者信息面板 */
        #infoPanel {
            border: 1px solid #CBD5E0;
            border-radius: 8px;
            background-color: #FFFFFF;
        }
        #infoPanel QLabel {
            font-size: 14px;
            color: #4A5568;
            padding: 12px 10px;
            border-right: 1px solid #EDF2F7;
            border-bottom: 1px solid #EDF2F7;
        }
        /* 去除最后一行和最后一列的边框 */
        #infoPanel QGridLayout > QWidget:nth-child(6n),
        #infoPanel QGridLayout > QWidget:nth-child(n+13) {
            border-right: none;
        }
        #infoPanel QGridLayout > QWidget:nth-child(n+13) {
            border-bottom: none;
        }

        /* 医嘱表格 */
        #ordersTable {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            gridline-style: solid;
            font-size: 14px;
        }
        QHeaderView::section {
            background-color: #EDF2F7;
            padding: 10px;
            border: none;
            border-bottom: 1px solid #E2E8F0;
            font-weight: bold;
            color: #2D3748;
        }
        QTableWidget::item {
            padding: 10px;
            border-bottom: 1px solid #F1F5F9;
        }
        QTableWidget::item:selected {
            background-color: transparent;
            color: black;
        }
        QTableWidget::alternating-row-color {
            background-color: #F7FAFC;
        }

        /* 底部说明 */
        #notesTitle {
            font-size: 15px;
            font-weight: bold;
            color: #2D3748;
        }
        #notesText {
            font-size: 14px;
            color: #4A5568;
        }
    )");
}