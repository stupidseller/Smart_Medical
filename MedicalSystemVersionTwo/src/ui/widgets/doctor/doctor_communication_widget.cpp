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
#include <QTimer>
#include <QFrame>
#include <QStyle>

// 简单气泡：根据 sender 决定左右与配色
class ChatMessageWidget : public QWidget {
public:
    ChatMessageWidget(const QString &content, bool sentByDoctor, QWidget *parent=nullptr) : QWidget(parent) {
        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 6, 10, 6);

        auto bubble = new QLabel(content);
        bubble->setWordWrap(true);
        bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
        bubble->setObjectName(sentByDoctor ? "sentMessageBubble" : "receivedMessageBubble");

        if (sentByDoctor) { layout->addStretch(); layout->addWidget(bubble); }
        else              { layout->addWidget(bubble); layout->addStretch(); }
    }
};

/* ---------------- PatientListItem ---------------- */

PatientListItem::PatientListItem(const ContactInfo &info, QWidget *parent)
    : QFrame(parent), m_info(info)
{
    setObjectName("patientListItem");
    setCursor(Qt::PointingHandCursor);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(10,8,10,8);

    auto textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);

    nameLabel = new QLabel(m_info.name);
    nameLabel->setObjectName("patientNameLabel");

    lastMessageLabel = new QLabel(m_info.lastMessage);
    lastMessageLabel->setObjectName("lastMessageLabel");

    textLayout->addWidget(nameLabel);
    textLayout->addWidget(lastMessageLabel);

    auto metaLayout = new QVBoxLayout();
    metaLayout->setSpacing(4);
    metaLayout->setAlignment(Qt::AlignTop);

    auto timeLabel = new QLabel(m_info.timestamp);
    timeLabel->setObjectName("timestampLabel");

    unreadIndicator = new QLabel();
    unreadIndicator->setObjectName("unreadIndicator");
    unreadIndicator->setAlignment(Qt::AlignCenter);
    unreadIndicator->setFixedSize(18,18);
    setUnread(m_info.unreadCount);

    metaLayout->addWidget(timeLabel, 0, Qt::AlignRight);
    metaLayout->addWidget(unreadIndicator, 0, Qt::AlignRight);

    mainLayout->addLayout(textLayout, 1);
    mainLayout->addLayout(metaLayout);
}

void PatientListItem::setSelected(bool selected) {
    setProperty("selected", selected);
    style()->unpolish(this);
    style()->polish(this);
}
void PatientListItem::setUnread(int n) {
    unreadIndicator->setText(n>0 ? QString::number(n) : "");
    unreadIndicator->setVisible(n>0);
}
void PatientListItem::setLastMessage(const QString &m) {
    lastMessageLabel->setText(m);
}

void PatientListItem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) emit clicked(m_info.conversationId, m_info.patientId);
    QFrame::mousePressEvent(event);
}

/* ---------------- CommunicationWidget ---------------- */

static inline QString pickS(const QJsonObject &o, std::initializer_list<const char*> keys) {
    for (auto k : keys) if (o.contains(k)) return o.value(k).toString();
    return {};
}
static inline int pickI(const QJsonObject &o, std::initializer_list<const char*> keys) {
    for (auto k : keys) if (o.contains(k)) return o.value(k).toInt();
    return 0;
}

CommunicationWidget::CommunicationWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    applyStyles();

    // 进入页面后由 Main 主动触发一次 requestLoadContacts() 或直接调 api
    emit requestLoadContacts();
}

void CommunicationWidget::initUI() {
    setObjectName("doctorCommunicationWidget");
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(createLeftPanel());
    mainLayout->addWidget(createRightPanel());
}

QWidget* CommunicationWidget::createLeftPanel() {
    auto leftPanel = new QFrame();
    leftPanel->setObjectName("leftPanel");

    auto layout = new QVBoxLayout(leftPanel);
    layout->setContentsMargins(10, 12, 10, 10);
    layout->setSpacing(10);

    auto title = new QLabel("患者列表");
    title->setObjectName("leftPanelTitle");

    auto scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("patientListScrollArea");

    auto scrollContent = new QWidget();
    patientListLayout = new QVBoxLayout(scrollContent);
    patientListLayout->setSpacing(6);
    patientListLayout->addStretch();
    scrollArea->setWidget(scrollContent);

    layout->addWidget(title);
    layout->addWidget(scrollArea);

    return leftPanel;
}

