#include "drugsearch.h"
#include <QDebug>
#include <QMessageBox> // For demonstration of "查看" button

DrugSearch::DrugSearch(QWidget *parent) :
    QWidget(parent)
{
    setupUI();
}

DrugSearch::~DrugSearch()
{
}

void DrugSearch::setupUI()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header Layout
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);
    backButton = new QPushButton("返回主页面"); // You can use an icon here
    backButton->setIcon(QIcon(":/icons/back_arrow.png")); // Assuming you have a resource file with an icon
    backButton->setIconSize(QSize(24, 24));
    backButton->setStyleSheet("QPushButton { border: none; font-size: 16px; color: #333; }");
    connect(backButton, &QPushButton::clicked, this, &DrugSearch::on_backButton_clicked);
    headerLayout->addWidget(backButton);
    QLabel *titleLabel = new QLabel("药品查看");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(); // Pushes content to the left

    mainLayout->addLayout(headerLayout);

    // Search and Filter Layout
    QHBoxLayout *searchFilterLayout = new QHBoxLayout();
    searchFilterLayout->setSpacing(10);

    searchLineEdit = new QLineEdit();
    searchLineEdit->setPlaceholderText("搜索药品名 / 通用名 / 适应症...");
    searchLineEdit->setFixedSize(300, 35);
    searchLineEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #ccc; border-radius: 5px; }");
    searchFilterLayout->addWidget(searchLineEdit);

    categoryComboBox = new QComboBox();
    categoryComboBox->addItem("全部类别");
    categoryComboBox->addItem("消化系统");
    categoryComboBox->addItem("感染/抗生素");
    categoryComboBox->addItem("内分泌");
    categoryComboBox->addItem("心血管");
    categoryComboBox->setFixedSize(120, 35);
    categoryComboBox->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #ccc; border-radius: 5px; }");
    searchFilterLayout->addWidget(categoryComboBox);

    dosageFormComboBox = new QComboBox();
    dosageFormComboBox->addItem("全部剂型");
    dosageFormComboBox->addItem("片剂");
    dosageFormComboBox->addItem("胶囊");
    dosageFormComboBox->setFixedSize(120, 35);
    dosageFormComboBox->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #ccc; border-radius: 5px; }");
    searchFilterLayout->addWidget(dosageFormComboBox);

    resetButton = new QPushButton("重置");
    resetButton->setFixedSize(80, 35);
    resetButton->setStyleSheet("QPushButton { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 5px; padding: 5px; }");
    connect(resetButton, &QPushButton::clicked, this, &DrugSearch::on_resetButton_clicked);
    searchFilterLayout->addWidget(resetButton);

    searchButton = new QPushButton("搜索");
    searchButton->setFixedSize(80, 35);
    searchButton->setStyleSheet("QPushButton { background-color: #007bff; color: white; border-radius: 5px; padding: 5px; }");
    connect(searchButton, &QPushButton::clicked, this, &DrugSearch::on_searchButton_clicked);
    searchFilterLayout->addWidget(searchButton);

    mainLayout->addLayout(searchFilterLayout);

    // Filter Buttons (Tags)
    QHBoxLayout *tagFilterLayout = new QHBoxLayout();
    tagFilterLayout->setSpacing(10);

    otcButton = new QPushButton("OTC(非处方)");
    prescriptionButton = new QPushButton("处方药");
    childrenButton = new QPushButton("儿童慎用");
    pregnancyForbiddenButton = new QPushButton("妊娠禁用");
    liverKidneyUnsuitableButton = new QPushButton("肝功能不全慎用");
    kidneyUnsuitableButton = new QPushButton("肾功能不全慎用");

    QList<QPushButton*> filterButtons = {otcButton, prescriptionButton, childrenButton,
                                         pregnancyForbiddenButton, liverKidneyUnsuitableButton, kidneyUnsuitableButton};

    for (QPushButton *btn : filterButtons) {
        btn->setCheckable(true); // Make buttons toggleable
        btn->setStyleSheet("QPushButton { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 15px; padding: 8px 15px; }"
                           "QPushButton:checked { background-color: #007bff; color: white; border: 1px solid #007bff; }");
        tagFilterLayout->addWidget(btn);
    }
    tagFilterLayout->addStretch();
    mainLayout->addLayout(tagFilterLayout);

    // Drug List Area
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    drugListWidget = new QWidget();
    drugListLayout = new QVBoxLayout(drugListWidget);
    drugListLayout->setContentsMargins(0, 0, 0, 0);
    drugListLayout->setSpacing(15);
    drugListLayout->setAlignment(Qt::AlignTop); // Align items to the top

    scrollArea->setWidget(drugListWidget);
    mainLayout->addWidget(scrollArea);

    // Add some sample drug cards
    addDrugCard("奥美拉唑 20mg · 片剂", "通用名: Omeprazole", "消化系统", {"消化系统", "RX", "肝功能不全慎用"}, true, true, false, true);
    addDrugCard("阿莫西林 500mg · 胶囊", "通用名: Amoxicillin", "感染/抗生素", {"感染/抗生素", "RX", "儿童慎用"}, true, false, true, false);
    addDrugCard("二甲双胍 0.5g · 片剂", "通用名: Metformin", "内分泌", {"内分泌", "RX", "肝功能不全慎用"}, true, true, false, true);
    addDrugCard("阿司匹林肠溶片 100mg · 片剂", "通用名: Aspirin (EC)", "心血管", {"心血管", "OTC", "妊娠禁用", "肾功能不全慎用"}, false, true, false, false);

    mainLayout->addStretch(); // Pushes content to the top
}

