#include "mainwindow.h"

// ═══════════════════════════════════════════════════════
//  CONSTRUCTOR
// ═══════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Load saved data on startup
    loadData();

    setWindowTitle("Connectify");
    resize(1100, 750);
    setMinimumSize(900, 650);

    // Build stacked widget
    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    // Create all pages
    loginPage   = new LoginPage(this);
    signupPage  = new SignupPage(this);
    feedPage    = new NewsFeedPage(this);
    profilePage = new ProfilePage(this);
    searchPage  = new SearchPage(this);
    messagePage = new MessagePage(this);
    adminPage   = new AdminDashboard(this);

    // Add to stack — ORDER MATTERS (matches index numbers)
    stack->addWidget(loginPage);    // 0
    stack->addWidget(signupPage);   // 1
    stack->addWidget(feedPage);     // 2
    stack->addWidget(profilePage);  // 3
    stack->addWidget(searchPage);   // 4
    stack->addWidget(messagePage);  // 5
    stack->addWidget(adminPage);    // 6

    connectSignals();
    showLogin();
}

// ═══════════════════════════════════════════════════════
//  DESTRUCTOR — save data on exit
// ═══════════════════════════════════════════════════════
MainWindow::~MainWindow() {
    saveData();
}

// ═══════════════════════════════════════════════════════
//  CONNECT ALL SIGNALS
// ═══════════════════════════════════════════════════════
void MainWindow::connectSignals() {
    // ── LoginPage ──
    // Support both signal styles (old: goToSignup/loginSuccess, new: signupLinkClicked/loginClicked)
    connect(loginPage, &LoginPage::signupLinkClicked, this, &MainWindow::showSignup);
    connect(loginPage, &LoginPage::loginClicked,      this, &MainWindow::onLoginClicked);
    // Legacy signals (in case LoginPage uses these names)
    // connect(loginPage, &LoginPage::goToSignup,   this, &MainWindow::showSignup);
    // connect(loginPage, &LoginPage::loginSuccess, this, &MainWindow::onLoginSuccess);

    // ── SignupPage ──
    connect(signupPage, &SignupPage::signupClicked,    this, &MainWindow::onSignupClicked);
    connect(signupPage, &SignupPage::loginLinkClicked, this, &MainWindow::showLogin);

    // ── NewsFeedPage ──
    connect(feedPage, &NewsFeedPage::createPostClicked, this, &MainWindow::onCreatePost);
    connect(feedPage, &NewsFeedPage::likeClicked,       this, &MainWindow::onLikePost);
    connect(feedPage, &NewsFeedPage::commentClicked,    this, &MainWindow::onCommentPost);
    connect(feedPage, &NewsFeedPage::logoutClicked,     this, &MainWindow::onLogout);
    connect(feedPage, &NewsFeedPage::goToProfile,       this, &MainWindow::showProfile);
    connect(feedPage, &NewsFeedPage::goToMessages,      this, &MainWindow::showMessages);
    connect(feedPage, &NewsFeedPage::goToSearch,        this, &MainWindow::showSearch);

    // ── ProfilePage (Arslan) ──
    connect(profilePage, &ProfilePage::editProfileClicked, this, [this]() {
    bool ok;
    QString newName = QInputDialog::getText(
        this, "Edit Profile", "Enter new username:",
        QLineEdit::Normal,
        QString::fromStdString(users[m_loggedInIndex]->userName), &ok
    );
    if (ok && !newName.trimmed().isEmpty()) {
        users[m_loggedInIndex]->userName = newName.toStdString();
        showProfile();
    }
});
    connect(profilePage, &ProfilePage::viewAllFriendsClicked, this, &MainWindow::showSearch);
    connect(profilePage, &ProfilePage::createPostClicked, this, &MainWindow::showNewsFeed);
    connect(profilePage, &ProfilePage::createPostClicked, this, [this]() {
    showNewsFeed();
});
    // ── SearchPage (Arslan) ──
    connect(searchPage, &SearchPage::viewProfileClicked, this, [this](int userID) {
        // Load that user's profile
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) {
                profilePage->loadProfile(
                    users[i]->userID,
                    QString::fromStdString(users[i]->userName),
                    QString::fromStdString(users[i]->userName),
                    "", "", "2025",
                    QString::fromStdString(users[i]->role),
                    users[i]->postCount, users[i]->friendCount,
                    users[i]->followerCount, users[i]->followingCount,
                    (users[i]->userID == users[m_loggedInIndex]->userID)
                );
                stack->setCurrentIndex(3);
                break;
            }
        }
    });
    connect(searchPage, &SearchPage::addFriendClicked, this, [this](int userID) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) {
                users[m_loggedInIndex]->sendRequest(users[i]);
                break;
            }
        }
    });
    connect(searchPage, &SearchPage::backClicked, this, &MainWindow::showNewsFeed);

    // ── MessagePage (Arslan) ──
    connect(messagePage, &MessagePage::sendMessageClicked,
            this, [this](int toUserID, const QString &text) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == toUserID) {
                msgSystem.sendMessage(users[m_loggedInIndex], users[i],
                                      text.toStdString());
                break;
            }
        }
    });
    connect(messagePage, &MessagePage::backClicked,   this, &MainWindow::showNewsFeed);
    connect(messagePage, &MessagePage::logoutClicked, this, &MainWindow::onLogout);

    // ── AdminDashboard (Arslan) ──
    connect(adminPage, &AdminDashboard::logoutClicked, this, &MainWindow::onLogout);
    connect(adminPage, &AdminDashboard::deleteUserClicked, this, [this](int userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) {
                adminDelete(m_loggedInIndex,
                            users[i]->userName);
                // Refresh admin page
                showAdminDashboard();
                break;
            }
        }
    });
    connect(adminPage, &AdminDashboard::navDashboard, this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::navUsers,     this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::navPosts,     this, &MainWindow::showAdminDashboard);
}

