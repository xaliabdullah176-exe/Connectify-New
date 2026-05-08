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
//  Single notification row
// ─────────────────────────────────────────────────────
class NotifRow : public QFrame {
    Q_OBJECT
public:
    NotifRow(const QString &message, const QString &time, QWidget *parent = nullptr);
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

private:
    QVBoxLayout *listLayout;
    QLabel      *emptyLabel;

    void setupUI();
    void applyStyles();
};
