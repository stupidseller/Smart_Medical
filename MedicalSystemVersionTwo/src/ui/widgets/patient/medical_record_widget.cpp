#include "medical_record_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QSvgWidget>
#include <QTextEdit>
#include <QSpacerItem>
#include <QFont>
#include <QFontMetrics>
#include <QApplication>

// SVG图标定义
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 12H5"/><path d="M12 19l-7-7 7-7"/></svg>)";

static const char* printIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6,9 6,2 18,2 18,9"/><path d="M6,18H4a2,2,0,0,1-2-2V11a2,2,0,0,1,2-2H20a2,2,0,0,1,2,2v5a2,2,0,0,1-2,2H18"/><rect x="6" y="14" width="12" height="8"/></svg>)";

static const char* exportIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="7,10 12,15 17,10"/><line x1="12" y1="15" x2="12" y2="3"/></svg>)";

static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#48BB78" stroke="white" stroke-width="1.5"><circle cx="12" cy="8" r="5"/><path d="M20 21a8 8 0 1 0-16 0"/></svg>)";

MedicalRecordWidget::MedicalRecordWidget(QWidget *parent)
        : QWidget(parent)
{
    setupData();
    initUI();
    applyStyles();
}

void MedicalRecordWidget::setupData()
{
    // 设置示例病历数据
    recordData.patientName = "张患者";
    recordData.gender = "女";
    recordData.age = "38岁";
    recordData.recordNumber = "P20230501";
    recordData.department = "心血管内科";
    recordData.bedNumber = "302床";
    recordData.admissionDate = "2025-08-30";
    recordData.recordDate = "2025-08-31";
    recordData.recordTime = "18:35";

    // 病历内容
    recordData.chiefComplaint = "反复胸闷、胸痛3天，加重2小时。";

    recordData.presentIllness = "患者于3天前无明显诱因出现胸闷不适、胸痛，位于前胸、"
                                "呈压迫性疼痛，持续约5-10分钟，休息后可缓解。2小时前症状加重，"
                                "疼痛较前剧烈，伴出汗、恶心、呕吐，无咯血，故来我科就诊，"
                                "目前症状有所缓解，为求进一步诊治来院。";

    recordData.pastHistory = "高血压病史5年，最高血压180/100mmHg，现降压药治疗，"
                             "否认胸科病史，冠心病史，否认手术外伤史，否认药物过敏史。";

    recordData.diagnosis = "急性非ST段抬高型心肌梗死";
    recordData.icdCode = "I21.0";
    recordData.doctorName = "王医生";

    // 诊断列表
    diagnosisList.clear();
    diagnosisList << DiagnosisItem{"主要诊断", "急性非ST段抬高型心肌梗死", "I21.0"};
}

