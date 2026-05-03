#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>

// ─────────────────────────────────────────────────────
//  Single post card shown in the feed
// ─────────────────────────────────────────────────────
class FeedPostCard : public QFrame
{
    Q_OBJECT
public:
    FeedPostCard(int postID,
                 const QString &ownerName,
                 const QString &content,
                 const QString &imagePath, // "" if text-only post
                 const QString &timeAgo,
                 int likes,
                 int comments,
                 bool canInteract, // false if not friend/self
                 QWidget *parent = nullptr);

    void updateLikes(int newCount);

signals:
    void likeClicked(int postID);
    void commentClicked(int postID);

private:
    int m_postID;
    QLabel *likesLabel;
    QLabel *commentsLabel;
};

// ─────────────────────────────────────────────────────
//  NewsFeedPage
// ─────────────────────────────────────────────────────
class NewsFeedPage : public QWidget
{
    Q_OBJECT

public:
    explicit NewsFeedPage(QWidget *parent = nullptr);

    // ── Call after login ──
    void loadCurrentUser(int userID, const QString &name);

    // ── Add a post to the feed (call in timestamp order, latest first) ──
    void addPost(int postID,
                 const QString &ownerName,
                 const QString &content,
                 const QString &imagePath, // "" for text post
                 const QString &timeAgo,
                 int likes,
                 int comments,
                 bool canInteract = true);

    // ── Clear feed (call before refresh) ──
    void clearFeed();

    // ── Update like count on a specific card ──
    void updatePostLikes(int postID, int newCount);

signals:
    // ── Backend integration hooks ──

    // User typed text (+optional image) and clicked Post
    // → call: Post* p = new Post(nextID++, content.toStdString());
    //          users[loggedInIndex]->createPost(p);
    void createPostClicked(const QString &content, const QString &imagePath);

    // User clicked ❤️ Like
    // → find post by postID in users array, call post->like()
    void likeClicked(int postID);

    // User clicked 💬 Comment
    void commentClicked(int postID);

    // Navbar signals
    void logoutClicked();
    void goToProfile();
    void goToMessages();
    void goToSearch();
    void goToNotifications();

private slots:
    void onPostBtnClicked();
    void onSelectImageClicked();

private:
    // ── State ──────────────────────────────────────────
    int m_currentUserID = -1;
    QString m_currentUserName;
    QString m_selectedImagePath;

    // ── Navbar ─────────────────────────────────────────
    QLabel *navUserNameLabel;
    QLabel *navAvatarLabel;

    // ── Create post box ────────────────────────────────
    QTextEdit *postInput;
    QLabel *imagePreviewLabel;
    QPushButton *selectImageBtn;
    QPushButton *postBtn;
    QLabel *charCountLabel;

    // ── Feed ───────────────────────────────────────────
    QVBoxLayout *feedLayout; // holds FeedPostCard widgets + stretch

    // ── Helpers ────────────────────────────────────────
    void setupUI();
    void applyStyles();
    QString makeInitials(const QString &name);
    QString formatTimeAgo(time_t timestamp);
};
