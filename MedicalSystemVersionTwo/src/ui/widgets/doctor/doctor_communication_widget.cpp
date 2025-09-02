#include "doctor_communication_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QSvgWidget>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStyle>
#include <QTimer>
#include <QTime>
// --- SVG Icons ---
static const char* backIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-arrow-left"><line x1="19" y1="12" x2="5" y2="12"></line><polyline points="12 19 5 12 12 5"></polyline></svg>)";
static const char* userIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-user"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path><circle cx="12" cy="7" r="4"></circle></svg>)";
static const char* sendIconSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-send"><line x1="22" y1="2" x2="11" y2="13"></line><polygon points="22 2 15 22 11 13 2 9 22 2"></polygon></svg>)";


// --- 自定义组件：消息气泡 ---
// (由于其逻辑简单，直接内联实现，无需单独文件)
class ChatMessageWidget : public QWidget {
public:
    explicit ChatMessageWidget(const ChatMessage &message, QWidget *parent = nullptr) : QWidget(parent) {
        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 5, 10, 5);

        auto bubble = new QLabel(message.content);
        bubble->setWordWrap(true);
        bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);

        // 【BUG修复】交换了 if 和 else 内部的逻辑，以正确匹配发送者和气泡样式
        if (message.sentByDoctor) { // 医生发送的消息
            bubble->setObjectName("sentMessageBubble"); // 应用蓝色气泡
            layout->addStretch();
            layout->addWidget(bubble);
        } else { // 患者发送的消息
            bubble->setObjectName("receivedMessageBubble"); // 应用白色气泡
            layout->addWidget(bubble);
            layout->addStretch();
        }
    }
};


// --- 自定义组件：患者列表项实现 ---
PatientListItem::PatientListItem(const PatientContactInfo &info, QWidget *parent)
        : QFrame(parent), m_patientId(info.id)
{
    this->setObjectName("patientListItem");
    this->setCursor(Qt::PointingHandCursor);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(15);

    auto icon = new QSvgWidget();
    icon->load(QByteArray(userIconSvg));
    icon->setFixedSize(40, 40);

    auto textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    nameLabel = new QLabel(info.name);
    nameLabel->setObjectName("patientNameLabel");
    lastMessageLabel = new QLabel(info.lastMessage);
    lastMessageLabel->setObjectName("lastMessageLabel");
    textLayout->addWidget(nameLabel);
    textLayout->addWidget(lastMessageLabel);

    auto metaLayout = new QVBoxLayout();
    metaLayout->setSpacing(5);
    metaLayout->setAlignment(Qt::AlignTop);
    auto timeLabel = new QLabel(info.timestamp);
    timeLabel->setObjectName("timestampLabel");

    unreadIndicator = new QLabel(info.unreadCount > 0 ? QString::number(info.unreadCount) : "");
    unreadIndicator->setObjectName("unreadIndicator");
    unreadIndicator->setAlignment(Qt::AlignCenter);
    unreadIndicator->setFixedSize(18, 18);
    unreadIndicator->setVisible(info.unreadCount > 0);

    metaLayout->addWidget(timeLabel);
    metaLayout->addWidget(unreadIndicator, 0, Qt::AlignRight);

    mainLayout->addWidget(icon);
    mainLayout->addLayout(textLayout, 1);
    mainLayout->addLayout(metaLayout);
}

void PatientListItem::setSelected(bool selected) {
    this->setProperty("selected", selected);
    style()->unpolish(this);
    style()->polish(this);
}

void PatientListItem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_patientId);
    }
    QFrame::mousePressEvent(event);
}

// --- 主界面实现 ---
DoctorCommunicationWidget::DoctorCommunicationWidget(QWidget *parent) : QWidget(parent)
{
    setupMockData();
    initUI();
    applyStyles();

    // 默认选中第一个患者
    if (!patientContacts.isEmpty()) {
        onPatientSelected(patientContacts.first().id);
    }
}

void DoctorCommunicationWidget::initUI() {
    this->setObjectName("doctorCommunicationWidget");
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(createLeftPanel());
    mainLayout->addWidget(createRightPanel());
}