void MedicalRecordWidget::initUI()
{
    this->setObjectName("medicalRecordWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(0);

    // 创建滚动区域
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setObjectName("recordScrollArea");

    // 创建内容容器
    contentWidget = new QWidget();
    contentWidget->setObjectName("recordContentWidget");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // 添加各个部分
    contentLayout->addWidget(createHeader());
    contentLayout->addSpacing(20);
    contentLayout->addWidget(createRecordHeader());
    contentLayout->addSpacing(10);
    contentLayout->addWidget(createPatientInfoTable());
    contentLayout->addSpacing(20);
    contentLayout->addWidget(createRecordContent());
    contentLayout->addSpacing(15);
    contentLayout->addWidget(createDiagnosisSection());
    contentLayout->addSpacing(30);

    scrollArea->setWidget(contentWidget);

    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(createActionButtons());
}

QWidget* MedicalRecordWidget::createHeader()
{
    QFrame *header = new QFrame();
    header->setObjectName("headerFrame");

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);
    headerLayout->setSpacing(16);

    // 返回按钮
    QPushButton *backBtn = new QPushButton("← 返回");
    backBtn->setObjectName("backButton");
    backBtn->setMinimumHeight(36);
    backBtn->setMinimumWidth(100);
    connect(backBtn, &QPushButton::clicked, this, &MedicalRecordWidget::onBackButtonClicked);

    // 标题
    QLabel *title = new QLabel("病历记录单");
    title->setObjectName("headerTitle");

    // 用户信息（可选）
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

QWidget* MedicalRecordWidget::createRecordHeader()
{
    QFrame *recordHeader = new QFrame();
    recordHeader->setObjectName("recordHeaderFrame");

    QVBoxLayout *headerLayout = new QVBoxLayout(recordHeader);
    headerLayout->setContentsMargins(40, 30, 40, 20);
    headerLayout->setSpacing(10);
    headerLayout->setAlignment(Qt::AlignCenter);

    // 医院名称
    QLabel *hospitalName = new QLabel("智慧医院");
    hospitalName->setObjectName("hospitalName");
    hospitalName->setAlignment(Qt::AlignCenter);

    // 病历记录单标题
    QLabel *recordTitle = new QLabel("病历记录单");
    recordTitle->setObjectName("recordTitle");
    recordTitle->setAlignment(Qt::AlignCenter);

    // 分割线
    QFrame *titleLine = new QFrame();
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setObjectName("titleLine");
    titleLine->setFixedHeight(2);
    titleLine->setMinimumWidth(400);

    headerLayout->addWidget(hospitalName);
    headerLayout->addWidget(recordTitle);
    headerLayout->addSpacing(15);
    headerLayout->addWidget(titleLine, 0, Qt::AlignCenter);

    return recordHeader;
}

QWidget* MedicalRecordWidget::createPatientInfoTable()
{
    QFrame *tableFrame = new QFrame();
    tableFrame->setObjectName("patientInfoTable");

    QGridLayout *tableLayout = new QGridLayout(tableFrame);
    tableLayout->setContentsMargins(40, 0, 40, 0);
    tableLayout->setSpacing(0);

    // 创建表格结构 - 3行2列的网格
    // 第一行
    tableLayout->addWidget(createTableCell("姓名", recordData.patientName), 0, 0);
    tableLayout->addWidget(createTableCell("性别", recordData.gender), 0, 1);
    tableLayout->addWidget(createTableCell("年龄", recordData.age), 0, 2);

    // 第二行
    tableLayout->addWidget(createTableCell("病历号", recordData.recordNumber), 1, 0);
    tableLayout->addWidget(createTableCell("科室", recordData.department), 1, 1);
    tableLayout->addWidget(createTableCell("床号", recordData.bedNumber), 1, 2);

    // 第三行
    tableLayout->addWidget(createTableCell("入院日期", recordData.admissionDate), 2, 0);
    tableLayout->addWidget(createTableCell("记录日期", recordData.recordDate), 2, 1);
    tableLayout->addWidget(createTableCell("记录时间", recordData.recordTime), 2, 2);

    // 设置列宽比例
    tableLayout->setColumnStretch(0, 1);
    tableLayout->setColumnStretch(1, 1);
    tableLayout->setColumnStretch(2, 1);

    return tableFrame;
}

QWidget* MedicalRecordWidget::createRecordContent()
{
    QFrame *contentFrame = new QFrame();
    contentFrame->setObjectName("recordContentFrame");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setContentsMargins(40, 0, 40, 0);
    contentLayout->setSpacing(20);

    // 主诉
    contentLayout->addWidget(createSectionFrame("主诉", recordData.chiefComplaint));

    // 现病史
    contentLayout->addWidget(createSectionFrame("现病史", recordData.presentIllness));

    // 既往史
    contentLayout->addWidget(createSectionFrame("既往史", recordData.pastHistory));

    return contentFrame;
}

QWidget* MedicalRecordWidget::createDiagnosisSection()
{
    QFrame *diagnosisFrame = new QFrame();
    diagnosisFrame->setObjectName("diagnosisFrame");

    QVBoxLayout *diagnosisLayout = new QVBoxLayout(diagnosisFrame);
    diagnosisLayout->setContentsMargins(40, 0, 40, 20);
    diagnosisLayout->setSpacing(15);

    // 诊断标题
    QLabel *diagnosisTitle = new QLabel("诊断");
    diagnosisTitle->setObjectName("sectionTitle");
    diagnosisLayout->addWidget(diagnosisTitle);

    // 诊断表格
    QFrame *diagnosisTable = new QFrame();
    diagnosisTable->setObjectName("diagnosisTable");

    QGridLayout *tableLayout = new QGridLayout(diagnosisTable);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);

    // 表头
    tableLayout->addWidget(createTableCell("诊断类型", "", true), 0, 0);
    tableLayout->addWidget(createTableCell("诊断内容", "", true), 0, 1);
    tableLayout->addWidget(createTableCell("ICD编码", "", true), 0, 2);

    // 诊断条目
    int row = 1;
    for (const auto &item : diagnosisList) {
        tableLayout->addWidget(createTableCell("", item.type), row, 0);
        tableLayout->addWidget(createTableCell("", item.content), row, 1);
        tableLayout->addWidget(createTableCell("", item.icdCode), row, 2);
        row++;
    }

    // 空行用于填充
    for (int i = row; i < 4; i++) {
        tableLayout->addWidget(createTableCell("", ""), i, 0);
        tableLayout->addWidget(createTableCell("", ""), i, 1);
        tableLayout->addWidget(createTableCell("", ""), i, 2);
    }

    // 设置列宽
    tableLayout->setColumnStretch(0, 1);
    tableLayout->setColumnStretch(1, 3);
    tableLayout->setColumnStretch(2, 1);

    diagnosisLayout->addWidget(diagnosisTable);

    return diagnosisFrame;
}

