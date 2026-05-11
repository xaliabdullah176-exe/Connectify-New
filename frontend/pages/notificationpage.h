#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QDateTime>

// ─────────────────────────────────────────────────────
//  Single notification row (plain text)
// ─────────────────────────────────────────────────────
class NotifRow : public QFrame {
    Q_OBJECT
public:
    NotifRow(const QString &message, const QString &time, QWidget *parent = nullptr);
};

// ─────────────────────────────────────────────────────
//  Friend request row with Accept / Decline
// ─────────────────────────────────────────────────────
class FriendRequestNotifRow : public QFrame {
    Q_OBJECT
public:
    FriendRequestNotifRow(const QString &message, const QString &time, int fromUserId,
                          QWidget *parent = nullptr);

signals:
    void accepted(int fromUserId);
    void declined(int fromUserId);

private:
    int m_fromUserId;
};

// ─────────────────────────────────────────────────────
//  Main NotificationPage
// ─────────────────────────────────────────────────────
class NotificationPage : public QWidget {
    Q_OBJECT

public:
    explicit NotificationPage(QWidget *parent = nullptr);

    // Call after login to populate notifications for this user
    void loadNotifications(int userID);

    // Clear all rows
    void clearNotifications();

signals:
    void backClicked();
    void friendRequestAccepted(int fromUserId);
    void friendRequestDeclined(int fromUserId);

private:
    QVBoxLayout *listLayout;
    QLabel      *emptyLabel;

    void setupUI();
    void applyStyles();
};
