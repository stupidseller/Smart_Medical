// 文件: src/ui/widgets/patient/communication_widget.h

#ifndef PATIENT_COMMUNICATION_WIDGET_H   // ← 修改宏名
#define PATIENT_COMMUNICATION_WIDGET_H

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
#include <QMap>        // 确保有这个
#include <QMouseEvent> // DoctorCardWidget 用到
#include <QEvent>      // enter/leave 事件用到

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

// —— 为了避免与医生端的 DoctorCardWidget 名字冲突，也一并改名 ——
// 原: class DoctorCardWidget
class PatientDoctorCardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PatientDoctorCardWidget(const DoctorContact &doctor, QWidget *parent = nullptr);

signals:
    void doctorSelected(const QString &doctorName);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString doctorName;
};

// 原: class CommunicationWidget
class PatientCommunicationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PatientCommunicationWidget(QWidget *parent = nullptr);
    ~PatientCommunicationWidget();

    void resetToStartPage();

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

    QWidget* createDoctorSelectionPanel();
    QWidget* createChatPanel();
    QWidget* createDoctorCard(const DoctorContact &doctor);
    QWidget* createMessageWidget(const ChatMessage &message);
    QWidget* createInputPanel();

private:
    QVBoxLayout *doctorListLayout = nullptr;
    QScrollArea *chatScrollArea = nullptr;
    QVBoxLayout *chatLayout = nullptr;
    QTextEdit *messageInput = nullptr;
    QPushButton *sendButton = nullptr;
    QLabel *currentDoctorLabel = nullptr;
    QLabel *doctorStatusLabel = nullptr;
    QStackedWidget *mainStack = nullptr;
    QWidget *doctorSelectionPage = nullptr;
    QWidget *chatPage = nullptr;

    QString currentDoctor;
    QMap<QString, QList<ChatMessage>> chatHistory;
    QTimer *timeUpdateTimer = nullptr;
};

#endif // PATIENT_COMMUNICATION_WIDGET_H