// ═══════════════════════════════════════════════════════
//  LOGIN
// ═══════════════════════════════════════════════════════
void MainWindow::onLoginClicked(const QString &username, const QString &password) {
    int index = login(username.toStdString(), password.toStdString());

    if (index == -1) {
        loginPage->showError("Invalid username or password.");
        return;
    }

    m_loggedInIndex = index;
    loginPage->clearFields();

    if (users[index]->role == "admin" || users[index]->role == "Admin")
        showAdminDashboard();
    else
        showNewsFeed();
}

// Legacy slot — called if LoginPage emits loginSuccess(bool)
void MainWindow::onLoginSuccess(bool isAdmin) {
    if (isAdmin)
        showAdminDashboard();
    else
        showNewsFeed();
}

// ═══════════════════════════════════════════════════════
//  SIGNUP
// ═══════════════════════════════════════════════════════
void MainWindow::onSignupClicked(const QString &fullName,
                                  const QString &username,
                                  const QString &email,
                                  const QString &password)
{
    (void)fullName; (void)email; // backend doesn't use these yet

    if (userNameExist(username.toStdString())) {
        signupPage->showError("Username already taken. Choose another.");
        return;
    }

    signup(nextID++, username.toStdString(), password.toStdString(), "user");

    int index = login(username.toStdString(), password.toStdString());
    if (index == -1) {
        signupPage->showError("Account created! Please log in.");
        showLogin();
        return;
    }

    m_loggedInIndex = index;
    signupPage->clearFields();
    showNewsFeed();
}

// ═══════════════════════════════════════════════════════
//  LOGOUT
// ═══════════════════════════════════════════════════════
void MainWindow::onLogout() {
    saveData();
    m_loggedInIndex = -1;
    showLogin();
}

// ═══════════════════════════════════════════════════════
//  CREATE POST
// ═══════════════════════════════════════════════════════
void MainWindow::onCreatePost(const QString &content, const QString &imagePath) {
    if (m_loggedInIndex == -1) return;

    Post *p = new Post(nextID++, content.toStdString());
    users[m_loggedInIndex]->createPost(p);

    QString uname = QString::fromStdString(users[m_loggedInIndex]->userName);
    feedPage->addPost(p->postID, uname, content, imagePath, "just now", 0, 0, true);

    saveData();
}

// ═══════════════════════════════════════════════════════
//  LIKE POST
// ═══════════════════════════════════════════════════════
void MainWindow::onLikePost(int postID) {
    if (m_loggedInIndex == -1) return;

    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->postCount; j++) {
            if (users[i]->posts[j]->postID == postID) {
                users[i]->posts[j]->like();
                notifSystem.addNotification(
                    users[i]->userID,
                    users[m_loggedInIndex]->userName + " liked your post."
                );
                return;
            }
        }
    }
}

// ═══════════════════════════════════════════════════════
//  COMMENT ON POST
// ═══════════════════════════════════════════════════════
void MainWindow::onCommentPost(int postID) {
    if (m_loggedInIndex == -1) return;

    bool ok;
    QString comment = QInputDialog::getText(
        this, "Add Comment", "Your comment:",
        QLineEdit::Normal, "", &ok
    );
    if (!ok || comment.trimmed().isEmpty()) return;

    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->postCount; j++) {
            if (users[i]->posts[j]->postID == postID) {
                std::string full = users[m_loggedInIndex]->userName
                                 + ": " + comment.toStdString();
                users[i]->posts[j]->addComment(full);
                notifSystem.addNotification(
                    users[i]->userID,
                    users[m_loggedInIndex]->userName + " commented on your post."
                );
                return;
            }
        }
    }
}

