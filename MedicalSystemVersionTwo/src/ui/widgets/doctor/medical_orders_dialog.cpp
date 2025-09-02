#include "medical_orders_dialog.h"
#include <QApplication>
#include <QScreen>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>

MedicalOrdersDialog::MedicalOrdersDialog(const PatientData &patientData, QWidget *parent)
        : QDialog(parent), m_patientData(patientData)
{
    setWindowTitle("长期医嘱单");
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose, false); // 防止自动删除

    // 设置固定窗口大小
    resize(900, 700);

    // 居中显示
    if (parent) {
        move(parent->geometry().center() - rect().center());
    }

    // 初始化所有成员指针
    initPointers();

    // 同步初始化UI，避免异步问题
    initUI();
    loadPatientData(patientData);
    setupTableData(); // 直接设置表格数据，不使用定时器
    applyStyles();
}

void MedicalOrdersDialog::initPointers()
{
    m_ordersTable = nullptr;
    m_notesEdit = nullptr;
    m_nameLabel = nullptr;
    m_genderLabel = nullptr;
    m_ageLabel = nullptr;
    m_patientIdLabel = nullptr;
    m_deptLabel = nullptr;
    m_bedLabel = nullptr;
    m_admitDateLabel = nullptr;
    m_diagnosisLabel = nullptr;
}

void MedicalOrdersDialog::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 25, 30, 25);

    // 添加各个部分
    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createPatientInfo());
    mainLayout->addWidget(createOrdersTable(), 1);
    mainLayout->addWidget(createNotesSection());

    // 关闭按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *closeBtn = new QPushButton("关闭", this); // 明确指定父对象
    closeBtn->setObjectName("closeButton");
    closeBtn->setMinimumSize(100, 35);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);

    mainLayout->addLayout(buttonLayout);
}

QWidget* MedicalOrdersDialog::createHeader()
{
    QWidget *headerWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(headerWidget);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *hospitalLabel = new QLabel("智慧医院", headerWidget);
    hospitalLabel->setObjectName("hospitalTitle");

    QLabel *titleLabel = new QLabel("长期医嘱单", headerWidget);
    titleLabel->setObjectName("recordTitle");

    // 分割线
    QFrame *line = new QFrame(headerWidget);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("titleLine");

    layout->addWidget(hospitalLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(line);

    return headerWidget;
}

QWidget* MedicalOrdersDialog::createPatientInfo()
{
    QFrame *infoFrame = new QFrame(this);
    infoFrame->setObjectName("patientInfoFrame");

    QGridLayout *grid = new QGridLayout(infoFrame);
    grid->setSpacing(15);
    grid->setContentsMargins(20, 15, 20, 15);

    // 第一行
    grid->addWidget(new QLabel("姓名", infoFrame), 0, 0);
    m_nameLabel = new QLabel(infoFrame);
    m_nameLabel->setObjectName("dataLabel");
    grid->addWidget(m_nameLabel, 0, 1);

    grid->addWidget(new QLabel("性别", infoFrame), 0, 2);
    m_genderLabel = new QLabel(infoFrame);
    m_genderLabel->setObjectName("dataLabel");
    grid->addWidget(m_genderLabel, 0, 3);

    grid->addWidget(new QLabel("年龄", infoFrame), 0, 4);
    m_ageLabel = new QLabel(infoFrame);
    m_ageLabel->setObjectName("dataLabel");
    grid->addWidget(m_ageLabel, 0, 5);

    // 第二行
    grid->addWidget(new QLabel("病房号", infoFrame), 1, 0);
    m_patientIdLabel = new QLabel(infoFrame);
    m_patientIdLabel->setObjectName("dataLabel");
    grid->addWidget(m_patientIdLabel, 1, 1);

    grid->addWidget(new QLabel("科室", infoFrame), 1, 2);
    m_deptLabel = new QLabel(infoFrame);
    m_deptLabel->setObjectName("dataLabel");
    grid->addWidget(m_deptLabel, 1, 3);

    grid->addWidget(new QLabel("床号", infoFrame), 1, 4);
    m_bedLabel = new QLabel(infoFrame);
    m_bedLabel->setObjectName("dataLabel");
    grid->addWidget(m_bedLabel, 1, 5);

    // 第三行
    grid->addWidget(new QLabel("入院日期", infoFrame), 2, 0);
    m_admitDateLabel = new QLabel(infoFrame);
    m_admitDateLabel->setObjectName("dataLabel");
    grid->addWidget(m_admitDateLabel, 2, 1);

    grid->addWidget(new QLabel("诊断", infoFrame), 2, 2);
    m_diagnosisLabel = new QLabel(infoFrame);
    m_diagnosisLabel->setObjectName("dataLabel");
    grid->addWidget(m_diagnosisLabel, 2, 3, 1, 3); // 跨3列

    return infoFrame;
}

QWidget* MedicalOrdersDialog::createOrdersTable()
{
    QWidget *tableContainer = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(tableContainer);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    m_ordersTable = new QTableWidget(tableContainer); // 指定父对象
    m_ordersTable->setObjectName("ordersTable");

    // 设置基本属性
    m_ordersTable->setColumnCount(5);
    m_ordersTable->setRowCount(0);

    // 设置列标题
    QStringList headers;
    headers << "日期" << "时间" << "医嘱内容" << "医生" << "签名";
    m_ordersTable->setHorizontalHeaderLabels(headers);

    // 设置表格属性
    m_ordersTable->setAlternatingRowColors(false);
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersTable->setShowGrid(true);
    m_ordersTable->setGridStyle(Qt::SolidLine);
    m_ordersTable->verticalHeader()->setVisible(false);

    // 设置列宽 - 同步设置，不使用定时器
    QHeaderView *header = m_ordersTable->horizontalHeader();
    if (header) {
        header->setStretchLastSection(true);
        m_ordersTable->setColumnWidth(0, 100);  // 日期
        m_ordersTable->setColumnWidth(1, 80);   // 时间
        m_ordersTable->setColumnWidth(2, 350);  // 医嘱内容
        m_ordersTable->setColumnWidth(3, 100);  // 医生
    }

    layout->addWidget(m_ordersTable);
    return tableContainer;
}

QWidget* MedicalOrdersDialog::createNotesSection()
{
    QWidget *notesWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(notesWidget);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 15, 0, 0);

    QLabel *notesTitle = new QLabel("医嘱说明:", notesWidget);
    notesTitle->setObjectName("notesTitle");

    m_notesEdit = new QTextEdit(notesWidget);
    m_notesEdit->setObjectName("notesText");
    m_notesEdit->setMaximumHeight(100);
    m_notesEdit->setReadOnly(true);

    layout->addWidget(notesTitle);
    layout->addWidget(m_notesEdit);

    return notesWidget;
}

