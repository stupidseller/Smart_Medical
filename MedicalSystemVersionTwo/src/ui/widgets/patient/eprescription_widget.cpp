#include "eprescription_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QSvgWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QDebug>

// --- 将SVG图标定义放在这里 ---
// EPrescriptionWidget SVG Icons
static const char* backArrowIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-arrow-left"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";
static const char* diagnosisIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#409EFF" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-clipboard"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"></path><rect x="8" y="2" width="8" height="4" rx="1" ry="1"></rect></svg>)";
static const char* drugIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#409EFF" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-package"><line x1="16.5" y1="9.4" x2="7.5" y2="4.21"></line><path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"></path><polyline points="3.27 6.96 12 12.01 20.73 6.96"></polyline><line x1="12" y1="22.08" x2="12" y2="12"></line></svg>)";
static const char* adviceIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#409EFF" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-edit-3"><path d="M12 20h9"></path><path d="M16.5 3.5a2.121 2.121 0 0 1 3 3L7 19l-4 1 1-4L16.5 3.5z"></path></svg>)";
static const char* deleteIcon = R"(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-trash-2"><polyline points="3 6 5 6 21 6"></polyline><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path><line x1="10" y1="11" x2="10" y2="17"></line><line x1="14" y1="11" x2="14" y2="17"></line></svg>)";


EPrescriptionWidget::EPrescriptionWidget(QWidget *parent)
        : QWidget(parent)
{
    initUI();
    applyStyles();
}

EPrescriptionWidget::~EPrescriptionWidget()
{
}

void EPrescriptionWidget::initUI()
{
    this->setObjectName("ePrescriptionWidget");
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(createHeader());

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(30, 20, 30, 20);
    contentLayout->setSpacing(25);

    contentLayout->addWidget(createPatientInfoPanel());
    contentLayout->addWidget(createDiagnosisPanel());
    contentLayout->addWidget(createDrugsPanel());
    contentLayout->addWidget(createAdvicePanel());
    contentLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(createBottomButtons());
}

QWidget* EPrescriptionWidget::createHeader()
{
    auto *header = new QFrame();
    header->setObjectName("header");
    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(20, 10, 20, 10);

    auto *backButton = new QPushButton("返回");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &EPrescriptionWidget::backRequested);

    auto *title = new QLabel("电子处方");
    title->setObjectName("titleLabel");

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();

    return header;
}

QWidget* EPrescriptionWidget::createPatientInfoPanel()
{
    auto *panel = new QFrame();
    panel->setObjectName("patientInfoPanel");
    auto *layout = new QHBoxLayout(panel);
    layout->setSpacing(50);

    auto create_info_label = [](const QString& key, const QString& value) {
        auto *label = new QLabel(QString("<b>%1:</b> %2").arg(key, value));
        label->setObjectName("infoLabel");
        return label;
    };

    layout->addWidget(create_info_label("姓名", "张患者"));
    layout->addWidget(create_info_label("ID", "P202305001"));
    layout->addWidget(create_info_label("性别", "女"));
    layout->addWidget(create_info_label("年龄", "38岁"));
    layout->addStretch();

    return panel;
}

QWidget* EPrescriptionWidget::createSectionHeader(const QString &svgIcon, const QString &title)
{
    auto *header = new QWidget();
    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    auto *icon = new QSvgWidget();
    icon->load(QByteArray(svgIcon.toUtf8()));
    icon->setFixedSize(22, 22);

    auto *label = new QLabel(title);
    label->setObjectName("sectionTitleLabel");

    layout->addWidget(icon);
    layout->addWidget(label);
    layout->addStretch();

    return header;
}


QWidget* EPrescriptionWidget::createDiagnosisPanel()
{
    auto *panel = new QFrame();
    panel->setObjectName("sectionFrame");
    auto *layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    layout->addWidget(createSectionHeader(diagnosisIcon, "诊断信息"));

    auto *formLayout = new QGridLayout();
    formLayout->setColumnStretch(1, 1);
    formLayout->setColumnStretch(3, 1);
    formLayout->setHorizontalSpacing(20);
    formLayout->setVerticalSpacing(10);

    formLayout->addWidget(new QLabel("主要诊断"), 0, 0);
    mainDiagnosisEdit = new QLineEdit();
    mainDiagnosisEdit->setPlaceholderText("请输入主要诊断");
    formLayout->addWidget(mainDiagnosisEdit, 0, 1);

    formLayout->addWidget(new QLabel("科室"), 0, 2);
    departmentCombo = new QComboBox();
    departmentCombo->addItems({"心血管内科", "呼吸内科", "神经内科", "骨科"});
    formLayout->addWidget(departmentCombo, 0, 3);

    layout->addLayout(formLayout);
    return panel;
}

