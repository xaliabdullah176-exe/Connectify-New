// ═══════════════════════════════════════════════════════════════════
//  mainwindow.h  —  Full Integration Guide for Connectify
//  QStackedWidget se sab pages switch honge
// ═══════════════════════════════════════════════════════════════════

#pragma once
#include <QMainWindow>
#include <QStackedWidget>

// ── Frontend pages ──
#include "loginpage.h"       // tumhara (already done)
#include "signuppage.h"      // tumhara
#include "newsfeedpage.h"    // tumhara

// ── Teammate pages ──
#include "profilepage.h"
#include "searchpage.h"
#include "messagepage.h"
#include "admindashboard.h"

// ── Backend ──
#include "../backend/user.h"   // adjust path as needed

// Globals from backend (declared in user.cpp / main.cpp)
extern User**   users;
extern int      userCount;
extern int      nextID;        // keep incrementing for new IDs
extern MessageSystem    msgSystem;
extern NotificationSystem notifSystem;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ── Auth ──
    void onLoginClicked(const QString &username, const QString &password);
    void onSignupClicked(const QString &fullName, const QString &username,
                         const QString &email,    const QString &password);
    void onLogout();

    // ── NewsFeed actions ──
    void onCreatePost(const QString &content, const QString &imagePath);
    void onLikePost(int postID);
    void onCommentPost(int postID);

    // ── Navigation ──
    void showLogin();
    void showSignup();
    void showNewsFeed();
    void showProfile();
    void showMessages();
    void showSearch();
    void showAdminDashboard();

private:
    QStackedWidget   *stack;

    // Pages
    LoginPage        *loginPage;
    SignupPage       *signupPage;
    NewsFeedPage     *feedPage;
    ProfilePage      *profilePage;
    SearchPage       *searchPage;
    MessagePage      *messagePage;
    AdminDashboard   *adminPage;

    // Current session
    int  m_loggedInIndex = -1;

    void loadFeed();     // populates feedPage with backend data
    void connectSignals();
};


// ═══════════════════════════════════════════════════════════════════
//  mainwindow.cpp
// ═══════════════════════════════════════════════════════════════════