void MedicalOrdersDialog::loadPatientData(const PatientData &data)
{
    if (m_nameLabel) m_nameLabel->setText(data.name);
    if (m_genderLabel) m_genderLabel->setText("女");
    if (m_ageLabel) m_ageLabel->setText(QString::number(data.age) + "岁");
    if (m_patientIdLabel) m_patientIdLabel->setText(data.patientId);
    if (m_deptLabel) m_deptLabel->setText(data.department);
    if (m_bedLabel) m_bedLabel->setText("302床");
    if (m_admitDateLabel) m_admitDateLabel->setText("2025-08-30");
    if (m_diagnosisLabel) m_diagnosisLabel->setText("高血压, 冠心病");
}

void MedicalOrdersDialog::setupTableData()
{
    if (!m_ordersTable) {
        qDebug() << "Orders table is null in setupTableData!";
        return;
    }

    // 准备数据
    QList<QStringList> orderData;
    orderData << (QStringList() << "2025-08-30" << "10:30" << "1. 内科护理常规 2. 一级护理 3. 低盐低脂饮食 4. 测血压 bid" << "王医生" << "");
    orderData << (QStringList() << "2025-08-30" << "10:30" << "1. 苯磺酸氨氯地平片 30mg po qd 2. 阿司匹林肠溶片 100mg po qd" << "王医生" << "");

    // 设置行数
    m_ordersTable->setRowCount(orderData.size());

    // 填充数据 - 使用最简单的方式
    for (int row = 0; row < orderData.size(); ++row) {
        const QStringList &rowData = orderData[row];
        for (int col = 0; col < qMin(rowData.size(), 5); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            // 不设置任何特殊属性，保持默认
            m_ordersTable->setItem(row, col, item);
        }
        // 设置适当的行高
        m_ordersTable->setRowHeight(row, 50);
    }

    // 设置医嘱说明
    if (m_notesEdit) {
        m_notesEdit->setPlainText("1. 长期医嘱: 有效时间24小时以上，医生注明停止时间后失效\n"
                                  "2. 临时医嘱: 有效时间在24小时内，只执行一次\n"
                                  "3. 护士签名: _____________ 核对签名: _____________");
    }
}

void MedicalOrdersDialog::applyStyles()
{
    // 使用最基本的样式，避免复杂渲染问题
    setStyleSheet(R"(
        QDialog {
            background-color: white;
        }
        #hospitalTitle {
            font-size: 24px;
            font-weight: bold;
            color: #1a1a1a;
        }
        #recordTitle {
            font-size: 20px;
            font-weight: 600;
            color: #333;
        }
        #patientInfoFrame {
            border: 2px solid #333;
            background-color: #fafafa;
        }
        #dataLabel {
            color: #555;
            padding: 2px 5px;
        }
        #ordersTable {
            border: 1px solid #333;
            background-color: white;
            gridline-color: #333;
            font-size: 12px;
        }
        #notesText {
            border: 1px solid #333;
            background-color: #fdfdfd;
            font-size: 12px;
            padding: 8px;
        }
        #closeButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-size: 14px;
        }
    )");
}