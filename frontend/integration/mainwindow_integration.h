// ═══════════════════════════════════════════════════════════════════
//  mainwindow_integration.h  —  Full Integration Guide for Connectify
//  QStackedWidget se sab pages switch honge
// ═══════════════════════════════════════════════════════════════════

#pragma once
#include <QMainWindow>
#include <QStackedWidget>

// ── Frontend pages ──
#include "loginpage.h"
#include "signuppage.h"
#include "newsfeedpage.h"

// ── Teammate pages ──
#include "profilepage.h"
#include "searchpage.h"
#include "messagepage.h"
#include "admindashboard.h"

// ── Backend ──
// NOTE: "user.h" resolves correctly because backend/include is in
//       target_include_directories — do NOT use a relative "../" path
//       as it breaks Qt's MOC tool during the build step.
#include "user.h"

// Globals from backend (declared in user.cpp / main.cpp)
extern User **users;
extern int userCount;
extern int nextID;
extern MessageSystem msgSystem;
extern NotificationSystem notifSystem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ── Auth ──
    void onLoginClicked(const QString &username, const QString &password);
    void onSignupClicked(const QString &fullName, const QString &username,
                         const QString &email, const QString &password);
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
    QStackedWidget *stack;

    // Pages
    LoginPage *loginPage;
    SignupPage *signupPage;
    NewsFeedPage *feedPage;
    ProfilePage *profilePage;
    SearchPage *searchPage;
    MessagePage *messagePage;
    AdminDashboard *adminPage;

    // Current session
    int m_loggedInIndex = -1;

    void loadFeed();
    void connectSignals();
};