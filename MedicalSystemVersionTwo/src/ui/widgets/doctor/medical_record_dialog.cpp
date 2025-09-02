#include "medical_record_dialog.h"
#include <QApplication>
#include <QScreen>

MedicalRecordDialog::MedicalRecordDialog(const PatientData &patientData, QWidget *parent)
        : QDialog(parent), m_patientData(patientData)
{
    setWindowTitle("病历记录单");
    setModal(true);

    // 设置窗口大小为屏幕的80%
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int width = screenGeometry.width() * 0.8;
    int height = screenGeometry.height() * 0.8;
    resize(width, height);

    // 居中显示
    move((screenGeometry.width() - width) / 2, (screenGeometry.height() - height) / 2);

    initUI();
    applyStyles();
    loadPatientData(patientData);
}

void MedicalRecordDialog::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 25, 30, 25);

    // 添加各个部分
    mainLayout->addWidget(createHeader());
    mainLayout->addWidget(createPatientInfo());
    mainLayout->addWidget(createMainContent(), 1);

    // 关闭按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *closeBtn = new QPushButton("关闭");
    closeBtn->setObjectName("closeButton");
    closeBtn->setMinimumSize(100, 35);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);

    mainLayout->addLayout(buttonLayout);
}

QWidget* MedicalRecordDialog::createHeader()
{
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(headerWidget);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *hospitalLabel = new QLabel("智慧医院");
    hospitalLabel->setObjectName("hospitalTitle");

    QLabel *titleLabel = new QLabel("病历记录单");
    titleLabel->setObjectName("recordTitle");

    // 分割线
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("titleLine");

    layout->addWidget(hospitalLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(line);

    return headerWidget;
}

QWidget* MedicalRecordDialog::createPatientInfo()
{
    QFrame *infoFrame = new QFrame();
    infoFrame->setObjectName("patientInfoFrame");

    QGridLayout *grid = new QGridLayout(infoFrame);
    grid->setSpacing(15);
    grid->setContentsMargins(20, 15, 20, 15);

    // 创建标签和数据标签
    grid->addWidget(new QLabel("姓名"), 0, 0);
    m_nameLabel = new QLabel();
    m_nameLabel->setObjectName("dataLabel");
    grid->addWidget(m_nameLabel, 0, 1);

    grid->addWidget(new QLabel("性别"), 0, 2);
    m_genderLabel = new QLabel();
    m_genderLabel->setObjectName("dataLabel");
    grid->addWidget(m_genderLabel, 0, 3);

    grid->addWidget(new QLabel("年龄"), 0, 4);
    m_ageLabel = new QLabel();
    m_ageLabel->setObjectName("dataLabel");
    grid->addWidget(m_ageLabel, 0, 5);

    grid->addWidget(new QLabel("病房号"), 1, 0);
    m_patientIdLabel = new QLabel();
    m_patientIdLabel->setObjectName("dataLabel");
    grid->addWidget(m_patientIdLabel, 1, 1);

    grid->addWidget(new QLabel("科室"), 1, 2);
    m_deptLabel = new QLabel();
    m_deptLabel->setObjectName("dataLabel");
    grid->addWidget(m_deptLabel, 1, 3);

    grid->addWidget(new QLabel("床号"), 1, 4);
    m_bedLabel = new QLabel();
    m_bedLabel->setObjectName("dataLabel");
    grid->addWidget(m_bedLabel, 1, 5);

    grid->addWidget(new QLabel("入院日期"), 2, 0);
    m_admitDateLabel = new QLabel();
    m_admitDateLabel->setObjectName("dataLabel");
    grid->addWidget(m_admitDateLabel, 2, 1);

    grid->addWidget(new QLabel("记录日期"), 2, 2);
    m_recordDateLabel = new QLabel();
    m_recordDateLabel->setObjectName("dataLabel");
    grid->addWidget(m_recordDateLabel, 2, 3);

    grid->addWidget(new QLabel("记录时间"), 2, 4);
    m_recordTimeLabel = new QLabel();
    m_recordTimeLabel->setObjectName("dataLabel");
    grid->addWidget(m_recordTimeLabel, 2, 5);

    return infoFrame;
}

QWidget* MedicalRecordDialog::createMainContent()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 20, 20, 20);

    // 主诉部分
    QLabel *chiefTitle = new QLabel("主诉");
    chiefTitle->setObjectName("sectionTitle");
    QFrame *chiefLine = new QFrame();
    chiefLine->setFrameShape(QFrame::HLine);
    chiefLine->setFrameShadow(QFrame::Sunken);
    chiefLine->setObjectName("sectionLine");

    m_chiefComplaintEdit = new QTextEdit();
    m_chiefComplaintEdit->setObjectName("contentText");
    m_chiefComplaintEdit->setMaximumHeight(80);
    m_chiefComplaintEdit->setReadOnly(true);

    layout->addWidget(chiefTitle);
    layout->addWidget(chiefLine);
    layout->addWidget(m_chiefComplaintEdit);

    // 现病史部分
    QLabel *presentTitle = new QLabel("现病史");
    presentTitle->setObjectName("sectionTitle");
    QFrame *presentLine = new QFrame();
    presentLine->setFrameShape(QFrame::HLine);
    presentLine->setFrameShadow(QFrame::Sunken);
    presentLine->setObjectName("sectionLine");

    m_presentIllnessEdit = new QTextEdit();
    m_presentIllnessEdit->setObjectName("contentText");
    m_presentIllnessEdit->setMaximumHeight(120);
    m_presentIllnessEdit->setReadOnly(true);

    layout->addWidget(presentTitle);
    layout->addWidget(presentLine);
    layout->addWidget(m_presentIllnessEdit);

    // 既往史部分
    QLabel *pastTitle = new QLabel("既往史");
    pastTitle->setObjectName("sectionTitle");
    QFrame *pastLine = new QFrame();
    pastLine->setFrameShape(QFrame::HLine);
    pastLine->setFrameShadow(QFrame::Sunken);
    pastLine->setObjectName("sectionLine");

    m_pastHistoryEdit = new QTextEdit();
    m_pastHistoryEdit->setObjectName("contentText");
    m_pastHistoryEdit->setMaximumHeight(100);
    m_pastHistoryEdit->setReadOnly(true);

    layout->addWidget(pastTitle);
    layout->addWidget(pastLine);
    layout->addWidget(m_pastHistoryEdit);

    // 诊断部分
    layout->addWidget(createDiagnosisSection());

    scrollArea->setWidget(contentWidget);
    return scrollArea;
}

