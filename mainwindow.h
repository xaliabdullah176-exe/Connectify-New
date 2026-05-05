#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QInputDialog>
#include <algorithm>

// ── Frontend pages ──
#include "LoginPage.h"
#include "signuppage.h"
#include "newsfeedpage.h"
#include "profilepage.h"
#include "searchpage.h"
#include "messagepage.h"
#include "admindashboard.h"

#include "backend/user.h"

// ── Backend globals (defined in user.cpp / main.cpp) ──
extern User**              users;
extern int                 userCount;
extern int                 nextID;
extern MessageSystem       msgSystem;
extern NotificationSystem  notifSystem;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // ── Auth ──
    void onLoginClicked(const QString &username, const QString &password);
    void onLoginSuccess(bool isAdmin);   // from existing LoginPage signal
    void onSignupClicked(const QString &fullName, const QString &username,
                         const QString &email,    const QString &password);
    void onLogout();

    // ── Feed actions ──
    void onCreatePost(const QString &content, const QString &imagePath);
    void onLikePost(int postID);
    void onCommentPost(int postID);
    void onDeletePost(int postID);

    // ── Navigation ──
    void showLogin();
    void showSignup();
    void showNewsFeed();
    void showProfile();
    void showMessages();
    void showSearch();
    void showAdminDashboard();

private:
    QStackedWidget  *stack;

    // ── Pages ──
    LoginPage       *loginPage;
    SignupPage      *signupPage;
    NewsFeedPage    *feedPage;
    ProfilePage     *profilePage;
    SearchPage      *searchPage;
    MessagePage     *messagePage;
    AdminDashboard  *adminPage;

    // ── Session ──
    int m_loggedInIndex = -1;

    // ── Helpers ──
    void connectSignals();
    void loadFeed();
};
