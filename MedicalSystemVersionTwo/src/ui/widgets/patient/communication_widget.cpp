#include "communication_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QTextEdit>
#include <QListWidget>
#include <QStackedWidget>
#include <QTimer>
#include <QDateTime>
#include <QScrollBar>
#include <QSpacerItem>
#include <QMouseEvent>

// DoctorCardWidget实现
DoctorCardWidget::DoctorCardWidget(const DoctorContact &doctor, QWidget *parent)
        : QFrame(parent), doctorName(doctor.name)
{
    setObjectName("doctorCard");
    setCursor(Qt::PointingHandCursor);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(15);

    // 医生头像
    QLabel *avatar = new QLabel();
    avatar->setObjectName("doctorAvatar");
    avatar->setFixedSize(60, 60);

    // 医生信息
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);

    QLabel *nameLabel = new QLabel(doctor.name);
    nameLabel->setObjectName("doctorCardName");

    QLabel *deptLabel = new QLabel(doctor.department);
    deptLabel->setObjectName("doctorCardDept");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(deptLabel);

    layout->addWidget(avatar);
    layout->addLayout(infoLayout);
    layout->addStretch();
}

void DoctorCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit doctorSelected(doctorName);
    }
    QFrame::mousePressEvent(event);
}

void DoctorCardWidget::enterEvent(QEvent *event)
{
    setStyleSheet("QFrame#doctorCard { border-color: #3182CE; background-color: #F7FAFC; }");
    QFrame::enterEvent(event);
}

void DoctorCardWidget::leaveEvent(QEvent *event)
{
    setStyleSheet("");
    QFrame::leaveEvent(event);
}

CommunicationWidget::CommunicationWidget(QWidget *parent)
        : QWidget(parent), doctorListLayout(nullptr), chatScrollArea(nullptr),
          chatLayout(nullptr), messageInput(nullptr), sendButton(nullptr),
          currentDoctorLabel(nullptr), doctorStatusLabel(nullptr),
          mainStack(nullptr), doctorSelectionPage(nullptr), chatPage(nullptr)
{
    setObjectName("CommunicationWidget");
    initUI();
    initStyleSheets();
    loadDoctorList();

    // 初始化聊天历史数据
    chatHistory["王医生"] = {
            ChatMessage{"您好，有什么可以帮您的？", "16:19", true, "王医生"}
    };

    timeUpdateTimer = new QTimer(this);
    connect(timeUpdateTimer, &QTimer::timeout, this, &CommunicationWidget::updateMessageTime);
    timeUpdateTimer->start(60000); // 每分钟更新一次
}

CommunicationWidget::~CommunicationWidget() {}

void CommunicationWidget::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 顶部标题栏
    QWidget* header = new QWidget();
    header->setObjectName("chatHeader");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    QPushButton *backButton = new QPushButton("← 返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &CommunicationWidget::backRequested);

    QLabel *title = new QLabel("医患沟通");
    title->setObjectName("pageTitle");

    QLabel *userName = new QLabel("张患者");
    userName->setObjectName("userName");

    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(userName);

    mainLayout->addWidget(header);

    // 主要内容区域 - 使用堆栈窗口切换医生选择和聊天界面
    mainStack = new QStackedWidget();

    // 创建医生选择页面
    doctorSelectionPage = createDoctorSelectionPanel();
    mainStack->addWidget(doctorSelectionPage);

    // 创建聊天页面
    chatPage = createChatPanel();
    mainStack->addWidget(chatPage);

    mainLayout->addWidget(mainStack);

    // 默认显示医生选择页面
    mainStack->setCurrentWidget(doctorSelectionPage);
    currentDoctor.clear(); // 确保每次进入都重置当前医生
}

QWidget* CommunicationWidget::createDoctorSelectionPanel() {
    QWidget *panel = new QWidget();
    panel->setObjectName("doctorSelectionPanel");

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(30, 20, 30, 20);
    layout->setSpacing(15);

    // 标题
    QLabel *selectionTitle = new QLabel("👤 选择医生进行沟通");
    selectionTitle->setObjectName("selectionTitle");
    layout->addWidget(selectionTitle);

    // 医生列表滚动区域
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("doctorScrollArea");

    QWidget *container = new QWidget();
    doctorListLayout = new QVBoxLayout(container);
    doctorListLayout->setContentsMargins(10, 10, 10, 10);
    doctorListLayout->setSpacing(15);

    scrollArea->setWidget(container);
    layout->addWidget(scrollArea);

    return panel;
}

