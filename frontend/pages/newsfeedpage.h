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
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#else
#include <QMediaPlayer>
#include <QVideoWidget>
#endif

// ─────────────────────────────────────────────────────
//  Single post card shown in the feed
// ─────────────────────────────────────────────────────
class FeedPostCard : public QFrame
{
    Q_OBJECT
public:
    FeedPostCard(int postID,
                 const QString &ownerName,
                 const QString &ownerAvatarPath,
                 const QString &content,
                 const QString &imagePath, // "" if no image
                 const QString &videoPath, // "" if no video
                 const QString &timeAgo,
                 int likes,
                 int comments,
                 bool canInteract, // false if not friend/self
                 bool isLikedByMe = false,
                 const QStringList &commentsList = QStringList(),
                 bool isOwnPost = false,
                 QWidget *parent = nullptr);

    void updateLikes(int newCount);

signals:
    void likeClicked(int postID);
    void commentClicked(int postID);
    void deleteClicked(int postID);

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
    void loadCurrentUser(int userID, const QString &name, const QString &avatarPath,
                         const QString &email = QString());

    // ── Add a post to the feed (call in timestamp order, latest first) ──
    void addPost(int postID,
                 const QString &ownerName,
                 const QString &ownerAvatarPath,
                 const QString &content,
                 const QString &imagePath,
                 const QString &videoPath,
                 const QString &timeAgo,
                 int likes,
                 int comments,
                 bool canInteract = true,
                 bool isLikedByMe = false,
                 const QStringList &commentsList = QStringList(),
                 bool isOwnPost = false);

    // ── Clear feed (call before refresh) ──
    void clearFeed();

    // ── Update like count on a specific card ──
    void updatePostLikes(int postID, int newCount);

    // ── Update notification badge count ──
    void setNotifBadge(int count);

signals:
    // ── Backend integration hooks ──

    // User typed text (+optional image) and clicked Post
    // → call: Post* p = new Post(nextID++, content.toStdString());
    //          users[loggedInIndex]->createPost(p);
    void createPostClicked(const QString &content, const QString &imagePath, const QString &videoPath);

    // User clicked ❤️ Like
    // → find post by postID in users array, call post->like()
    void likeClicked(int postID);

    // User clicked 💬 Comment
    void commentClicked(int postID);

    // User clicked 🗑️ Delete
    void deletePostClicked(int postID);

    // Navbar signals
    void logoutClicked();
    void goToProfile();
    void goToMessages();
    void goToSearch();
    void goToNotifications();
    void showOwnPostsClicked();
    void showNetworkPostsClicked();

private slots:
    void onPostBtnClicked();
    void onSelectMediaClicked();

private:
    // ── State ──────────────────────────────────────────
    int m_currentUserID = -1;
    QString m_currentUserName;
    QString m_selectedImagePath;
    QString m_selectedVideoPath;

    // ── Navbar ─────────────────────────────────────────
    QLabel *navUserNameLabel;
    QLabel *navUserEmailLabel;
    QLabel *navAvatarLabel;
    QPushButton *m_notifBtn;      // sidebar notification button
    QLabel      *m_notifBadge;    // red badge label overlaid on notifBtn

    // ── Create post box ────────────────────────────────
    QTextEdit *postInput;
    QLabel *imagePreviewLabel;
    QPushButton *selectMediaBtn;
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