QWidget* CommunicationWidget::createRightPanel() {
    rightPanel = new QFrame();
    rightPanel->setObjectName("rightPanel");

    auto layout = new QVBoxLayout(rightPanel);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    // header
    auto header = new QFrame();
    header->setObjectName("chatHeader");
    auto headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(12,8,12,8);

    auto backButton = new QPushButton("返回工作台");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &CommunicationWidget::backRequested);

    chattingWithLabel = new QLabel("请选择患者");
    chattingWithLabel->setObjectName("chattingWithLabel");
    patientStatusLabel = new QLabel("离线");
    patientStatusLabel->setObjectName("patientStatusLabel");

    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(chattingWithLabel);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(patientStatusLabel);
    headerLayout->addStretch();

    // message view
    auto messageScrollArea = new QScrollArea();
    messageScrollArea->setWidgetResizable(true);
    messageScrollArea->setObjectName("messageScrollArea");

    auto scrollContent = new QWidget();
    messageViewLayout = new QVBoxLayout(scrollContent);
    messageViewLayout->setContentsMargins(20, 10, 20, 10);
    messageViewLayout->setSpacing(10);
    messageViewLayout->addStretch();
    messageScrollArea->setWidget(scrollContent);

    // input
    auto inputFrame = new QFrame();
    inputFrame->setObjectName("inputFrame");
    auto inputLayout = new QHBoxLayout(inputFrame);
    inputLayout->setSpacing(12);
    inputLayout->setContentsMargins(12,10,12,10);

    messageInput = new QTextEdit();
    messageInput->setObjectName("messageInput");
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setFixedHeight(60);

    sendButton = new QPushButton("发送");
    sendButton->setObjectName("sendButton");
    sendButton->setFixedHeight(40);
    connect(sendButton, &QPushButton::clicked, this, &CommunicationWidget::onSendMessage);

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    layout->addWidget(header);
    layout->addWidget(messageScrollArea, 1);
    layout->addWidget(inputFrame);

    rightPanel->setVisible(false);
    return rightPanel;
}

void CommunicationWidget::applyStyles() {
    setStyleSheet(R"(
        #doctorCommunicationWidget { background-color:#EBF4FF; }

        #leftPanel {
            background-color:#FFFFFF; border-right:1px solid #E4E7ED;
            min-width:280px; max-width:280px;
        }
        #leftPanelTitle { font-size:16px; font-weight:bold; color:#303133; padding:4px; }
        #patientListScrollArea { border:none; }

        #patientListItem {
            padding:10px; border-radius:8px; border:1px solid transparent;
        }
        #patientListItem:hover { background-color:#F5F7FA; }
        #patientListItem[selected="true"] { background-color:#ECF5FF; border-color:#D9ECFF; }
        #patientNameLabel { font-size:14px; font-weight:bold; color:#303133; }
        #lastMessageLabel { font-size:12px; color:#909399; }
        #timestampLabel { font-size:12px; color:#C0C4CC; }
        #unreadIndicator {
            background:#F56C6C; color:#fff; font-size:11px; font-weight:bold;
            border-radius:9px; min-width:18px; min-height:18px;
        }

        #rightPanel { background-color:#F5F7FA; }
        #chatHeader { background-color:#FFFFFF; border-bottom:1px solid #E4E7ED; }
        #backButton { background:transparent; border:none; color:#606266; font-size:14px; }
        #chattingWithLabel { font-size:16px; font-weight:bold; }
        #patientStatusLabel { font-size:12px; color:#67C23A; }
        #messageScrollArea { border:none; }

        #sentMessageBubble, #receivedMessageBubble {
            padding:10px 15px; font-size:14px; border-radius:12px; max-width:480px;
        }
        #sentMessageBubble { background:#409EFF; color:#fff; }
        #receivedMessageBubble { background:#FFFFFF; color:#303133; }

        #inputFrame { background:#FFFFFF; border-top:1px solid #E4E7ED; }
        #messageInput {
            border:1px solid #DCDFE6; border-radius:8px; background:#F5F7FA;
        }
        #sendButton { background:#409EFF; color:#fff; border:none; border-radius:6px; padding:0 16px; }
        #sendButton:hover { background:#66B1FF; }
    )");
}

/* -------- Widget → UI -------- */

void CommunicationWidget::setContacts(const QJsonArray &contacts, const QString &side) {
    Q_UNUSED(side);
    contacts_.clear();
    qDeleteAll(itemByConv_);
    itemByConv_.clear();

    // 清空列表（保留最后一个 stretch）
    while (patientListLayout->count() > 1) {
        auto it = patientListLayout->takeAt(0);
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }

    for (const auto &v : contacts) {
        const QJsonObject c = v.toObject();
        ContactInfo info;
        info.conversationId = pickI(c, {"conversation_id","conv_id","id"});
        info.patientId      = pickI(c, {"patient_id","peer_id"});
        info.name           = pickS(c, {"patient_name","name","nickname"});
        info.lastMessage    = pickS(c, {"last_message","last_msg","preview"});
        info.timestamp      = pickS(c, {"last_time","timestamp","time"});
        info.unreadCount    = pickI(c, {"unread_count","unread","unreadNum"});

        contacts_.insert(info.conversationId, info);

        auto *item = new PatientListItem(info);
        itemByConv_.insert(info.conversationId, item);
        connect(item, &PatientListItem::clicked, this, &CommunicationWidget::onOpenContact);
        patientListLayout->insertWidget(patientListLayout->count()-1, item);
    }

    // 左侧准备好后，默认不主动打开会话，等待用户点击（或由 Main 再触发）
    rightPanel->setVisible(false);
}