QWidget* DoctorCommunicationWidget::createLeftPanel() {
    auto leftPanel = new QFrame();
    leftPanel->setObjectName("leftPanel");

    auto layout = new QVBoxLayout(leftPanel);
    layout->setContentsMargins(10, 15, 10, 10);
    layout->setSpacing(10);

    auto title = new QLabel("患者列表");
    title->setObjectName("leftPanelTitle");

    auto scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("patientListScrollArea");

    auto scrollContent = new QWidget();
    patientListLayout = new QVBoxLayout(scrollContent);
    patientListLayout->setSpacing(5);
    patientListLayout->addStretch();

    scrollArea->setWidget(scrollContent);

    layout->addWidget(title);
    layout->addWidget(scrollArea);

    loadPatientList();

    return leftPanel;
}

QWidget* DoctorCommunicationWidget::createRightPanel() {
    rightPanel = new QFrame();
    rightPanel->setObjectName("rightPanel");

    auto layout = new QVBoxLayout(rightPanel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // -- Header --
    auto header = new QFrame();
    header->setObjectName("chatHeader");
    auto headerLayout = new QHBoxLayout(header);

    auto backButton = new QPushButton("返回工作台");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &DoctorCommunicationWidget::backRequested);

    chattingWithLabel = new QLabel("请选择患者");
    chattingWithLabel->setObjectName("chattingWithLabel");

    patientStatusLabel = new QLabel();
    patientStatusLabel->setObjectName("patientStatusLabel");

    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(chattingWithLabel);
    headerLayout->addWidget(patientStatusLabel);
    headerLayout->addStretch();

    // -- Message View --
    auto messageScrollArea = new QScrollArea();
    messageScrollArea->setWidgetResizable(true);
    messageScrollArea->setObjectName("messageScrollArea");

    auto scrollContent = new QWidget();
    messageViewLayout = new QVBoxLayout(scrollContent);
    messageViewLayout->setContentsMargins(20, 10, 20, 10);
    messageViewLayout->setSpacing(10);
    messageViewLayout->addStretch();

    messageScrollArea->setWidget(scrollContent);

    // -- Input Area --
    auto inputFrame = new QFrame();
    inputFrame->setObjectName("inputFrame");
    auto inputLayout = new QHBoxLayout(inputFrame);
    inputLayout->setSpacing(15);

    messageInput = new QTextEdit();
    messageInput->setObjectName("messageInput");
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setFixedHeight(60);

    sendButton = new QPushButton();
    sendButton->setObjectName("sendButton");
    sendButton->setFixedSize(48, 48);
    sendButton->setIconSize(QSize(24, 24));
    auto icon = new QSvgWidget(sendButton);
    icon->load(QByteArray(sendIconSvg));
    connect(sendButton, &QPushButton::clicked, this, &DoctorCommunicationWidget::onSendMessage);

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    layout->addWidget(header);
    layout->addWidget(messageScrollArea, 1);
    layout->addWidget(inputFrame);

    rightPanel->setVisible(false); // 初始隐藏

    return rightPanel;
}

void DoctorCommunicationWidget::setupMockData() {
    // 模拟患者联系人数据
    patientContacts["p001"] = {"p001", "张患者", "好的，谢谢医生！", "昨天", 1};
    patientContacts["p002"] = {"p002", "李先生", "我感觉好多了。", "14:20", 0};
    patientContacts["p003"] = {"p003", "王女士", "请问药什么时间吃？", "09:35", 3};

    // 模拟聊天记录 (布尔值: false表示医生发送, true表示患者发送)
    conversationHistories["p001"].append({"您好，请问有什么可以帮您的？", "昨天", false});
    conversationHistories["p001"].append({"医生，我最近感觉有点头晕。", "昨天", true});
    conversationHistories["p001"].append({"建议您先测量一下血压，注意休息。", "昨天", false});
    conversationHistories["p001"].append({"好的，谢谢医生！", "昨天", true});

    conversationHistories["p002"].append({"李先生您好，上次开的药效果如何？", "14:18", false});
    conversationHistories["p002"].append({"我感觉好多了。", "14:20", true});

    conversationHistories["p003"].append({"王女士您好。", "09:30", false});
}

void DoctorCommunicationWidget::loadPatientList() {
    for (const auto &info : patientContacts) {
        auto item = new PatientListItem(info);
        connect(item, &PatientListItem::clicked, this, &DoctorCommunicationWidget::onPatientSelected);
        patientListLayout->insertWidget(patientListLayout->count() - 1, item);
        patientListItems[info.id] = item;
    }
}