QWidget* EPrescriptionWidget::createDrugsPanel()
{
    auto *panel = new QFrame();
    panel->setObjectName("sectionFrame");
    auto *layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    layout->addWidget(createSectionHeader(drugIcon, "药品信息"));

    // --- Table Header ---
    auto *header = new QWidget();
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(10, 0, 10, 0);
    QStringList headers = {"药品名称", "规格", "数量", "用法用量", "天数", "操作"};
    int stretches[] = {3, 2, 1, 3, 1, 1};
    for(int i = 0; i < headers.size(); ++i) {
        auto *label = new QLabel(headers[i]);
        label->setObjectName("drugHeaderLabel");
        headerLayout->addWidget(label, stretches[i]);
    }
    layout->addWidget(header);

    // --- Dynamic Drug Rows ---
    auto *rowsContainer = new QWidget();
    drugRowsLayout = new QVBoxLayout(rowsContainer);
    drugRowsLayout->setContentsMargins(0, 0, 0, 0);
    drugRowsLayout->setSpacing(10);
    layout->addWidget(rowsContainer);
    addDrugRow(true); // Add the first pre-filled row
    addDrugRow();     // Add a second empty row

    // --- Add Button ---
    auto *addDrugButton = new QPushButton("+ 添加药品");
    addDrugButton->setObjectName("addDrugButton");
    connect(addDrugButton, &QPushButton::clicked, this, &EPrescriptionWidget::onAddDrugClicked);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(addDrugButton);
    layout->addLayout(buttonLayout);

    return panel;
}

void EPrescriptionWidget::addDrugRow(bool isFirstRow)
{
    auto *rowWidget = new QFrame();
    rowWidget->setObjectName("drugRowFrame");
    auto *layout = new QHBoxLayout(rowWidget);
    layout->setContentsMargins(10, 5, 10, 5);
    int stretches[] = {3, 2, 1, 3, 1, 1};

    // 药品名称
    auto *drugNameCombo = new QComboBox();
    drugNameCombo->addItems({"硝苯地平缓释片", "阿司匹林肠溶片", "阿托伐他汀钙片", "请选择药品"});
    drugNameCombo->setEditable(true);
    layout->addWidget(drugNameCombo, stretches[0]);

    // 规格
    auto *specEdit = new QLineEdit();
    layout->addWidget(specEdit, stretches[1]);

    // 数量
    auto *quantitySpin = new QSpinBox();
    quantitySpin->setRange(1, 999);
    layout->addWidget(quantitySpin, stretches[2]);

    // 用法用量
    auto *dosageEdit = new QLineEdit();
    layout->addWidget(dosageEdit, stretches[3]);

    // 天数
    auto *daysSpin = new QSpinBox();
    daysSpin->setRange(1, 365);
    layout->addWidget(daysSpin, stretches[4]);

    // 操作
    auto *deleteButton = new QPushButton();
    deleteButton->setObjectName("deleteButton");
    auto* icon = new QSvgWidget(deleteButton);
    icon->load(QByteArray(deleteIcon));
    icon->setFixedSize(16,16);
    QHBoxLayout* btnLayout = new QHBoxLayout(deleteButton);
    btnLayout->addWidget(icon);
    btnLayout->setContentsMargins(0,0,0,0);
    deleteButton->setFixedSize(32, 32);

    // 使用 lambda 表达式连接信号，可以轻松传递行信息
    connect(deleteButton, &QPushButton::clicked, this, [this, rowWidget]{
        removeDrugRow(rowWidget);
    });
    layout->addWidget(deleteButton, stretches[5], Qt::AlignCenter);

    // 如果是第一行，填充示例数据
    if (isFirstRow) {
        drugNameCombo->setCurrentText("硝苯地平缓释片");
        specEdit->setText("30mgx7片");
        quantitySpin->setValue(4);
        dosageEdit->setText("每日1次, 每次1片");
        daysSpin->setValue(28);
    } else {
        drugNameCombo->setCurrentIndex(3);
        quantitySpin->setValue(1);
        daysSpin->setValue(7);
    }

    drugRowsLayout->addWidget(rowWidget);
    drugRowWidgets.append(rowWidget);
}

void EPrescriptionWidget::onAddDrugClicked()
{
    addDrugRow();
}

void EPrescriptionWidget::removeDrugRow(QWidget* rowWidget)
{
    if (drugRowWidgets.size() <= 1) {
        // 至少保留一行，防止全部删除
        qDebug() << "Cannot delete the last drug row.";
        return;
    }
    drugRowsLayout->removeWidget(rowWidget);
    drugRowWidgets.removeOne(rowWidget);
    rowWidget->deleteLater(); // 使用deleteLater()安全地删除对象
}


