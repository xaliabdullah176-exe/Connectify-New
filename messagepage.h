#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QScrollBar>
#include <QDateTime>

// ─────────────────────────────────────────────
//  A single message bubble (sent or received)
// ─────────────────────────────────────────────
class MessageBubble : public QFrame {
    Q_OBJECT
public:
    MessageBubble(const QString &text,
                  const QString &time,
                  bool isSent,
                  QWidget *parent = nullptr);
};

// ─────────────────────────────────────────────
//  A single row in the chat list (left panel)
// ─────────────────────────────────────────────
class ChatListItem : public QFrame {
    Q_OBJECT
public:
    ChatListItem(int userID,
                 const QString &name,
                 const QString &lastMessage,
                 const QString &time,
                 bool isOnline,
                 QWidget *parent = nullptr);

    int getUserID() const { return m_userID; }
    void setActive(bool active);

signals:
    void clicked(int userID);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int     m_userID;
    bool    m_active = false;
};

// ─────────────────────────────────────────────
//  Main MessagePage
// ─────────────────────────────────────────────
class MessagePage : public QWidget {
    Q_OBJECT

public:
    explicit MessagePage(QWidget *parent = nullptr);

    // Call once after login
    void loadCurrentUser(int userID, const QString &name);

    // Add a contact to the left chat list
    void addContact(int userID,
                    const QString &name,
                    const QString &lastMessage,
                    const QString &time,
                    bool isOnline = false);

    // Add a message bubble to the open conversation
    // isSent = true  → right side (current user)
    // isSent = false → left side  (other user)
    void addMessage(const QString &text,
                    const QString &time,
                    bool isSent);

    // Clear all bubbles (call before loading a new conversation)
    void clearMessages();

    // Programmatically open a conversation
    void openConversation(int userID,
                          const QString &contactName,
                          bool isOnline);

signals:
    void sendMessageClicked(int toUserID, const QString &text);
    void contactSelected(int userID);
    void logoutClicked();
    void backClicked();          // navigate back to previous page

private slots:
    void onSendClicked();
    void onContactClicked(int userID);

private:
    // ── State ──────────────────────────────────────
    int     m_currentUserID  = -1;
    int     m_openChatUserID = -1;
    QString m_currentUserName;

    // ── Left panel ─────────────────────────────────
    QVBoxLayout *chatListLayout;   // holds ChatListItem widgets + stretch
    QLineEdit   *searchBox;
    QList<ChatListItem*> chatItems;

    // ── Right panel ────────────────────────────────
    QWidget     *placeholderWidget;   // shown when no chat is open
    QWidget     *conversationWidget;  // shown when a chat is open

    QLabel      *chatNameLabel;
    QLabel      *chatStatusLabel;
    QLabel      *chatAvatarLabel;

    QScrollArea *messagesScrollArea;
    QVBoxLayout *messagesLayout;     // inside scroll — holds bubbles + stretch

    QLineEdit   *messageInput;
    QPushButton *sendBtn;

    // ── Helpers ────────────────────────────────────
    void setupUI();
    void applyStyles();
    void scrollToBottom();
    QString makeInitials(const QString &name);
};
