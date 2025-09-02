#include "prescription_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QFrame>
#include <QSvgWidget>

// 使用静态SVG数据简化资源依赖
static const char* backIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";
static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* trashIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="3 6 5 6 21 6"></polyline><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path><line x1="10" y1="11" x2="10" y2="17"></line><line x1="14" y1="11" x2="14" y2="17"></line></svg>)";
static const char* diagnosisIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14.5 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7.5L14.5 2z"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="12" y1="18" x2="12" y2="12"></line><line x1="9" y1="15" x2="15" y2="15"></line></svg>)";
static const char* drugIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="8" x2="12" y2="16"></line><line x1="8" y1="12" x2="16" y2="12"></line></svg>)";
static const char* adviceIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.44 11.05l-9.19 9.19a6 6 0 0 1-8.49-8.49l9.19-9.19a4 4 0 0 1 5.66 5.66l-9.2 9.19a2 2 0 0 1-2.83-2.83l8.49-8.48"></path></svg>)";


PrescriptionWidget::PrescriptionWidget(QWidget *parent)
        : QWidget(parent)
{
    initUI();
    applyStyles();
}

PrescriptionWidget::~PrescriptionWidget() {}

void PrescriptionWidget::initUI()
{
    this->setObjectName("prescriptionWidget");
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. Header
    mainLayout->addWidget(createHeader());

    // 2. Scroll Area for Content
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(80, 30, 80, 50);
    contentLayout->setSpacing(30);

    // Add sections to content layout
    contentLayout->addWidget(createPatientSelection());
    contentLayout->addWidget(createDiagnosisSection());
    contentLayout->addWidget(createDrugsSection());
    contentLayout->addWidget(createAdviceSection());
    contentLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);

    // 3. Footer
    mainLayout->addWidget(createFooterButtons());
}

QWidget* PrescriptionWidget::createHeader() {
    auto *header = new QWidget();
    header->setObjectName("header");
    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(20, 10, 20, 10);

    auto* backButton = new QPushButton(" ");
    backButton->setObjectName("backButton");
    auto* backIconWidget = new QSvgWidget();
    backIconWidget->load(QByteArray(backIcon));
    backIconWidget->setFixedSize(20, 20);
    auto* backLayout = new QHBoxLayout(backButton);
    backLayout->addWidget(backIconWidget);
    connect(backButton, &QPushButton::clicked, this, &PrescriptionWidget::backRequested);

    auto* title = new QLabel("开具电子处方");
    title->setObjectName("titleLabel");

    auto* doctorInfo = new QWidget();
    auto* doctorLayout = new QHBoxLayout(doctorInfo);
    doctorLayout->setContentsMargins(0,0,0,0);
    auto* userIconWidget = new QSvgWidget();
    userIconWidget->load(QByteArray(userIconSvg));
    userIconWidget->setFixedSize(22,22);
    auto* doctorName = new QLabel("王医生");
    doctorName->setObjectName("doctorNameLabel");
    doctorLayout->addWidget(userIconWidget);
    doctorLayout->addWidget(doctorName);

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(doctorInfo);

    return header;
}

QWidget* createSectionTitle(const QString& svgIcon, const QString& title) {
    auto *container = new QWidget();
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(10);
    auto* icon = new QSvgWidget();
    icon->load(QByteArray(svgIcon.toUtf8()));
    icon->setFixedSize(22, 22);
    auto* label = new QLabel(title);
    label->setObjectName("sectionTitle");
    layout->addWidget(icon);
    layout->addWidget(label);
    layout->addStretch();
    return container;
}


QWidget* PrescriptionWidget::createPatientSelection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);

    layout->addWidget(createSectionTitle(userIconSvg, "选择患者"));

    auto* patientCombo = new QComboBox();
    patientCombo->addItem("张患者 (P202305001)");
    patientCombo->addItem("李患者 (P202305002)");

    auto* patientInfoContainer = new QWidget();
    patientInfoContainer->setObjectName("patientInfoContainer");
    auto* infoLayout = new QHBoxLayout(patientInfoContainer);
    infoLayout->setContentsMargins(20, 15, 20, 15);
    infoLayout->setSpacing(40);
    infoLayout->addWidget(new QLabel("姓名：张患者"));
    infoLayout->addWidget(new QLabel("ID: P202305001"));
    infoLayout->addWidget(new QLabel("性别：女"));
    infoLayout->addWidget(new QLabel("年龄：38岁"));
    infoLayout->addStretch();

    layout->addWidget(patientCombo);
    layout->addWidget(patientInfoContainer);

    return container;
}