QWidget* MedicalRecordWidget::createActionButtons()
{
    QFrame *actionFrame = new QFrame();
    actionFrame->setObjectName("actionButtonFrame");

    QHBoxLayout *actionLayout = new QHBoxLayout(actionFrame);
    actionLayout->setContentsMargins(24, 16, 24, 16);
    actionLayout->setSpacing(16);

    // 打印病历按钮
    printBtn = new QPushButton();
    printBtn->setObjectName("printButton");
    printBtn->setMinimumHeight(44);

    QHBoxLayout *printLayout = new QHBoxLayout(printBtn);
    printLayout->setContentsMargins(16, 8, 16, 8);
    printLayout->setSpacing(8);

    QSvgWidget *printIcon = new QSvgWidget();
    printIcon->load(QByteArray(printIconSvg));
    printIcon->setFixedSize(20, 20);

    QLabel *printText = new QLabel("打印病历");
    printText->setObjectName("buttonText");

    printLayout->addWidget(printIcon);
    printLayout->addWidget(printText);
    printLayout->addStretch();

    connect(printBtn, &QPushButton::clicked, this, &MedicalRecordWidget::onPrintRecordClicked);

    // 导出病历按钮
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

    connect(exportBtn, &QPushButton::clicked, this, &MedicalRecordWidget::onExportRecordClicked);

    actionLayout->addStretch();
    actionLayout->addWidget(printBtn);
    actionLayout->addWidget(exportBtn);
    actionLayout->addStretch();

    return actionFrame;
}

QFrame* MedicalRecordWidget::createSectionFrame(const QString &title, const QString &content)
{
    QFrame *sectionFrame = new QFrame();
    sectionFrame->setObjectName("contentSection");

    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionFrame);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    sectionLayout->setSpacing(8);

    // 标题
    QLabel *sectionTitle = new QLabel(title);
    sectionTitle->setObjectName("sectionTitle");

    // 下划线
    QFrame *underline = new QFrame();
    underline->setFrameShape(QFrame::HLine);
    underline->setObjectName("sectionUnderline");
    underline->setFixedHeight(1);

    // 内容
    QLabel *sectionContent = new QLabel(content);
    sectionContent->setObjectName("sectionContent");
    sectionContent->setWordWrap(true);
    sectionContent->setMinimumHeight(80);
    sectionContent->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    sectionLayout->addWidget(sectionTitle);
    sectionLayout->addWidget(underline);
    sectionLayout->addWidget(sectionContent);

    return sectionFrame;
}