QWidget* CommunicationWidget::createChatPanel() {
    QWidget *panel = new QWidget();
    panel->setObjectName("chatPanel");

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 医生信息栏
    QWidget *doctorInfo = new QWidget();
    doctorInfo->setObjectName("doctorInfoBar");
    QHBoxLayout *doctorInfoLayout = new QHBoxLayout(doctorInfo);
    doctorInfoLayout->setContentsMargins(20, 15, 20, 15);

    // 返回选择医生按钮
    QPushButton *backToDoctorListButton = new QPushButton("← 选择医生");
    backToDoctorListButton->setObjectName("backToDoctorListButton");
    connect(backToDoctorListButton, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentWidget(doctorSelectionPage);
    });

    // 医生头像
    QLabel *doctorAvatar = new QLabel();
    doctorAvatar->setObjectName("doctorAvatar");
    doctorAvatar->setFixedSize(40, 40);

    // 医生信息
    QVBoxLayout *doctorDetailsLayout = new QVBoxLayout();
    doctorDetailsLayout->setSpacing(2);

    currentDoctorLabel = new QLabel();
    currentDoctorLabel->setObjectName("currentDoctorName");

    doctorStatusLabel = new QLabel();
    doctorStatusLabel->setObjectName("doctorStatus");

    doctorDetailsLayout->addWidget(currentDoctorLabel);
    doctorDetailsLayout->addWidget(doctorStatusLabel);

    // **已修改：移除“在线”状态指示器**
    // QLabel *onlineIndicator = new QLabel("在线");
    // onlineIndicator->setObjectName("onlineIndicator");

    doctorInfoLayout->addWidget(backToDoctorListButton);
    doctorInfoLayout->addSpacing(10);
    doctorInfoLayout->addWidget(doctorAvatar);
    doctorInfoLayout->addLayout(doctorDetailsLayout);
    doctorInfoLayout->addStretch();
    // doctorInfoLayout->addWidget(onlineIndicator); // **已修改：移除**

    layout->addWidget(doctorInfo);

    // 聊天消息区域
    chatScrollArea = new QScrollArea();
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setFrameShape(QFrame::NoFrame);
    chatScrollArea->setObjectName("chatScrollArea");

    QWidget *chatContainer = new QWidget();
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setContentsMargins(20, 20, 20, 20);
    chatLayout->setSpacing(15);
    chatLayout->addStretch(); // 让消息从底部开始显示

    chatScrollArea->setWidget(chatContainer);
    layout->addWidget(chatScrollArea);

    // 输入区域
    layout->addWidget(createInputPanel());

    return panel;
}

QWidget* CommunicationWidget::createInputPanel() {
    QWidget *inputPanel = new QWidget();
    inputPanel->setObjectName("inputPanel");

    QHBoxLayout *layout = new QHBoxLayout(inputPanel);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(10);

    messageInput = new QTextEdit();
    messageInput->setObjectName("messageInput");
    messageInput->setMaximumHeight(80);
    messageInput->setPlaceholderText("输入消息...");

    sendButton = new QPushButton("📨");
    sendButton->setObjectName("sendButton");
    sendButton->setFixedSize(50, 50);
    connect(sendButton, &QPushButton::clicked, this, &CommunicationWidget::onSendMessage);

    layout->addWidget(messageInput);
    layout->addWidget(sendButton);

    return inputPanel;
}

QWidget* CommunicationWidget::createDoctorCard(const DoctorContact &doctor) {
    DoctorCardWidget *card = new DoctorCardWidget(doctor, this);

    // 连接信号
    connect(card, &DoctorCardWidget::doctorSelected, this, &CommunicationWidget::onDoctorSelected);

    return card;
}