// ═══════════════════════════════════════════════════════
//  LOAD FEED
// ═══════════════════════════════════════════════════════
void MainWindow::loadFeed() {
    feedPage->clearFeed();
    if (m_loggedInIndex == -1) return;

    User *me = users[m_loggedInIndex];

    struct FeedItem { Post *post; User *owner; };
    QList<FeedItem> items;

    // Posts from people I follow
    for (int i = 0; i < me->followingCount; i++) {
        User *u = me->following[i];
        for (int j = 0; j < u->postCount; j++)
            items.append({u->posts[j], u});
    }
    // My own posts
    for (int i = 0; i < me->postCount; i++)
        items.append({me->posts[i], me});

    // Sort latest first
    std::sort(items.begin(), items.end(), [](const FeedItem &a, const FeedItem &b) {
        return a.post->timestamp > b.post->timestamp;
    });

    for (auto &item : items) {
        QString ownerName = QString::fromStdString(item.owner->userName);
        QString content   = QString::fromStdString(item.post->content);

        // Time ago
        time_t now  = time(nullptr);
        double diff = difftime(now, item.post->timestamp);
        QString timeAgo;
        if      (diff < 60)    timeAgo = "just now";
        else if (diff < 3600)  timeAgo = QString("%1m ago").arg((int)(diff/60));
        else if (diff < 86400) timeAgo = QString("%1h ago").arg((int)(diff/3600));
        else                   timeAgo = QString("%1d ago").arg((int)(diff/86400));

        // canInteract: own post or friend's post
        bool canInteract = (item.owner == me);
        if (!canInteract)
            for (int i = 0; i < me->friendCount; i++)
                if (me->friends[i] == item.owner) { canInteract = true; break; }

        feedPage->addPost(
            item.post->postID, ownerName, content,
            "",   // imagePath — extend Post if needed
            timeAgo,
            item.post->likeCount,
            item.post->commentCount,
            canInteract
        );
    }
}

// ═══════════════════════════════════════════════════════
//  NAVIGATION
// ═══════════════════════════════════════════════════════
void MainWindow::showLogin() {
    stack->setCurrentIndex(0);
}

void MainWindow::showSignup() {
    stack->setCurrentIndex(1);
}

void MainWindow::showNewsFeed() {
    if (m_loggedInIndex == -1) return;
    QString name = QString::fromStdString(users[m_loggedInIndex]->userName);
    feedPage->loadCurrentUser(users[m_loggedInIndex]->userID, name);
    loadFeed();
    stack->setCurrentIndex(2);
}

void MainWindow::showProfile() {
    if (m_loggedInIndex == -1) return;
    User *u = users[m_loggedInIndex];
    profilePage->clearPosts();
    profilePage->loadProfile(
        u->userID,
        QString::fromStdString(u->userName),
        QString::fromStdString(u->userName),
        "", "", "2025",
        QString::fromStdString(u->role),
        u->postCount, u->friendCount,
        u->followerCount, u->followingCount,
        true
    );
    // Load user's own posts into profile
    for (int i = 0; i < u->postCount; i++) {
        profilePage->addTextPost(
            u->posts[i]->postID,
            QString::fromStdString(u->userName),
            QString::fromStdString(u->userName),
            QString::fromStdString(u->posts[i]->content),
            "recently",
            u->posts[i]->likeCount,
            u->posts[i]->commentCount
        );
    }
    // Load friends into sidebar
    for (int i = 0; i < u->friendCount; i++) {
        profilePage->addFriend(
            QString::fromStdString(u->friends[i]->userName),
            "mutual friend"
        );
    }
    stack->setCurrentIndex(3);
}

void MainWindow::showSearch() {
    if (m_loggedInIndex == -1) return;
    searchPage->loadCurrentUser(
        users[m_loggedInIndex]->userID,
        QString::fromStdString(users[m_loggedInIndex]->userName)
    );
    searchPage->clearResults();
    for (int i = 0; i < userCount; i++) {
        if (users[i]->userID == users[m_loggedInIndex]->userID) continue;
        bool isFriend = false;
        for (int j = 0; j < users[m_loggedInIndex]->friendCount; j++)
            if (users[m_loggedInIndex]->friends[j] == users[i])
                { isFriend = true; break; }
        searchPage->addUser(
            users[i]->userID,
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->userName),
            "", isFriend
        );
    }
    stack->setCurrentIndex(4);
}

void MainWindow::showMessages() {
    if (m_loggedInIndex == -1) return;
    messagePage->loadCurrentUser(
        users[m_loggedInIndex]->userID,
        QString::fromStdString(users[m_loggedInIndex]->userName)
    );
    // Load friends as contacts
    User *me = users[m_loggedInIndex];
    for (int i = 0; i < me->friendCount; i++) {
        messagePage->addContact(
            me->friends[i]->userID,
            QString::fromStdString(me->friends[i]->userName),
            "Click to chat",
            "", true
        );
    }
    stack->setCurrentIndex(5);
}

void MainWindow::showAdminDashboard() {
    if (m_loggedInIndex == -1) return;
    User *u = users[m_loggedInIndex];
    adminPage->clearUsers();
    adminPage->clearModPosts();
    adminPage->loadAdmin(u->userID, QString::fromStdString(u->userName));

    // Count total posts
    int totalPosts = 0;
    for (int i = 0; i < userCount; i++)
        totalPosts += users[i]->postCount;

    adminPage->setStats(userCount, totalPosts, 0, 0);

    for (int i = 0; i < userCount; i++) {
        adminPage->addUserRow(
            users[i]->userID,
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->role),
            users[i]->postCount,
            false
        );
    }
    stack->setCurrentIndex(6);
}