void DoctorCommunicationWidget::onPatientSelected(const QString &patientId) {
    if (currentPatientId == patientId) return;

    // 更新选中状态
    if (patientListItems.contains(currentPatientId)) {
        patientListItems[currentPatientId]->setSelected(false);
    }
    if (patientListItems.contains(patientId)) {
        patientListItems[patientId]->setSelected(true);
    }

    currentPatientId = patientId;
    rightPanel->setVisible(true);

    // 更新聊天窗口头部
    const auto &info = patientContacts[patientId];
    chattingWithLabel->setText(info.name);
    patientStatusLabel->setText("在线");

    // 加载聊天记录
    loadChatHistory(patientId);
}

void DoctorCommunicationWidget::loadChatHistory(const QString &patientId) {
    // 清空旧消息
    QLayoutItem* item;
    while ((item = messageViewLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    messageViewLayout->addStretch();

    // 加载新消息
    const auto &history = conversationHistories[patientId];
    for (const auto &message : history) {
        addMessageToView(message);
    }
}

void DoctorCommunicationWidget::addMessageToView(const ChatMessage &message, bool isNewMessage) {
    auto messageWidget = new ChatMessageWidget(message);
    messageViewLayout->insertWidget(messageViewLayout->count() - 1, messageWidget);

    if (isNewMessage) {
        // 滚动到底部
        QTimer::singleShot(10, this, [this](){
            auto scrollArea = this->findChild<QScrollArea*>("messageScrollArea");
            if (scrollArea) {
                scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
            }
        });
    }
}


void DoctorCommunicationWidget::onSendMessage() {
    QString text = messageInput->toPlainText().trimmed();
    if (text.isEmpty() || currentPatientId.isEmpty()) {
        return;
    }

    ChatMessage message = {text, QTime::currentTime().toString("HH:mm"), true};

    // 添加到数据模型
    conversationHistories[currentPatientId].append(message);

    // 更新UI
    addMessageToView(message, true);

    // 清空输入框
    messageInput->clear();
}


void DoctorCommunicationWidget::applyStyles() {
    this->setStyleSheet(R"(
        #doctorCommunicationWidget { background-color: #EBF4FF; }
        #leftPanel {
            background-color: #FFFFFF;
            border-right: 1px solid #E4E7ED;
            min-width: 280px;
            max-width: 280px;
        }
        #leftPanelTitle {
            font-size: 16px;
            font-weight: bold;
            color: #303133;
            padding: 5px;
        }
        #patientListScrollArea { border: none; }

        #patientListItem {
            padding: 12px;
            border-radius: 8px;
            border: 1px solid transparent;
        }
        #patientListItem:hover { background-color: #F5F7FA; }
        #patientListItem[selected="true"] {
            background-color: #ECF5FF;
            border-color: #D9ECFF;
        }

        #patientNameLabel { font-size: 14px; font-weight: bold; color: #303133; }
        #lastMessageLabel { font-size: 12px; color: #909399; }
        #timestampLabel { font-size: 12px; color: #C0C4CC; }
        #unreadIndicator {
            background-color: #F56C6C;
            color: white;
            font-size: 11px;
            font-weight: bold;
            border-radius: 9px;
        }

        #rightPanel { background-color: #F5F7FA; }
        #chatHeader {
            background-color: #FFFFFF;
            padding: 10px 20px;
            border-bottom: 1px solid #E4E7ED;
        }
        #backButton {
            background: transparent;
            border: none;
            color: #606266;
            font-size: 14px;
        }
        #chattingWithLabel { font-size: 16px; font-weight: bold; }
        #patientStatusLabel { font-size: 12px; color: #67C23A; }
        #messageScrollArea { border: none; }

        #sentMessageBubble, #receivedMessageBubble {
            padding: 10px 15px;
            font-size: 14px;
            border-radius: 12px;
            max-width: 450px;
        }
        #sentMessageBubble {
            background-color: #409EFF;
            color: white;
        }
        #receivedMessageBubble {
            background-color: #FFFFFF;
            color: #303133;
        }

        #inputFrame {
            background-color: #FFFFFF;
            padding: 15px 20px;
            border-top: 1px solid #E4E7ED;
        }
        #messageInput {
            border: 1px solid #DCDFE6;
            border-radius: 8px;
            background-color: #F5F7FA;
        }
        #sendButton {
            background-color: #409EFF;
            border-radius: 24px;
            border: none;
        }
        #sendButton:hover { background-color: #66B1FF; }
    )");
}