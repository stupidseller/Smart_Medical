#ifndef COMMUNICATION_WIDGET_H
#define COMMUNICATION_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QTextEdit>
#include <QListWidget>
#include <QStackedWidget>
#include <QTimer>

struct DoctorContact {
    QString name;
    QString department;
    QString title;
    bool isOnline;
    QString lastMessage;
    QString lastTime;
};

struct ChatMessage {
    QString content;
    QString time;
    bool isFromDoctor;
    QString senderName;
};

class DoctorCardWidget : public QFrame
{
Q_OBJECT

public:
    explicit DoctorCardWidget(const DoctorContact &doctor, QWidget *parent = nullptr);

signals:
    void doctorSelected(const QString &doctorName);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString doctorName;
};

class CommunicationWidget : public QWidget
{
Q_OBJECT

public:
    explicit CommunicationWidget(QWidget *parent = nullptr);
    ~CommunicationWidget();

    void resetToStartPage(); // 添加重置方法

signals:
    void backRequested();

private slots:
    void onDoctorSelected(const QString &doctorName);
    void onSendMessage();
    void updateMessageTime();

private:
    void initUI();
    void initStyleSheets();
    void loadDoctorList();
    void loadChatHistory(const QString &doctorName);
    void sendMessage(const QString &message);

    // UI创建函数
    QWidget* createDoctorSelectionPanel();
    QWidget* createChatPanel();
    QWidget* createDoctorCard(const DoctorContact &doctor);
    QWidget* createMessageWidget(const ChatMessage &message);
    QWidget* createInputPanel();

private:
    QVBoxLayout *doctorListLayout;
    QScrollArea *chatScrollArea;
    QVBoxLayout *chatLayout;
    QTextEdit *messageInput;
    QPushButton *sendButton;
    QLabel *currentDoctorLabel;
    QLabel *doctorStatusLabel;
    QStackedWidget *mainStack;
    QWidget *doctorSelectionPage;
    QWidget *chatPage;

    QString currentDoctor;
    QMap<QString, QList<ChatMessage>> chatHistory;
    QTimer *timeUpdateTimer;
};

#endif // COMMUNICATION_WIDGET_H