QWidget* EPrescriptionWidget::createAdvicePanel()
{
    auto *panel = new QFrame();
    panel->setObjectName("sectionFrame");
    auto *layout = new QVBoxLayout(panel);
    layout->setSpacing(15);
    layout->addWidget(createSectionHeader(adviceIcon, "医嘱与建议"));

    auto *formLayout = new QGridLayout();
    formLayout->setVerticalSpacing(10);

    formLayout->addWidget(new QLabel("用药指导"), 0, 0, Qt::AlignTop);
    medicationGuidanceEdit = new QTextEdit();
    medicationGuidanceEdit->setPlaceholderText("请输入用药指导...");
    medicationGuidanceEdit->setText("请严格按时服药，不要随意停药或调整剂量。服药期间注意监测血压，如出现头晕、乏力等不适请及时就医。");
    medicationGuidanceEdit->setMinimumHeight(100);
    formLayout->addWidget(medicationGuidanceEdit, 0, 1);

    formLayout->addWidget(new QLabel("注意事项"), 1, 0, Qt::AlignTop);
    precautionsEdit = new QTextEdit();
    precautionsEdit->setPlaceholderText("请输入注意事项...");
    precautionsEdit->setMinimumHeight(100);
    formLayout->addWidget(precautionsEdit, 1, 1);

    formLayout->setColumnStretch(1, 1);

    layout->addLayout(formLayout);
    return panel;
}


QWidget* EPrescriptionWidget::createBottomButtons()
{
    auto *panel = new QFrame();
    panel->setObjectName("bottomButtonPanel");
    auto *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(30, 15, 30, 15);
    layout->setSpacing(15);

    auto *saveButton = new QPushButton("保存处方");
    saveButton->setObjectName("saveButton");

    auto *printButton = new QPushButton("打印处方");
    printButton->setObjectName("printButton");

    layout->addStretch();
    layout->addWidget(saveButton);
    layout->addWidget(printButton);

    return panel;
}


void EPrescriptionWidget::applyStyles()
{
    this->setStyleSheet(R"(
        #ePrescriptionWidget {
            background-color: #F5F7FA;
        }
        #scrollArea {
            border: none;
        }
        #contentWidget {
            background-color: #F5F7FA;
        }
        #header {
            background-color: #FFFFFF;
            border-bottom: 1px solid #E4E7ED;
        }
        #titleLabel {
            font-size: 18px;
            font-weight: bold;
            color: #303133;
        }
        #backButton {
            background-color: transparent;
            border: none;
            color: #606266;
            font-size: 14px;
        }
        #backButton:hover {
            color: #409EFF;
        }
        #patientInfoPanel {
            background-color: #ECF5FF;
            border: 1px solid #D9ECFF;
            border-radius: 8px;
            padding: 15px 25px;
        }
        #infoLabel {
            font-size: 14px;
            color: #303133;
        }
        #sectionFrame {
            background-color: #FFFFFF;
            border-radius: 8px;
            border: 1px solid #E4E7ED;
            padding: 20px;
        }
        #sectionTitleLabel {
            font-size: 16px;
            font-weight: bold;
            color: #303133;
        }
        QLabel {
            font-size: 14px;
            color: #606266;
        }
        QLineEdit, QComboBox, QSpinBox, QTextEdit {
            background-color: #FFFFFF;
            border: 1px solid #DCDFE6;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
            color: #606266;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QTextEdit:focus {
            border-color: #409EFF;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox::down-arrow {
            image: url(data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="%23909399" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6 9 12 15 18 9"></polyline></svg>);
        }
        #drugHeaderLabel {
            font-weight: bold;
            color: #303133;
            padding-left: 5px;
        }
        #drugRowFrame {
            background-color: #FAFAFA;
            border-radius: 5px;
        }
        #addDrugButton {
            background-color: #F0F9EB;
            color: #67C23A;
            border: 1px dashed #C2E7B0;
            border-radius: 4px;
            padding: 8px 15px;
            font-size: 14px;
        }
        #addDrugButton:hover {
            background-color: #67C23A;
            color: #FFFFFF;
            border-style: solid;
        }
        #deleteButton {
            background-color: #FEF0F0;
            border: 1px solid #FBC4C4;
            border-radius: 50%;
            color: #F56C6C;
        }
        #deleteButton:hover {
            background-color: #F56C6C;
            border-color: #F56C6C;
            color: white;
        }
        /* Make icon color change on hover */
        #deleteButton:hover QSvgWidget {
            stroke: white;
        }
        #bottomButtonPanel {
            background-color: #FFFFFF;
            border-top: 1px solid #E4E7ED;
        }
        QPushButton {
             font-size: 14px;
             padding: 10px 20px;
             border-radius: 4px;
        }
        #saveButton {
            background-color: #409EFF;
            color: white;
            border: none;
        }
        #saveButton:hover {
            background-color: #66B1FF;
        }
        #printButton {
            background-color: #FFFFFF;
            color: #606266;
            border: 1px solid #DCDFE6;
        }
        #printButton:hover {
            color: #409EFF;
            border-color: #C6E2FF;
            background-color: #ECF5FF;
        }
    )");
}