QWidget* CommunicationWidget::createMessageWidget(const ChatMessage &message) {
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(messageWidget);
    mainLayout->setContentsMargins(0, 5, 0, 5);

    if (message.isFromDoctor) {
        // 医生消息 - 左对齐
        QFrame *bubble = new QFrame();
        bubble->setObjectName("doctorMessageBubble");

        QVBoxLayout *bubbleLayout = new QVBoxLayout(bubble);
        bubbleLayout->setContentsMargins(15, 10, 15, 10);

        QLabel *contentLabel = new QLabel(message.content);
        contentLabel->setObjectName("messageContent");
        contentLabel->setWordWrap(true);

        QLabel *timeLabel = new QLabel(message.time);
        timeLabel->setObjectName("messageTime");

        bubbleLayout->addWidget(contentLabel);
        bubbleLayout->addWidget(timeLabel);

        mainLayout->addWidget(bubble);
        mainLayout->addStretch();
    } else {
        // 患者消息 - 右对齐
        QFrame *bubble = new QFrame();
        bubble->setObjectName("patientMessageBubble");

        QVBoxLayout *bubbleLayout = new QVBoxLayout(bubble);
        bubbleLayout->setContentsMargins(15, 10, 15, 10);

        QLabel *contentLabel = new QLabel(message.content);
        contentLabel->setObjectName("messageContent");
        contentLabel->setWordWrap(true);

        QLabel *timeLabel = new QLabel(message.time);
        timeLabel->setObjectName("messageTime");
        timeLabel->setAlignment(Qt::AlignRight);

        bubbleLayout->addWidget(contentLabel);
        bubbleLayout->addWidget(timeLabel);

        mainLayout->addStretch();
        mainLayout->addWidget(bubble);
    }

    return messageWidget;
}

void CommunicationWidget::loadDoctorList() {
    if (!doctorListLayout) return;

    // **已修改：移除 isOnline 初始化**
    QList<DoctorContact> doctors = {
            {"王医生", "心血管内科", "主任医师", "您好，有什么可以帮您的？", "16:19"},
            {"李医生", "消化内科", "副主任医师", "", ""},
            {"张医生", "骨科", "主治医师", "", ""},
            {"刘医生", "妇产科", "主治医师", "", ""}
    };

    for(const auto &doctor : doctors) {
        doctorListLayout->addWidget(createDoctorCard(doctor));
    }

    doctorListLayout->addStretch();
}

void CommunicationWidget::onDoctorSelected(const QString &doctorName) {
    currentDoctor = doctorName;

    // 更新医生信息
    if (currentDoctorLabel) {
        currentDoctorLabel->setText(doctorName);
    }
    if (doctorStatusLabel) {
        // 根据医生姓名设置相应的科室和职称信息
        if (doctorName == "王医生") {
            doctorStatusLabel->setText("心血管内科主任医师");
        } else if (doctorName == "李医生") {
            doctorStatusLabel->setText("消化内科副主任医师");
        } else if (doctorName == "张医生") {
            doctorStatusLabel->setText("骨科主治医师");
        } else if (doctorName == "刘医生") {
            doctorStatusLabel->setText("妇产科主治医师");
        }
    }

    // 加载聊天历史
    loadChatHistory(doctorName);

    // 切换到聊天界面
    mainStack->setCurrentWidget(chatPage);
}

