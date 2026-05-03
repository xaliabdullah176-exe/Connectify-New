// #ifndef ADMINDASHBOARD_H
// #define ADMINDASHBOARD_H

// #include <QWidget>

// class AdminDashboard : public QWidget
// {
//     Q_OBJECT
// public:
//     explicit AdminDashboard(QWidget *parent = nullptr);

// signals:
// };

// #endif // ADMINDASHBOARD_H

#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QGridLayout>
#include <QStackedWidget>

class AdminDashboard : public QWidget {
    Q_OBJECT

public:
    explicit AdminDashboard(QWidget *parent = nullptr);

    // ── Call after admin logs in ──
    void loadAdmin(int adminID, QString adminName);

    // ── Populate stats (pass values from backend) ──
    void setStats(int totalUsers, int totalPosts,
                  int bannedUsers, int postsToReview);

    // ── Add a user row to the table ──
    void addUserRow(int userID, QString name, QString role,
                    int postCount, bool isBanned);

    // ── Add a post to moderation section ──
    void addModPost(int postID, int ownerID, QString ownerName,
                    QString content, int likes, int comments);

    // ── Clear all rows (call before refresh) ──
    void clearUsers();
    void clearModPosts();

signals:
    void deleteUserClicked(int userID);
    void banUserClicked(int userID);
    void unbanUserClicked(int userID);
    void viewUserClicked(int userID);
    void deletePostClicked(int postID);
    void logoutClicked();
    void navDashboard();
    void navUsers();
    void navPosts();

private:
    // Sidebar
    QLabel      *adminAvatarLabel;
    QLabel      *adminNameLabel;
    QPushButton *navDashBtn;
    QPushButton *navUsersBtn;
    QPushButton *navPostsBtn;
    QPushButton *logoutBtn;

    // Topbar
    QLabel      *pageTitleLabel;
    QLabel      *pageSubLabel;

    // Stat cards
    QLabel      *totalUsersNum;
    QLabel      *totalPostsNum;
    QLabel      *bannedUsersNum;
    QLabel      *reviewPostsNum;

    // User table
    QVBoxLayout *userTableLayout;

    // Mod posts grid
    QGridLayout *modPostsLayout;
    int          modPostCount = 0;

    // Content area (switched by sidebar nav)
    QStackedWidget *contentStack;
    QWidget        *dashboardPage;
    QWidget        *usersPage;
    QWidget        *postsPage;

    void setupUI();
    void applyStyles();
    void setActiveSidebarBtn(QPushButton* active);

    QFrame*  makeStatCard   (QLabel *&numLabel, QString icon,
                         QString labelText, QString changeText,
                         bool isWarning = false);
    QFrame*  makeUserRow    (int userID, QString name, QString role,
                        int postCount, bool isBanned);
    QFrame*  makeModPostCard(int postID, int ownerID, QString ownerName,
                            QString content, int likes, int comments);
    QWidget* makeSidebarBtn (QPushButton *&btn, QString icon, QString text);
};