void DrugSearch::addDrugCard(const QString &name, const QString &genericName, const QString &description,
                             const QStringList &tags, bool isRx, bool isChildrenSafe, bool isLiverKidneySafe, bool isPregnancySafe)
{
    QWidget *cardWidget = new QWidget();
    cardWidget->setStyleSheet("QWidget { background-color: white; border: 1px solid #e0e0e0; border-radius: 8px; padding: 15px; }");
    QHBoxLayout *cardLayout = new QHBoxLayout(cardWidget);
    cardLayout->setSpacing(15);

    // Left icon/placeholder
    QLabel *iconLabel = new QLabel("💊"); // Or use QPixmap for a real icon
    iconLabel->setStyleSheet("font-size: 30px; color: #007bff;");
    iconLabel->setFixedSize(40, 40);
    iconLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(iconLabel);

    // Drug details
    QVBoxLayout *detailsLayout = new QVBoxLayout();
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    detailsLayout->addWidget(nameLabel);

    QLabel *genericNameLabel = new QLabel("通用名: " + genericName);
    genericNameLabel->setStyleSheet("font-size: 14px; color: #666;");
    detailsLayout->addWidget(genericNameLabel);

    QHBoxLayout *tagRowLayout = new QHBoxLayout();
    tagRowLayout->setSpacing(5);
    for (const QString &tag : tags) {
        QLabel *tagLabel = new QLabel(tag);
        tagLabel->setStyleSheet("QLabel { background-color: #e0f7fa; color: #007bff; border-radius: 5px; padding: 3px 8px; font-size: 12px; }");
        tagRowLayout->addWidget(tagLabel);
    }
    tagRowLayout->addStretch();
    detailsLayout->addLayout(tagRowLayout);

    detailsLayout->addStretch(); // Pushes details to the top
    cardLayout->addLayout(detailsLayout);
    cardLayout->addStretch(); // Pushes "查看" button to the right

    // View button
    QPushButton *viewButton = new QPushButton("查看");
    viewButton->setStyleSheet("QPushButton { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 5px; padding: 8px 15px; }"
                              "QPushButton:hover { background-color: #e0e0e0; }");
    viewButton->setFixedSize(80, 35);
    // Use lambda to connect with specific drug information if needed
    connect(viewButton, &QPushButton::clicked, this, &DrugSearch::on_viewButton_clicked);
    cardLayout->addWidget(viewButton);

    drugListLayout->addWidget(cardWidget);
}


void DrugSearch::on_backButton_clicked()
{
    qDebug() << "Back to main page clicked!";
    // Emit a signal or navigate back to the main window
    // For example:
    // Q_EMIT backToMainWindow();
    this->close(); // Close the current widget
}

void DrugSearch::on_searchButton_clicked()
{
    qDebug() << "Search button clicked!";
    qDebug() << "Search Text: " << searchLineEdit->text();
    qDebug() << "Category: " << categoryComboBox->currentText();
    qDebug() << "Dosage Form: " << dosageFormComboBox->currentText();

    // Here you would implement your actual search logic,
    // clear existing drug cards and add new ones based on search results.
}

void DrugSearch::on_resetButton_clicked()
{
    qDebug() << "Reset button clicked!";
    searchLineEdit->clear();
    categoryComboBox->setCurrentIndex(0);
    dosageFormComboBox->setCurrentIndex(0);
    // Uncheck all filter buttons
    QList<QPushButton*> filterButtons = {otcButton, prescriptionButton, childrenButton,
                                         pregnancyForbiddenButton, liverKidneyUnsuitableButton, kidneyUnsuitableButton};
    for (QPushButton *btn : filterButtons) {
        btn->setChecked(false);
    }
    // Re-add all default drug cards (or refresh from a full dataset)
    // For this example, we'll just clear and re-add the initial ones.
    QLayoutItem *item;
    while ((item = drugListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    addDrugCard("奥美拉唑 20mg · 片剂", "通用名: Omeprazole", "消化系统", {"消化系统", "RX", "肝功能不全慎用"}, true, true, false, true);
    addDrugCard("阿莫西林 500mg · 胶囊", "通用名: Amoxicillin", "感染/抗生素", {"感染/抗生素", "RX", "儿童慎用"}, true, false, true, false);
    addDrugCard("二甲双胍 0.5g · 片剂", "通用名: Metformin", "内分泌", {"内分泌", "RX", "肝功能不全慎用"}, true, true, false, true);
    addDrugCard("阿司匹林肠溶片 100mg · 片剂", "通用名: Aspirin (EC)", "心血管", {"心血管", "OTC", "妊娠禁用", "肾功能不全慎用"}, false, true, false, false);
}

void DrugSearch::on_viewButton_clicked()
{
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    if (senderButton) {
        // In a real application, you would identify which drug card this button belongs to
        // and fetch its detailed information.
        // For demonstration, we'll just show a generic message box.
        QMessageBox::information(this, "药品详情", "这里将显示该药品的详细信息，例如：\n"
                                 "通用名：Omeprazole\n"
                                 "适应症：胃溃疡、十二指肠溃疡\n"
                                 "用法用量：每日一次，每次20mg\n"
                                 "禁忌：对本品过敏者禁用\n"
                                 "注意事项：肝肾功能不全者慎用");
    }
}
