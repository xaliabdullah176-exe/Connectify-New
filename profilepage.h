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
                     QString profileImagePath,
                     QString bio,
                     QString location,
                     QString joinDate,
                     QString birthDate,
                     QString githubUsername,
                     QString email,
                     QString role,
                     int posts, int friends,
                     int followers, int following,
                     bool isOwn = true);

    // ── Add posts dynamically ──
    void addTextPost (int postID, QString name, QString handle,
                      QString content, QString timeAgo,
                      int likes, int comments, bool isLikedByMe = false,
                      const QStringList &commentsList = QStringList(),
                      bool isOwnPost = false);

    void addImagePost(int postID, QString name, QString handle,
                      QString content, QString imagePath,
                      QString timeAgo, int likes, int comments, bool isLikedByMe = false,
                      const QStringList &commentsList = QStringList(),
                      bool isOwnPost = false);

    // ── Add friend to left sidebar ──
    void addFriend(QString name, QString mutualCount);

    // ── Clear all posts (call before refresh) ──
    void clearPosts();
    void clearFriends();

signals:
    void backClicked();
    void editProfileClicked();
    void createPostClicked();
    void viewAllFriendsClicked();
    void deleteAccountClicked();
    void reportUserClicked(int userID);

    // User clicked 🗑️ Delete
    void deletePostClicked(int postID);
    void likeClicked(int postID);
    void commentClicked(int postID);

private:
    // Header widgets
    QLabel      *avatarLabel;
    QLabel      *nameLabel;
    QLabel      *handleLabel;
    QLabel      *headerEmailLabel;
    QLabel      *bioLabel;
    QLabel      *locationLabel;
    QLabel      *joinDateLabel;
    QPushButton *backBtn;
    QPushButton *editBtn;
    QPushButton *deleteAccountBtn;
    QPushButton *createPostBtn;
    QPushButton *reportUserBtn = nullptr;

    // Stats
    QLabel *postsNum, *friendsNum, *followersNum, *followingNum;

    // Left column
    QVBoxLayout *friendsLayout;
    QLabel      *friendsCountLabel;
    QLabel      *aboutBirthLabel;
    QLabel      *aboutGithubLabel;
    QLabel      *aboutEmailLabel;
    QPushButton *viewAllFriendsBtn;

    // Right column (posts)
    QVBoxLayout *postsLayout;
    bool         canDeletePosts = false;
    int          m_profileUserID = -1;

    void setupUI();
    void applyStyles();

    // Helpers
    QFrame* makePostCard   (int postID, QString name, QString handle,
                             QString content, QString imagePath,
                             QString timeAgo, int likes, int comments,
                             bool hasImage, bool isLikedByMe,
                             const QStringList &commentsList,
                             bool isOwnPost);
    QFrame* makeFriendItem (QString name, QString mutual);
    QWidget* makeStatCell  (QLabel *&numLabel, const QString &labelText);
};