QFrame* MedicalRecordWidget::createTableCell(const QString &label, const QString &value, bool isHeader)
{
    QFrame *cell = new QFrame();
    if (isHeader) {
        cell->setObjectName("tableHeaderCell");
    } else {
        cell->setObjectName("tableCell");
    }

    QHBoxLayout *cellLayout = new QHBoxLayout(cell);
    cellLayout->setContentsMargins(12, 8, 12, 8);
    cellLayout->setSpacing(4);

    if (!label.isEmpty()) {
        QLabel *labelWidget = new QLabel(label);
        labelWidget->setObjectName("cellLabel");
        cellLayout->addWidget(labelWidget);

        if (!value.isEmpty()) {
            QLabel *valueWidget = new QLabel(value);
            valueWidget->setObjectName("cellValue");
            cellLayout->addWidget(valueWidget);
        }
    } else {
        QLabel *valueWidget = new QLabel(value);
        if (isHeader) {
            valueWidget->setObjectName("cellHeaderText");
        } else {
            valueWidget->setObjectName("cellValue");
        }
        cellLayout->addWidget(valueWidget);
    }

    return cell;
}

void MedicalRecordWidget::setMedicalRecordData(const MedicalRecordData &data)
{
    recordData = data;
    // 重新初始化UI以更新显示的数据
    // 这里可以添加更新现有控件的逻辑
}

void MedicalRecordWidget::onBackButtonClicked()
{
    emit backRequested();
}

void MedicalRecordWidget::onPrintRecordClicked()
{
    // 实现打印功能
    // 这里可以调用系统的打印对话框
}

void MedicalRecordWidget::onExportRecordClicked()
{
    // 实现导出PDF功能
    // 这里可以使用QPrinter生成PDF文件
}

void MedicalRecordWidget::applyStyles()
{
    this->setStyleSheet(R"(
        #medicalRecordWidget {
            background-color: #f0f9ff;
        }

        /* 头部样式 */
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

        /* 滚动区域样式 */
        #recordScrollArea {
            background-color: white;
            border: 1px solid #e2e8f0;
            border-radius: 12px;
        }
        #recordContentWidget {
            background-color: white;
        }

        /* 病历头部样式 */
        #recordHeaderFrame {
            background-color: white;
        }
        #hospitalName {
            font-size: 24px;
            font-weight: bold;
            color: #1e293b;
            margin-bottom: 5px;
        }
        #recordTitle {
            font-size: 18px;
            font-weight: 600;
            color: #374151;
            margin-bottom: 10px;
        }
        #titleLine {
            background-color: #000000;
            border: none;
        }

        /* 患者信息表格样式 */
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
            margin-right: 4px;
        }
        #cellValue, #cellHeaderText {
            font-size: 14px;
            color: #1f2937;
        }
        #cellHeaderText {
            font-weight: 600;
            text-align: center;
        }

        /* 病历内容样式 */
        #recordContentFrame {
            background-color: white;
        }
        #contentSection {
            margin-bottom: 15px;
        }
        #sectionTitle {
            font-size: 16px;
            font-weight: 600;
            color: #1f2937;
            margin-bottom: 5px;
        }
        #sectionUnderline {
            background-color: #000000;
            border: none;
        }
        #sectionContent {
            font-size: 14px;
            color: #374151;
            line-height: 1.6;
            padding: 10px 0;
            text-align: justify;
        }

        /* 诊断表格样式 */
        #diagnosisFrame {
            background-color: white;
        }
        #diagnosisTable {
            border: 1px solid #000000;
        }
        #diagnosisTable #tableCell,
        #diagnosisTable #tableHeaderCell {
            min-height: 40px;
        }

        /* 操作按钮样式 */
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
}