void CommunicationWidget::loadChatHistory(const QString &doctorName) {
    if (!chatLayout) return;

    // 清空现有消息
    QLayoutItem *item;
    while ((item = chatLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // 加载历史消息
    if (chatHistory.contains(doctorName)) {
        const auto &messages = chatHistory[doctorName];
        for (const auto &message : messages) {
            chatLayout->addWidget(createMessageWidget(message));
        }
    }

    chatLayout->addStretch();

    // 滚动到底部
    QTimer::singleShot(100, [this]() {
        if (chatScrollArea) {
            chatScrollArea->verticalScrollBar()->setValue(
                    chatScrollArea->verticalScrollBar()->maximum()
            );
        }
    });
}

void CommunicationWidget::onSendMessage() {
    if (!messageInput || messageInput->toPlainText().trimmed().isEmpty()) {
        return;
    }

    QString messageText = messageInput->toPlainText().trimmed();
    sendMessage(messageText);
    messageInput->clear();
}

void CommunicationWidget::sendMessage(const QString &message) {
    if (currentDoctor.isEmpty()) return;

    // 添加患者消息
    ChatMessage patientMessage;
    patientMessage.content = message;
    patientMessage.time = QDateTime::currentDateTime().toString("hh:mm");
    patientMessage.isFromDoctor = false;
    patientMessage.senderName = "患者";

    chatHistory[currentDoctor].append(patientMessage);

    // 移除最后的stretch
    if (chatLayout->count() > 0) {
        QLayoutItem *lastItem = chatLayout->itemAt(chatLayout->count() - 1);
        if (lastItem->spacerItem()) {
            chatLayout->removeItem(lastItem);
            delete lastItem;
        }
    }

    // 添加消息widget
    chatLayout->addWidget(createMessageWidget(patientMessage));
    chatLayout->addStretch();

    // 滚动到底部
    QTimer::singleShot(100, [this]() {
        if (chatScrollArea) {
            chatScrollArea->verticalScrollBar()->setValue(
                    chatScrollArea->verticalScrollBar()->maximum()
            );
        }
    });
}

void CommunicationWidget::updateMessageTime() {
    // 可以在这里更新消息时间显示
}

void CommunicationWidget::resetToStartPage() {
    // 重置到医生选择页面
    currentDoctor.clear();
    mainStack->setCurrentWidget(doctorSelectionPage);
}

void CommunicationWidget::initStyleSheets() {
    this->setStyleSheet(R"(
        #CommunicationWidget {
            background-color: #F0F4F8;
        }

        #chatHeader {
            background-color: white;
            border-bottom: 1px solid #E2E8F0;
        }

        #pageTitle {
            font-size: 20px;
            font-weight: bold;
            color: #2D3748;
        }

        #backButton {
            font-size: 14px;
            color: #4A5568;
            border: none;
            background: transparent;
            padding: 8px 12px;
        }
        #backButton:hover {
            color: #2D3748;
            background-color: #F7FAFC;
            border-radius: 6px;
        }

        #userName {
            font-size: 14px;
            color: #3182CE;
            background-color: #EBF8FF;
            padding: 6px 12px;
            border-radius: 15px;
        }

        #doctorSelectionPanel {
            background-color: #F0F4F8;
        }

        #selectionTitle {
            font-size: 18px;
            font-weight: bold;
            color: #2D3748;
            padding: 10px 0;
        }

        #doctorScrollArea {
            background-color: transparent;
        }

        #backToDoctorListButton {
            font-size: 14px;
            color: #3182CE;
            border: none;
            background: transparent;
            padding: 6px 12px;
        }
        #backToDoctorListButton:hover {
            background-color: #EBF8FF;
            border-radius: 6px;
        }

        #doctorCard {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
            margin: 5px 0;
        }
        #doctorCard:hover {
            border-color: #3182CE;
            background-color: #F7FAFC;
        }

        #doctorAvatar {
            background-color: #3182CE;
            border-radius: 30px;
        }

        #doctorCardName {
            font-size: 16px;
            font-weight: bold;
            color: #2D3748;
        }

        #doctorCardDept {
            font-size: 14px;
            color: #718096;
        }

        #chatPanel {
            background-color: white;
        }

        #doctorInfoBar {
            background-color: #F7FAFC;
            border-bottom: 1px solid #E2E8F0;
        }

        #currentDoctorName {
            font-size: 16px;
            font-weight: bold;
            color: #2D3748;
        }

        #doctorStatus {
            font-size: 12px;
            color: #718096;
        }

        #onlineIndicator {
            font-size: 12px;
            color: #38A169;
            font-weight: bold;
        }

        #chatScrollArea {
            background-color: #F7FAFC;
        }

        #doctorMessageBubble {
            background-color: white;
            border-radius: 15px;
            border: 1px solid #E2E8F0;
            max-width: 300px;
        }

        #patientMessageBubble {
            background-color: #3182CE;
            border-radius: 15px;
            max-width: 300px;
        }
        #patientMessageBubble #messageContent {
            color: white;
        }
        #patientMessageBubble #messageTime {
            color: rgba(255,255,255,0.8);
        }

        #messageContent {
            font-size: 14px;
            color: #2D3748;
            line-height: 1.4;
        }

        #messageTime {
            font-size: 11px;
            color: #A0AEC0;
        }

        #inputPanel {
            background-color: white;
            border-top: 1px solid #E2E8F0;
        }

        #messageInput {
            border: 1px solid #E2E8F0;
            border-radius: 20px;
            padding: 10px 15px;
            font-size: 14px;
            background-color: #F7FAFC;
        }
        #messageInput:focus {
            border-color: #3182CE;
            background-color: white;
        }

        #sendButton {
            background-color: #3182CE;
            color: white;
            border: none;
            border-radius: 25px;
            font-size: 18px;
        }
        #sendButton:hover {
            background-color: #2B6CB0;
        }
        #sendButton:pressed {
            background-color: #2C5282;
        }
    )");
}