void CommunicationWidget::setMessages(const QJsonArray &messages, int conversationId) {
    if (conversationId != currentConversationId_) {
        // 不是当前会话：只更新未读和预览
        if (contacts_.contains(conversationId)) {
            auto info = contacts_.value(conversationId);
            if (!messages.isEmpty()) {
                const auto m = messages.last().toObject();
                info.lastMessage = pickS(m, {"content","text"});
                contacts_[conversationId] = info;
                if (itemByConv_.contains(conversationId)) {
                    itemByConv_[conversationId]->setLastMessage(info.lastMessage);
                }
            }
        }
        return;
    }

    clearMessages();
    for (const auto &v : messages) {
        const QJsonObject m = v.toObject();
        const QString content = pickS(m, {"content","text"});
        const QString sender  = pickS(m, {"sender_type","sender"});
        const bool sentByDoctor = (sender.compare("doctor", Qt::CaseInsensitive)==0);
        addMessageBubble(content, sentByDoctor, false);
    }
    addMessageBubble("", true, true); // 仅用于触发滚动到底部（内容空白会被忽略视觉）
}

/* 发送成功回执：只对当前会话追加；其他会话更新未读与预览 */
void CommunicationWidget::appendMessage(const QJsonObject &message, int conversationId) {
    const QString content = pickS(message, {"content","text"});
    const QString sender  = pickS(message, {"sender_type","sender"});
    const bool sentByDoctor = (sender.compare("doctor", Qt::CaseInsensitive)==0);

    // 更新左侧预览
    if (contacts_.contains(conversationId)) {
        auto info = contacts_.value(conversationId);
        info.lastMessage = content;
        if (conversationId != currentConversationId_ && !sentByDoctor) {
            info.unreadCount += 1;
        }
        contacts_[conversationId] = info;
        if (itemByConv_.contains(conversationId)) {
            itemByConv_[conversationId]->setLastMessage(info.lastMessage);
            itemByConv_[conversationId]->setUnread(info.unreadCount);
        }
    }

    if (conversationId == currentConversationId_) {
        addMessageBubble(content, sentByDoctor, true);
    }
}

void CommunicationWidget::showError(const QString &err) {
    // 简单提示（你也可以换成气泡/状态条）
    addMessageBubble(err.isEmpty() ? "操作失败" : err, false, true);
}

/* -------- UI 内部逻辑 -------- */

void CommunicationWidget::onOpenContact(int conversationId, int patientId) {
    // 取消旧选中
    if (itemByConv_.contains(currentConversationId_))
        itemByConv_[currentConversationId_]->setSelected(false);

    currentConversationId_ = conversationId;
    currentPatientId_ = patientId;

    if (itemByConv_.contains(currentConversationId_)) {
        itemByConv_[currentConversationId_]->setSelected(true);
        itemByConv_[currentConversationId_]->setUnread(0);
    }
    rightPanel->setVisible(true);

    // 头部
    const QString name = contacts_.value(conversationId).name;
    chattingWithLabel->setText(name);
    patientStatusLabel->setText("在线");

    clearMessages();
    emit requestOpenConversation(conversationId);
}

void CommunicationWidget::onSendMessage() {
    const QString text = messageInput->toPlainText().trimmed();
    if (text.isEmpty()) return;

    if (currentConversationId_ > 0) {
        emit requestSendMessageByConv(currentConversationId_, text);
    } else if (currentPatientId_ > 0) {
        emit requestSendMessageByPeer(currentPatientId_, text);
    }
    messageInput->clear();
}

void CommunicationWidget::rebuildContactList() {
    // 已在 setContacts 中做了完整重建，这里暂不需要额外逻辑
}

void CommunicationWidget::clearMessages() {
    while (messageViewLayout->count() > 1) { // 保留最后一个 stretch
        auto it = messageViewLayout->takeAt(0);
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }
}

void CommunicationWidget::addMessageBubble(const QString &content, bool sentByDoctor, bool scrollToBottom) {
    if (!content.isEmpty()) {
        auto *w = new ChatMessageWidget(content, sentByDoctor);
        messageViewLayout->insertWidget(messageViewLayout->count()-1, w);
    }
    if (scrollToBottom) {
        QTimer::singleShot(10, this, [this](){
            auto scrollArea = this->findChild<QScrollArea*>("messageScrollArea");
            if (scrollArea) {
                auto *bar = scrollArea->verticalScrollBar();
                bar->setValue(bar->maximum());
            }
        });
    }
}
