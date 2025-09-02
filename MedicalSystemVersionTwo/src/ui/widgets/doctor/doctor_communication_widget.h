#ifndef DOCTOR_COMMUNICATION_WIDGET_H
#define DOCTOR_COMMUNICATION_WIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QList>
#include "../common/shared_data_types.h" // <--- 引入公共头文件，替换掉旧的 struct 定义

// --- 前置声明 ---
class QVBoxLayout;
class QLabel;
class QTextEdit;
class QPushButton;

// --- 数据结构 ---
// 存储患者基本信息
struct PatientContactInfo {
    QString id;
    QString name;
    QString lastMessage;
    QString timestamp;
    int unreadCount = 0;
};

// 【已移除】ChatMessage 结构体已移至 shared_data_types.h

// --- 自定义组件：左侧患者列表项 ---
class PatientListItem : public QFrame
{
Q_OBJECT
public:
    explicit PatientListItem(const PatientContactInfo &info, QWidget *parent = nullptr);
    QString patientId() const { return m_patientId; }
    void setSelected(bool selected);

signals:
    void clicked(const QString &patientId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_patientId;
    QLabel *nameLabel;
    QLabel *lastMessageLabel;
    QLabel *unreadIndicator;
};


// --- 主界面 ---
class DoctorCommunicationWidget : public QWidget
{
Q_OBJECT

public:
    explicit DoctorCommunicationWidget(QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void onPatientSelected(const QString &patientId);
    void onSendMessage();

private:
    void initUI();
    void applyStyles();
    void setupMockData();

    QWidget* createLeftPanel();
    QWidget* createRightPanel();

    void loadPatientList();
    void loadChatHistory(const QString &patientId);
    void addMessageToView(const ChatMessage &message, bool isNewMessage = false);

    // UI 组件
    QVBoxLayout *patientListLayout;
    QVBoxLayout *messageViewLayout;
    QWidget *rightPanel;
    QLabel *chattingWithLabel;
    QLabel *patientStatusLabel;
    QTextEdit *messageInput;
    QPushButton *sendButton;

    // 数据
    QString currentPatientId;
    QMap<QString, PatientContactInfo> patientContacts;
    QMap<QString, QList<ChatMessage>> conversationHistories;
    QMap<QString, PatientListItem*> patientListItems;
};

#endif // DOCTOR_COMMUNICATION_WIDGET_H