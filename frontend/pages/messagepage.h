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
                 const QString &avatarPath,
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

    // When opening Messages: updates name; full reset only if the account changed
    void loadCurrentUser(int userID, const QString &name, const QString &email = QString());

    // Call on logout so the next login does not inherit open-chat / user id state
    void resetForNewSession();

    // Add a contact to the left chat list
    void addContact(int userID,
                    const QString &name,
                    const QString &avatarPath,
                    const QString &lastMessage,
                    const QString &time,
                    bool isOnline = false);

    // Clear all contacts
    void clearContacts();

    // Add a message bubble to the open conversation
    // isSent = true  → right side (current user)
    // isSent = false → left side  (other user)
    void addMessage(const QString &text,
                    const QString &time,
                    bool isSent);

    // Clear all bubbles (call before loading a new conversation)
    void clearMessages();

    // Programmatically open a conversation (DM)
    void openConversation(int userID,
                          const QString &contactName,
                          const QString &avatarPath,
                          bool isOnline);

    // Group chat (chat key is negative group id: -groupID)
    void openGroupConversation(int groupID,
                               const QString &groupName,
                               const QString &iconPath,
                               int memberCount,
                               bool creatorCanManageMembers);

    // After rebuilding the left list, restore highlight for the open chat (same account)
    void syncListSelectionWithOpenChat();

signals:
    void dmMessageSendRequested(int peerUserId, const QString &text);
    void groupMessageSendRequested(int groupId, const QString &text);
    void contactSelected(int userID);
    void createGroupClicked();
    void renameGroupClicked(int groupID);
    void addMembersToGroupClicked(int groupID);
    void manageGroupMembersClicked(int groupID);
    void logoutClicked();
    void backClicked();          // navigate back to previous page

private slots:
    void onSendClicked();
    void onContactClicked(int userID);
    void onRenameGroupClicked();
    void onAddMembersClicked();
    void onManageGroupClicked();

private:
    enum class ChatKind { None, Dm, Group };

    // ── State ──────────────────────────────────────
    int      m_currentUserID  = -1;
    int      m_openChatUserID = -1; // list highlight: DM = peer id, group = -groupId
    ChatKind m_chatKind       = ChatKind::None;
    int      m_activeGroupId  = -1; // positive when m_chatKind == Group
    QString  m_currentUserName;

    // ── Left panel ─────────────────────────────────
    QVBoxLayout *chatListLayout;   // holds ChatListItem widgets + stretch
    QLineEdit   *searchBox;
    QLabel      *sessionEmailLabel;
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
    QPushButton *createGroupBtn = nullptr;
    QPushButton *renameGroupBtn = nullptr;
    QPushButton *addMembersBtn = nullptr;
    QPushButton *manageGroupBtn = nullptr;

    // ── Helpers ────────────────────────────────────
    void setupUI();
    void applyStyles();
    void scrollToBottom();
    QString makeInitials(const QString &name);
};