QWidget* PrescriptionWidget::createDiagnosisSection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(createSectionTitle(diagnosisIcon, "诊断信息"));

    auto* gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    auto* mainDiagnosisEdit = new QLineEdit();
    mainDiagnosisEdit->setPlaceholderText("请输入主要诊断");
    gridLayout->addWidget(new QLabel("主要诊断"), 0, 0);
    gridLayout->addWidget(mainDiagnosisEdit, 0, 1);

    auto* departmentCombo = new QComboBox();
    departmentCombo->addItem("心血管内科");
    departmentCombo->addItem("呼吸科");
    departmentCombo->addItem("神经科");
    gridLayout->addWidget(new QLabel("科室"), 0, 2);
    gridLayout->addWidget(departmentCombo, 0, 3);

    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(3, 1);

    layout->addLayout(gridLayout);
    return container;
}

QWidget* PrescriptionWidget::createDrugsSection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(createSectionTitle(drugIcon, "药品信息"));

    // Table Header
    auto* header = new QWidget();
    header->setObjectName("drugsHeader");
    auto* headerLayout = new QGridLayout(header);
    headerLayout->addWidget(new QLabel("药品名称"), 0, 0, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("规格"), 0, 1, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("数量"), 0, 2, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("用法用量"), 0, 3, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("天数"), 0, 4, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("操作"), 0, 5, Qt::AlignCenter);
    headerLayout->setColumnStretch(0, 3);
    headerLayout->setColumnStretch(1, 2);
    headerLayout->setColumnStretch(2, 1);
    headerLayout->setColumnStretch(3, 3);
    headerLayout->setColumnStretch(4, 1);
    headerLayout->setColumnStretch(5, 1);

    layout->addWidget(header);

    m_drugsLayout = new QVBoxLayout();
    m_drugsLayout->setSpacing(10);
    // Add initial drug rows from the image
    m_drugsLayout->addWidget(createDrugRow());
    m_drugsLayout->addWidget(createDrugRow());

    layout->addLayout(m_drugsLayout);

    auto* addDrugButton = new QPushButton("+ 添加药品");
    addDrugButton->setObjectName("addDrugButton");
    connect(addDrugButton, &QPushButton::clicked, this, &PrescriptionWidget::onAddDrugClicked);

    auto* buttonContainer = new QHBoxLayout();
    buttonContainer->addWidget(addDrugButton);
    buttonContainer->addStretch();
    layout->addLayout(buttonContainer);

    return container;
}

void PrescriptionWidget::onAddDrugClicked() {
    if (m_drugsLayout) {
        m_drugsLayout->addWidget(createDrugRow());
    }
}

void PrescriptionWidget::onRemoveDrugClicked()
{
    // Sender is the remove button
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // The button's parent is the drug row widget
        QWidget* drugRow = button->parentWidget();
        if (drugRow) {
            drugRow->hide();
            drugRow->deleteLater();
        }
    }
}


QWidget* PrescriptionWidget::createDrugRow() {
    auto* row = new QWidget();
    row->setObjectName("drugRow");
    auto* layout = new QGridLayout(row);
    layout->setContentsMargins(0,0,0,0);

    auto* drugName = new QComboBox();
    drugName->addItem("硝苯地平控释片");
    drugName->addItem("阿司匹林肠溶片");
    drugName->setEditable(true); // Allow searching

    auto* spec = new QLineEdit("30mgx7片");
    auto* quantity = new QLineEdit("4");
    auto* dosage = new QLineEdit("每日1次, 每次1片");
    auto* days = new QLineEdit("28");
    auto* removeBtn = new QPushButton();
    removeBtn->setObjectName("removeDrugButton");
    auto* trashIconWidget = new QSvgWidget(removeBtn);
    trashIconWidget->load(QByteArray(trashIcon));
    trashIconWidget->setFixedSize(16,16);
    trashIconWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    auto* btnLayout = new QHBoxLayout(removeBtn);
    btnLayout->addWidget(trashIconWidget);

    connect(removeBtn, &QPushButton::clicked, this, &PrescriptionWidget::onRemoveDrugClicked);

    layout->addWidget(drugName, 0, 0);
    layout->addWidget(spec, 0, 1);
    layout->addWidget(quantity, 0, 2);
    layout->addWidget(dosage, 0, 3);
    layout->addWidget(days, 0, 4);
    layout->addWidget(removeBtn, 0, 5, Qt::AlignCenter);

    layout->setColumnStretch(0, 3);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 1);
    layout->setColumnStretch(3, 3);
    layout->setColumnStretch(4, 1);
    layout->setColumnStretch(5, 1);

    return row;
}


