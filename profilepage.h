#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QPixmap>

class ProfilePage : public QWidget {
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);

    // ── Call this to load user data (from backend later) ──
    void loadProfile(int    userID,
                     QString name,
                     QString handle,
                     QString bio,
                     QString location,
                     QString joinDate,
                     QString role,
                     int posts, int friends,
                     int followers, int following,
                     bool isOwn = true);

    // ── Add posts dynamically ──
    void addTextPost (int postID, QString name, QString handle,
                      QString content, QString timeAgo,
                      int likes, int comments);

    void addImagePost(int postID, QString name, QString handle,
                      QString content, QString imagePath,
                      QString timeAgo, int likes, int comments);

    // ── Add friend to left sidebar ──
    void addFriend(QString name, QString mutualCount);

    // ── Clear all posts (call before refresh) ──
    void clearPosts();

signals:
    void editProfileClicked();
    void createPostClicked();
    void viewAllFriendsClicked();

private:
    // Header widgets
    QLabel      *avatarLabel;
    QLabel      *nameLabel;
    QLabel      *handleLabel;
    QLabel      *bioLabel;
    QLabel      *locationLabel;
    QLabel      *joinDateLabel;
    QPushButton *editBtn;

    // Stats
    QLabel *postsNum, *friendsNum, *followersNum, *followingNum;

    // Left column
    QVBoxLayout *friendsLayout;
    QLabel      *friendsCountLabel;

    // Right column (posts)
    QVBoxLayout *postsLayout;

    void setupUI();
    void applyStyles();

    // Helpers
    QFrame* makePostCard   (int postID, QString name, QString handle,
                             QString content, QString imagePath,
                             QString timeAgo, int likes, int comments,
                             bool hasImage);
    QFrame* makeFriendItem (QString name, QString mutual);
    QWidget* makeStatCell  (QLabel *&numLabel, const QString &labelText);
};