QWidget* MedicalRecordDialog::createDiagnosisSection()
{
    QWidget *diagnosisWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(diagnosisWidget);
    layout->setSpacing(10);

    QLabel *diagnosisTitle = new QLabel("诊断");
    diagnosisTitle->setObjectName("sectionTitle");
    QFrame *diagnosisLine = new QFrame();
    diagnosisLine->setFrameShape(QFrame::HLine);
    diagnosisLine->setFrameShadow(QFrame::Sunken);
    diagnosisLine->setObjectName("sectionLine");

    // 诊断表格
    QFrame *diagnosisFrame = new QFrame();
    diagnosisFrame->setObjectName("diagnosisTable");
    QGridLayout *diagnosisGrid = new QGridLayout(diagnosisFrame);
    diagnosisGrid->setSpacing(1);
    diagnosisGrid->setContentsMargins(1, 1, 1, 1);

    // 表头
    QLabel *typeHeader = new QLabel("诊断类型");
    typeHeader->setObjectName("tableHeader");
    QLabel *contentHeader = new QLabel("诊断内容");
    contentHeader->setObjectName("tableHeader");
    QLabel *icdHeader = new QLabel("ICD编码");
    icdHeader->setObjectName("tableHeader");

    diagnosisGrid->addWidget(typeHeader, 0, 0);
    diagnosisGrid->addWidget(contentHeader, 0, 1);
    diagnosisGrid->addWidget(icdHeader, 0, 2);

    // 主要诊断
    QLabel *mainType = new QLabel("主要诊断");
    mainType->setObjectName("tableCell");
    m_diagnosisLabel = new QLabel();
    m_diagnosisLabel->setObjectName("tableCell");
    m_icdCodeLabel = new QLabel();
    m_icdCodeLabel->setObjectName("tableCell");

    diagnosisGrid->addWidget(mainType, 1, 0);
    diagnosisGrid->addWidget(m_diagnosisLabel, 1, 1);
    diagnosisGrid->addWidget(m_icdCodeLabel, 1, 2);

    // 设置列宽比例
    diagnosisGrid->setColumnStretch(0, 1);
    diagnosisGrid->setColumnStretch(1, 3);
    diagnosisGrid->setColumnStretch(2, 1);

    layout->addWidget(diagnosisTitle);
    layout->addWidget(diagnosisLine);
    layout->addWidget(diagnosisFrame);

    return diagnosisWidget;
}