QWidget* PrescriptionWidget::createAdviceSection() {
    auto* container = new QWidget();
    container->setObjectName("sectionContainer");
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(createSectionTitle(adviceIcon, "医嘱与建议"));

    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);

    auto* guidanceEdit = new QTextEdit();
    guidanceEdit->setPlaceholderText("请严格按时服药，不要随意停药或调整剂量。服药期间注意监测血压，如出现头晕、乏力等不适请及时就医。");
    guidanceEdit->setMinimumHeight(100);

    auto* precautionsEdit = new QTextEdit();
    precautionsEdit->setPlaceholderText("定期复查肝肾功能和血脂水平，保持低盐低脂饮食，适量运动，控制体重。");
    precautionsEdit->setMinimumHeight(100);

    formLayout->addRow("用药指导", guidanceEdit);
    formLayout->addRow("注意事项", precautionsEdit);

    layout->addLayout(formLayout);
    return container;
}


QWidget* PrescriptionWidget::createFooterButtons() {
    auto* footer = new QWidget();
    footer->setObjectName("footer");
    auto* layout = new QHBoxLayout(footer);
    layout->setContentsMargins(0, 15, 80, 15);
    layout->addStretch();

    auto* cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("cancelButton");

    auto* saveButton = new QPushButton("暂存");
    saveButton->setObjectName("saveButton");

    auto* issueButton = new QPushButton("开具处方");
    issueButton->setObjectName("issueButton");

    layout->addWidget(cancelButton);
    layout->addWidget(saveButton);
    layout->addWidget(issueButton);

    return footer;
}


void PrescriptionWidget::applyStyles() {
    this->setStyleSheet(R"(
        #prescriptionWidget, #contentWidget {
            background-color: #F7FAFC;
        }
        #scrollArea {
            border: none;
        }
        #header {
            background-color: white;
            border-bottom: 1px solid #E2E8F0;
        }
        #backButton {
            background-color: transparent;
            border: none;
            color: #4A5568;
            font-size: 14px;
            padding: 8px 12px;
        }
        #backButton:hover {
            background-color: #F7FAFC;
        }
        #backButton QSvgWidget {
            stroke: #4A5568;
        }
        #titleLabel {
            font-size: 18px;
            font-weight: bold;
            color: #2D3748;
        }
        #doctorNameLabel {
            font-size: 14px;
            color: #4A5568;
        }
        #header QSvgWidget {
            stroke: #718096;
        }

        #sectionContainer {
            background-color: white;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            padding: 20px;
        }

        #sectionTitle {
            font-size: 16px;
            font-weight: bold;
            color: #2D3748;
        }
        #sectionContainer QSvgWidget {
             stroke: #3182CE;
        }

        #patientInfoContainer {
            background-color: #F7FAFC;
            border-radius: 6px;
            margin-top: 10px;
        }
        #patientInfoContainer QLabel {
            color: #4A5568;
        }

        QComboBox, QLineEdit, QTextEdit {
            border: 1px solid #CBD5E0;
            border-radius: 6px;
            padding: 8px 12px;
            background-color: white;
            font-size: 14px;
        }
        QComboBox:focus, QLineEdit:focus, QTextEdit:focus {
            border-color: #3182CE;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: none;
        }

        #drugsHeader {
            background-color: #EDF2F7;
            border-radius: 6px;
            padding: 10px 0;
            margin-bottom: 10px;
        }
        #drugsHeader QLabel {
            font-weight: bold;
            color: #4A5568;
        }

        #drugRow {
            padding: 5px 0;
        }
        #removeDrugButton {
            background-color: #FFF5F5;
            border: 1px solid #E53E3E;
            color: #E53E3E;
            border-radius: 6px;
            padding: 5px;
        }
        #removeDrugButton:hover {
            background-color: #E53E3E;
            color: white;
        }
        #removeDrugButton QSvgWidget {
            stroke: #E53E3E;
        }
         #removeDrugButton:hover QSvgWidget {
            stroke: white;
        }

        #addDrugButton {
            background-color: #E6FFFA;
            color: #2C7A7B;
            border: 1px dashed #38B2AC;
            font-weight: bold;
            max-width: 120px;
        }
        #addDrugButton:hover {
            background-color: #B2F5EA;
        }

        #footer {
            background-color: white;
            border-top: 1px solid #E2E8F0;
        }
        #footer QPushButton {
            padding: 10px 25px;
            font-size: 14px;
            border-radius: 6px;
            font-weight: bold;
        }
        #cancelButton {
            background-color: #E2E8F0;
            color: #4A5568;
            border: none;
        }
        #cancelButton:hover {
            background-color: #CBD5E0;
        }
        #saveButton {
            background-color: white;
            color: #3182CE;
            border: 1px solid #3182CE;
        }
        #saveButton:hover {
            background-color: #EBF8FF;
        }
        #issueButton {
            background-color: #3182CE;
            color: white;
            border: none;
        }
        #issueButton:hover {
            background-color: #2B6CB0;
        }
    )");
}