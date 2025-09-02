#ifndef DOCTOR_COMMUNICATION_WIDGET_H
#define DOCTOR_COMMUNICATION_WIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QList>
#include <QJsonArray>
#include <QJsonObject>

class QVBoxLayout;
class QLabel;
class QTextEdit;
class QPushButton;

// 左侧联系人模型（来自服务器/Widget）
struct ContactInfo {
    int conversationId = 0;
    int patientId = 0;
    QString name;
    QString lastMessage;
    QString timestamp;
    int unreadCount = 0;
};

// 左侧列表项
class PatientListItem : public QFrame
{
Q_OBJECT
public:
    explicit PatientListItem(const ContactInfo &info, QWidget *parent = nullptr);
    void setSelected(bool selected);
    int conversationId() const { return m_info.conversationId; }
    int patientId() const { return m_info.patientId; }
    void setUnread(int n);
    void setLastMessage(const QString &m);

signals:
    void clicked(int conversationId, int patientId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    ContactInfo m_info;
    QLabel *nameLabel = nullptr;
    QLabel *lastMessageLabel = nullptr;
    QLabel *unreadIndicator = nullptr;
};

// 主控件
class CommunicationWidget : public QWidget
{
Q_OBJECT
public:
    explicit CommunicationWidget(QWidget *parent = nullptr);

signals:
    // 让 Main 去调用 Widget 的函数（UI -> Main -> Widget）
    void requestLoadContacts();
    void requestOpenConversation(int conversationId);
    void requestSendMessageByConv(int conversationId, const QString &text);
    void requestSendMessageByPeer(int patientId, const QString &text);

    void backRequested();

public slots:
    // Widget -> UI
    void setContacts(const QJsonArray &contacts, const QString &side);
    void setMessages(const QJsonArray &messages, int conversationId);
    void appendMessage(const QJsonObject &message, int conversationId);
    void showError(const QString &err);

private slots:
    void onOpenContact(int conversationId, int patientId);
    void onSendMessage();

private:
    void initUI();
    void applyStyles();
    QWidget* createLeftPanel();
    QWidget* createRightPanel();

    void rebuildContactList();
    void clearMessages();
    void addMessageBubble(const QString &content, bool sentByDoctor, bool scrollToBottom);

    // UI
    QVBoxLayout *patientListLayout = nullptr;
    QVBoxLayout *messageViewLayout = nullptr;
    QWidget *rightPanel = nullptr;
    QLabel *chattingWithLabel = nullptr;
    QLabel *patientStatusLabel = nullptr;
    QTextEdit *messageInput = nullptr;
    QPushButton *sendButton = nullptr;

    // 数据
    int currentConversationId_ = 0;
    int currentPatientId_ = 0;

    // key: conversationId
    QMap<int, ContactInfo> contacts_;
    QMap<int, PatientListItem*> itemByConv_;
};

#endif // DOCTOR_COMMUNICATION_WIDGET_H