void MedicalRecordDialog::loadPatientData(const PatientData &data)
{
    m_nameLabel->setText(data.name);
    m_genderLabel->setText("女");
    m_ageLabel->setText(QString::number(data.age) + "岁");
    m_patientIdLabel->setText(data.patientId);
    m_deptLabel->setText(data.department);
    m_bedLabel->setText("302床");
    m_admitDateLabel->setText("2025-08-30");
    m_recordDateLabel->setText("2025-08-31");
    m_recordTimeLabel->setText("18:35");

    // 设置病历内容
    m_chiefComplaintEdit->setText("反复胸闷、胸痛3天，加重2小时。");

    m_presentIllnessEdit->setText("患者3天前无明显诱因出现胸闷、胸痛，位于心前区，呈压榨性疼痛，"
                                  "持续95-10分钟，休息后可缓解。2小时前症状加重，胸痛持续不缓解，"
                                  "伴出汗、恶心，无发热、咳嗽、咳痰、胸痛。自测血压偏高，"
                                  "日测症状不缓解，为求进一步诊治来院。");

    m_pastHistoryEdit->setText("高血压病史5年，最高血压180/100mmHg，现服降药治疗，"
                               "否认糖尿病、冠心病史，否认手术外伤史，否认药物过敏史。");

    m_diagnosisLabel->setText("急性的冠心病情况");
    m_icdCodeLabel->setText("I21.0");
}

void MedicalRecordDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog {
            background-color: white;
        }
        #hospitalTitle {
            font-size: 24px;
            font-weight: bold;
            color: #1a1a1a;
            margin: 10px 0;
        }
        #recordTitle {
            font-size: 20px;
            font-weight: 600;
            color: #333;
            margin-bottom: 10px;
        }
        #titleLine {
            background-color: #333;
            max-height: 2px;
            margin: 10px 0;
        }
        #patientInfoFrame {
            border: 2px solid #333;
            background-color: #fafafa;
            margin: 10px 0;
        }
        QLabel {
            font-size: 13px;
            color: #333;
            font-weight: 500;
        }
        #dataLabel {
            font-weight: normal;
            color: #555;
            padding: 2px 5px;
        }
        #sectionTitle {
            font-size: 16px;
            font-weight: bold;
            color: #1a1a1a;
            margin: 15px 0 5px 0;
        }
        #sectionLine {
            background-color: #666;
            max-height: 1px;
            margin: 5px 0 15px 0;
        }
        #contentText {
            border: 1px solid #ccc;
            background-color: #fdfdfd;
            font-size: 13px;
            line-height: 1.6;
            padding: 10px;
        }
        #diagnosisTable {
            border: 2px solid #333;
            background-color: white;
        }
        #tableHeader {
            background-color: #f5f5f5;
            border: 1px solid #333;
            font-weight: bold;
            padding: 8px;
            text-align: center;
        }
        #tableCell {
            border: 1px solid #333;
            padding: 8px;
            background-color: white;
        }
        #closeButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            font-weight: 500;
            padding: 8px 16px;
        }
        #closeButton:hover {
            background-color: #2563eb;
        }
    )");
}