/*
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // ── Load saved data on startup ──────────────────────
    loadData();   // from filehandling.cpp

    // ── Build stacked widget ────────────────────────────
    stack = new QStackedWidget(this);
    setCentralWidget(stack);
    resize(1100, 750);
    setWindowTitle("Connectify");

    loginPage   = new LoginPage();
    signupPage  = new SignupPage();
    feedPage    = new NewsFeedPage();
    profilePage = new ProfilePage();
    searchPage  = new SearchPage();
    messagePage = new MessagePage();
    adminPage   = new AdminDashboard();

    stack->addWidget(loginPage);    // index 0
    stack->addWidget(signupPage);   // index 1
    stack->addWidget(feedPage);     // index 2
    stack->addWidget(profilePage);  // index 3
    stack->addWidget(searchPage);   // index 4
    stack->addWidget(messagePage);  // index 5
    stack->addWidget(adminPage);    // index 6

    connectSignals();
    showLogin();
}

MainWindow::~MainWindow() {
    saveData();   // persist to files on exit
}

// ─────────────────────────────────────────────────────
//  CONNECT ALL SIGNALS
// ─────────────────────────────────────────────────────
void MainWindow::connectSignals() {
    // Login page
    connect(loginPage,  &LoginPage::loginClicked,       this, &MainWindow::onLoginClicked);
    connect(loginPage,  &LoginPage::signupLinkClicked,  this, &MainWindow::showSignup);

    // Signup page
    connect(signupPage, &SignupPage::signupClicked,     this, &MainWindow::onSignupClicked);
    connect(signupPage, &SignupPage::loginLinkClicked,  this, &MainWindow::showLogin);

    // Feed page
    connect(feedPage, &NewsFeedPage::createPostClicked, this, &MainWindow::onCreatePost);
    connect(feedPage, &NewsFeedPage::likeClicked,       this, &MainWindow::onLikePost);
    connect(feedPage, &NewsFeedPage::commentClicked,    this, &MainWindow::onCommentPost);
    connect(feedPage, &NewsFeedPage::logoutClicked,     this, &MainWindow::onLogout);
    connect(feedPage, &NewsFeedPage::goToProfile,       this, &MainWindow::showProfile);
    connect(feedPage, &NewsFeedPage::goToMessages,      this, &MainWindow::showMessages);
    connect(feedPage, &NewsFeedPage::goToSearch,        this, &MainWindow::showSearch);
}

// ─────────────────────────────────────────────────────
//  LOGIN
// ─────────────────────────────────────────────────────
void MainWindow::onLoginClicked(const QString &username, const QString &password) {
    int index = login(username.toStdString(), password.toStdString());

    if (index == -1) {
        loginPage->showError("Invalid username or password.");
        return;
    }

    m_loggedInIndex = index;
    loginPage->clearFields();

    // Route: admin vs normal user
    if (users[index]->role == "admin" || users[index]->role == "Admin") {
        showAdminDashboard();
    } else {
        showNewsFeed();
    }
}

// ─────────────────────────────────────────────────────
//  SIGNUP
// ─────────────────────────────────────────────────────
void MainWindow::onSignupClicked(const QString &fullName,
                                  const QString &username,
                                  const QString &email,
                                  const QString &password)
{
    // Check if username already exists (backend handles this too)
    if (userNameExist(username.toStdString())) {
        signupPage->showError("Username already taken. Choose another.");
        return;
    }

    // NOTE: backend signup() only takes (id, username, password, role)
    // fullName and email are Qt-side only for now — extend User class if needed
    signup(nextID++, username.toStdString(), password.toStdString(), "user");

    // Auto-login after signup
    int index = login(username.toStdString(), password.toStdString());
    if (index == -1) {
        signupPage->showError("Account created but login failed. Try logging in.");
        showLogin();
        return;
    }

    m_loggedInIndex = index;
    signupPage->clearFields();
    showNewsFeed();
}

// ─────────────────────────────────────────────────────
//  LOGOUT
// ─────────────────────────────────────────────────────
void MainWindow::onLogout() {
    saveData();   // save before logging out
    m_loggedInIndex = -1;
    showLogin();
}

// ─────────────────────────────────────────────────────
//  CREATE POST
// ─────────────────────────────────────────────────────
void MainWindow::onCreatePost(const QString &content, const QString &imagePath) {
    if (m_loggedInIndex == -1) return;

    // Create backend post
    Post *p = new Post(nextID++, content.toStdString());
    users[m_loggedInIndex]->createPost(p);

    // Immediately show it at top of feed
    QString username = QString::fromStdString(users[m_loggedInIndex]->userName);
    feedPage->addPost(p->postID, username, content, imagePath, "just now", 0, 0, true);

    saveData();   // persist
}

// ─────────────────────────────────────────────────────
//  LIKE POST
// ─────────────────────────────────────────────────────
void MainWindow::onLikePost(int postID) {
    if (m_loggedInIndex == -1) return;

    // Search all users' posts for this postID
    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->postCount; j++) {
            if (users[i]->posts[j]->postID == postID) {
                users[i]->posts[j]->like();

                // Notify post owner
                notifSystem.addNotification(
                    users[i]->userID,
                    users[m_loggedInIndex]->userName + " liked your post."
                );
                return;
            }
        }
    }
}

// ─────────────────────────────────────────────────────
//  COMMENT ON POST  (opens a simple input dialog)
// ─────────────────────────────────────────────────────
void MainWindow::onCommentPost(int postID) {
    if (m_loggedInIndex == -1) return;

    bool ok;
    QString comment = QInputDialog::getText(
        this, "Add Comment", "Your comment:", QLineEdit::Normal, "", &ok
    );
    if (!ok || comment.trimmed().isEmpty()) return;

    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->postCount; j++) {
            if (users[i]->posts[j]->postID == postID) {
                string fullComment = users[m_loggedInIndex]->userName
                                   + ": " + comment.toStdString();
                users[i]->posts[j]->addComment(fullComment);

                notifSystem.addNotification(
                    users[i]->userID,
                    users[m_loggedInIndex]->userName + " commented on your post."
                );
                return;
            }
        }
    }
}

// ─────────────────────────────────────────────────────
//  LOAD FEED  (call after login / refresh)
// ─────────────────────────────────────────────────────
void MainWindow::loadFeed() {
    feedPage->clearFeed();

    User *me = users[m_loggedInIndex];
    QString myName = QString::fromStdString(me->userName);

    // Collect posts: following + own (same logic as backend showNewsFeed)
    struct FeedItem { Post* post; User* owner; };
    QList<FeedItem> items;

    for (int i = 0; i < me->followingCount; i++) {
        User *u = me->following[i];
        for (int j = 0; j < u->postCount; j++)
            items.append({u->posts[j], u});
    }
    for (int i = 0; i < me->postCount; i++)
        items.append({me->posts[i], me});

    // Sort by timestamp (latest first)
    std::sort(items.begin(), items.end(), [](const FeedItem &a, const FeedItem &b) {
        return a.post->timestamp > b.post->timestamp;
    });

    for (auto &item : items) {
        QString ownerName = QString::fromStdString(item.owner->userName);
        QString content   = QString::fromStdString(item.post->content);
        QString timeAgo   = feedPage->formatTimeAgo(item.post->timestamp); // make public if needed

        // canInteract = I am friends with owner OR it's my own post
        bool canInteract = (item.owner == me);
        if (!canInteract) {
            for (int i = 0; i < me->friendCount; i++)
                if (me->friends[i] == item.owner) { canInteract = true; break; }
        }

        feedPage->addPost(item.post->postID, ownerName, content,
                          "",  // imagePath: extend Post class for images
                          timeAgo,
                          item.post->likeCount,
                          item.post->commentCount,
                          canInteract);
    }
}

// ─────────────────────────────────────────────────────
//  NAVIGATION
// ─────────────────────────────────────────────────────
void MainWindow::showLogin() {
    stack->setCurrentIndex(0);
}

void MainWindow::showSignup() {
    stack->setCurrentIndex(1);
}

void MainWindow::showNewsFeed() {
    QString name = QString::fromStdString(users[m_loggedInIndex]->userName);
    feedPage->loadCurrentUser(users[m_loggedInIndex]->userID, name);
    loadFeed();
    stack->setCurrentIndex(2);
}

void MainWindow::showProfile() {
    // Teammate's ProfilePage — load current user data
    User *u = users[m_loggedInIndex];
    profilePage->loadProfile(
        u->userID,
        QString::fromStdString(u->userName),
        QString::fromStdString(u->userName),  // handle = username
        "",                                    // bio (extend User if needed)
        "", "2025",
        QString::fromStdString(u->role),
        u->postCount, u->friendCount,
        u->followerCount, u->followingCount,
        true
    );
    stack->setCurrentIndex(3);
}

void MainWindow::showMessages() {
    messagePage->loadCurrentUser(
        users[m_loggedInIndex]->userID,
        QString::fromStdString(users[m_loggedInIndex]->userName)
    );
    stack->setCurrentIndex(5);
}

void MainWindow::showSearch() {
    searchPage->loadCurrentUser(
        users[m_loggedInIndex]->userID,
        QString::fromStdString(users[m_loggedInIndex]->userName)
    );
    // Load all users into search
    searchPage->clearResults();
    for (int i = 0; i < userCount; i++) {
        if (users[i]->userID == users[m_loggedInIndex]->userID) continue;
        bool isFriend = false;
        for (int j = 0; j < users[m_loggedInIndex]->friendCount; j++)
            if (users[m_loggedInIndex]->friends[j] == users[i]) { isFriend = true; break; }
        searchPage->addUser(
            users[i]->userID,
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->userName),
            "", isFriend
        );
    }
    stack->setCurrentIndex(4);
}

void MainWindow::showAdminDashboard() {
    User *u = users[m_loggedInIndex];
    adminPage->loadAdmin(u->userID, QString::fromStdString(u->userName));
    adminPage->setStats(userCount, 0, 0, 0);
    for (int i = 0; i < userCount; i++) {
        adminPage->addUserRow(
            users[i]->userID,
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->role),
            users[i]->postCount, false
        );
    }
    stack->setCurrentIndex(6);
}
*/
