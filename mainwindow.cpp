#include "mainwindow.h"
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QLabel>
#include <QTextEdit>
#include <unordered_set>

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
    notifPage   = new NotificationPage(this);

    // Add to stack — ORDER MATTERS (matches index numbers)
    stack->addWidget(loginPage);    // 0
    stack->addWidget(signupPage);   // 1
    stack->addWidget(feedPage);     // 2
    stack->addWidget(profilePage);  // 3
    stack->addWidget(searchPage);   // 4
    stack->addWidget(messagePage);  // 5
    stack->addWidget(adminPage);    // 6
    stack->addWidget(notifPage);    // 7

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
    connect(feedPage, &NewsFeedPage::deletePostClicked, this, &MainWindow::onDeletePost);
    connect(feedPage, &NewsFeedPage::logoutClicked,     this, &MainWindow::onLogout);
    connect(feedPage, &NewsFeedPage::goToProfile,       this, [this]() { m_viewedUserID = -1; showProfile(); });
    connect(feedPage, &NewsFeedPage::goToMessages,      this, &MainWindow::showMessages);
    connect(feedPage, &NewsFeedPage::goToSearch,        this, &MainWindow::showSearch);
    connect(feedPage, &NewsFeedPage::goToNotifications, this, &MainWindow::showNotifications);
    connect(feedPage, &NewsFeedPage::showOwnPostsClicked, this, [this]() {
        m_feedViewMode = FeedViewMode::Home;
        loadFeed();
    });
    connect(feedPage, &NewsFeedPage::showNetworkPostsClicked, this, [this]() {
        m_feedViewMode = FeedViewMode::Network;
        loadFeed();
    });

    // ── ProfilePage (Arslan) ──
    connect(profilePage, &ProfilePage::likeClicked, this, &MainWindow::onLikePost);
    connect(profilePage, &ProfilePage::commentClicked, this, &MainWindow::onCommentPost);
    connect(profilePage, &ProfilePage::editProfileClicked, this, [this]() {
    if (m_loggedInIndex < 0) return;

    User *me = users[m_loggedInIndex];
    QDialog dlg(this);
    dlg.setWindowTitle("Edit About");

    QFormLayout *form = new QFormLayout(&dlg);
    QLineEdit *usernameEdit = new QLineEdit(QString::fromStdString(me->userName), &dlg);
    QDateEdit *birthEdit = new QDateEdit(&dlg);
    birthEdit->setCalendarPopup(true);
    birthEdit->setDisplayFormat("yyyy-MM-dd");
    const QDate parsedBirth = QDate::fromString(QString::fromStdString(me->birthDate), "yyyy-MM-dd");
    birthEdit->setDate(parsedBirth.isValid() ? parsedBirth : QDate::currentDate());
    QLineEdit *githubEdit = new QLineEdit(QString::fromStdString(me->githubUsername), &dlg);
    githubEdit->setPlaceholderText("e.g. mustafa-tech");

    // Profile photo
    QLabel *photoLabel = new QLabel(&dlg);
    photoLabel->setWordWrap(true);
    QString selectedPhotoPath = QString::fromStdString(me->profileImagePath);
    photoLabel->setText(selectedPhotoPath.isEmpty() ? "No photo selected" : selectedPhotoPath);

    QHBoxLayout *photoRow = new QHBoxLayout();
    QPushButton *choosePhotoBtn = new QPushButton("Choose Photo", &dlg);
    QPushButton *clearPhotoBtn  = new QPushButton("Clear", &dlg);
    photoRow->addWidget(choosePhotoBtn);
    photoRow->addWidget(clearPhotoBtn);

    connect(choosePhotoBtn, &QPushButton::clicked, &dlg, [&]() {
        const QString path = QFileDialog::getOpenFileName(
            this,
            "Select Profile Photo",
            "",
            "Images (*.png *.jpg *.jpeg *.gif *.bmp *.webp)"
        );
        if (path.isEmpty()) return;
        selectedPhotoPath = path;
        photoLabel->setText(path);
    });
    connect(clearPhotoBtn, &QPushButton::clicked, &dlg, [&]() {
        selectedPhotoPath.clear();
        photoLabel->setText("No photo selected");
    });

    form->addRow("Username", usernameEdit);
    form->addRow("Birthday", birthEdit);
    form->addRow("GitHub", githubEdit);
    form->addRow("Profile Photo", photoRow);
    form->addRow("", photoLabel);

    QDialogButtonBox *buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    const QString newName = usernameEdit->text().trimmed();
    if (newName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Username", "Username cannot be empty.");
        return;
    }

    for (int i = 0; i < userCount; ++i) {
        if (i != m_loggedInIndex &&
            QString::fromStdString(users[i]->userName).compare(newName, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "Username Exists", "This username is already taken.");
            return;
        }
    }

    me->userName = newName.toStdString();
    me->birthDate = birthEdit->date().toString("yyyy-MM-dd").toStdString();
    me->githubUsername = githubEdit->text().trimmed().toStdString();
    me->profileImagePath = selectedPhotoPath.trimmed().toStdString();
    saveData();
        showProfile();
});
    connect(profilePage, &ProfilePage::viewAllFriendsClicked, this, &MainWindow::showSearch);
    connect(profilePage, &ProfilePage::backClicked, this, [this]() {
        if (m_loggedInIndex != -1 &&
            (users[m_loggedInIndex]->role == "admin" || users[m_loggedInIndex]->role == "Admin") &&
            m_viewedUserID != -1) {
            m_viewedUserID = -1;
            showAdminDashboard();
            return;
        }

        if (m_viewedUserID != -1) {
            m_viewedUserID = -1;
            showSearch();
            return;
        }

        showNewsFeed();
    });
    connect(profilePage, &ProfilePage::createPostClicked, this, &MainWindow::showNewsFeed);
    connect(profilePage, &ProfilePage::createPostClicked, this, [this]() {
        showNewsFeed();
    });
    connect(profilePage, &ProfilePage::deletePostClicked, this, &MainWindow::onDeletePost);
    connect(profilePage, &ProfilePage::deleteAccountClicked, this, &MainWindow::onDeleteMyAccount);

    // ── SearchPage (Arslan) ──
    connect(searchPage, &SearchPage::viewProfileClicked, this, [this](int userID) {
        m_viewedUserID = userID;
        showProfile();
    });
    connect(searchPage, &SearchPage::addFriendClicked, this, [this](int userID) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) {
                users[m_loggedInIndex]->sendRequest(users[i]);
                users[i]->acceptRequest(users[m_loggedInIndex]);
                // Notify the added user
                notifSystem.addNotification(
                    users[i]->userID,
                    users[m_loggedInIndex]->userName + " added you as a friend!"
                );
                // Notify me too
                notifSystem.addNotification(
                    users[m_loggedInIndex]->userID,
                    "You are now friends with " + users[i]->userName + "!"
                );
                saveData();
                showSearch();
                break;
            }
        }
    });
    connect(searchPage, &SearchPage::backClicked, this, &MainWindow::showNewsFeed);

    // ── MessagePage (Arslan) ──
    connect(messagePage, &MessagePage::contactSelected, this, [this](int contactUserID) {
        if (m_loggedInIndex == -1) return;
        User* contactUser = nullptr;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == contactUserID) {
                contactUser = users[i];
                break;
            }
        }
        if (!contactUser) return;

        messagePage->clearMessages();
        messagePage->openConversation(
            contactUser->userID,
            QString::fromStdString(contactUser->userName),
            QString::fromStdString(contactUser->profileImagePath),
            true
        );

        User* me = users[m_loggedInIndex];
        for (int i = 0; i < msgSystem.msgCount; i++) {
            Message* m = msgSystem.msg[i];
            if ((m->senderID == me->userID && m->receiverID == contactUser->userID) ||
                (m->senderID == contactUser->userID && m->receiverID == me->userID)) {
                bool isSentByMe = (m->senderID == me->userID);
                
                QString timeStr = "";
                if (m->timestamp > 0) {
                    QDateTime dt = QDateTime::fromSecsSinceEpoch(m->timestamp);
                    timeStr = dt.toString("hh:mm AP");
                }
                
                messagePage->addMessage(QString::fromStdString(m->text), timeStr, isSentByMe);
            }
        }
    });

    connect(messagePage, &MessagePage::sendMessageClicked,
            this, [this](int toUserID, const QString &text) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == toUserID) {
                const bool sent = msgSystem.sendMessage(
                    users[m_loggedInIndex], users[i], text.toStdString()
                );
                if (sent) {
                    // Notify the receiver only if message was actually delivered.
                    notifSystem.addNotification(
                        toUserID,
                        users[m_loggedInIndex]->userName + " sent you a message: \"" + text.toStdString().substr(0, 40) + (text.length() > 40 ? "..." : "") + "\""
                    );
                    saveData();
                }
                break;
            }
        }
    });
    connect(messagePage, &MessagePage::backClicked,   this, &MainWindow::showNewsFeed);
    connect(messagePage, &MessagePage::logoutClicked, this, &MainWindow::onLogout);

    // ── AdminDashboard (Arslan) ──
    connect(adminPage, &AdminDashboard::logoutClicked, this, &MainWindow::onLogout);
    connect(adminPage, &AdminDashboard::createNewsClicked, this, &MainWindow::onAdminCreateNews);
    connect(adminPage, &AdminDashboard::deleteUserClicked, this, [this](int userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) {
                const QString targetName = QString::fromStdString(users[i]->userName);
                const auto reply = QMessageBox::question(
                    this,
                    "Confirm Delete",
                    "Delete user \"" + targetName + "\" permanently?",
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No
                );
                if (reply != QMessageBox::Yes) return;

                adminDelete(m_loggedInIndex,
                            users[i]->userName);
                // Refresh admin page
                saveData();
                showAdminDashboard();
                break;
            }
        }
    });
    connect(adminPage, &AdminDashboard::banUserClicked, this, [this](int userID) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID != userID) continue;
            if (users[i]->role == "admin" || users[i]->role == "Admin") return;
            if (users[i]->isBanned) return;

            const QString targetName = QString::fromStdString(users[i]->userName);
            const auto reply = QMessageBox::question(
                this,
                "Confirm Ban",
                "Ban user \"" + targetName + "\"?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );
            if (reply != QMessageBox::Yes) return;

            users[i]->isBanned = true;
            notifSystem.addNotification(
                users[i]->userID,
                "Your account has been banned by admin."
            );
            saveData();
            showAdminDashboard();
            return;
        }
    });
    connect(adminPage, &AdminDashboard::unbanUserClicked, this, [this](int userID) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID != userID) continue;
            if (users[i]->role == "admin" || users[i]->role == "Admin") return;
            if (!users[i]->isBanned) return;

            const QString targetName = QString::fromStdString(users[i]->userName);
            const auto reply = QMessageBox::question(
                this,
                "Confirm Unban",
                "Unban user \"" + targetName + "\"?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );
            if (reply != QMessageBox::Yes) return;

            users[i]->isBanned = false;
            notifSystem.addNotification(
                users[i]->userID,
                "Your account has been unbanned by admin."
            );
            saveData();
            showAdminDashboard();
            return;
        }
    });
    connect(adminPage, &AdminDashboard::navDashboard, this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::navUsers,     this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::navPosts,     this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::viewUserClicked, this, [this](int userID) {
        m_viewedUserID = userID;
        showProfile();
    });
    connect(adminPage, &AdminDashboard::deletePostClicked, this, [this](int postID) {
        if (m_loggedInIndex == -1) return;
        if (users[m_loggedInIndex]->role != "admin" && users[m_loggedInIndex]->role != "Admin") return;

        for (int i = 0; i < userCount; i++) {
            User* owner = users[i];
            for (int j = 0; j < owner->postCount; j++) {
                if (owner->posts[j]->postID != postID) continue;

                const auto reply = QMessageBox::question(
                    this,
                    "Confirm Post Delete",
                    "Delete this post from @" + QString::fromStdString(owner->userName) + "?",
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No
                );
                if (reply != QMessageBox::Yes) return;

                if (owner->deletePost(postID)) {
                    notifSystem.addNotification(
                        owner->userID,
                        "Your post was removed by admin moderation."
                    );
                    saveData();
                    showAdminDashboard();
                }
                return;
            }
        }
    });

    // ── NotificationPage ──
    connect(notifPage, &NotificationPage::backClicked, this, &MainWindow::showNewsFeed);
}

void MainWindow::onDeleteMyAccount() {
    if (m_loggedInIndex == -1) return;
    User *me = users[m_loggedInIndex];
    if (!me) return;
    if (me->role == "admin" || me->role == "Admin") {
        QMessageBox::warning(this, "Not Allowed", "Admin account cannot be deleted.");
        return;
    }

    const auto reply = QMessageBox::question(
        this,
        "Delete Account",
        "Are you sure you want to delete your account permanently?\nThis action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    if (reply != QMessageBox::Yes) return;

    const int oldIndex = m_loggedInIndex;
    m_loggedInIndex = -1;
    m_viewedUserID = -1;
    deleteAccount(oldIndex);
    saveData();
    showLogin();
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
    if (users[index]->isBanned) {
        loginPage->showError("Your account is banned by admin.");
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

    Post *p = new Post(nextID++, content.toStdString(), imagePath.toStdString());
    users[m_loggedInIndex]->createPost(p);

    // Notify all friends about the new post
    User *me = users[m_loggedInIndex];
    for (int i = 0; i < me->friendCount; i++) {
        notifSystem.addNotification(
            me->friends[i]->userID,
            me->userName + " shared a new post!"
        );
    }

    // Refresh based on active view (Home/Feed) after posting.
    loadFeed();
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
                bool isLiked = users[i]->posts[j]->toggleLike(users[m_loggedInIndex]->userID);
                if (isLiked) {
                    // Do not notify user for their own like.
                    if (users[i]->userID != users[m_loggedInIndex]->userID) {
                        notifSystem.addNotification(
                            users[i]->userID,
                            users[m_loggedInIndex]->userName + " liked your post."
                        );
                    }
                }
                
                // Refresh the UI to reflect the new like count
                if (stack->currentIndex() == 2) {
                    showNewsFeed();
                } else if (stack->currentIndex() == 3) {
                    showProfile();
                }
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
                // Do not notify user for their own comment.
                if (users[i]->userID != users[m_loggedInIndex]->userID) {
                    notifSystem.addNotification(
                        users[i]->userID,
                        users[m_loggedInIndex]->userName + " commented on your post."
                    );
                }

                // Refresh the UI to reflect the new comment count
                if (stack->currentIndex() == 2) {
                    showNewsFeed();
                } else if (stack->currentIndex() == 3) {
                    showProfile();
                }
                return;
            }
        }
    }
}

// ═══════════════════════════════════════════════════════
//  DELETE POST
// ═══════════════════════════════════════════════════════
void MainWindow::onDeletePost(int postID) {
    if (m_loggedInIndex == -1) return;
    User *me = users[m_loggedInIndex];
    const bool isAdmin = (me->role == "admin" || me->role == "Admin");

    bool deleted = false;

    if (isAdmin) {
        for (int i = 0; i < userCount && !deleted; i++) {
            if (users[i]->deletePost(postID)) {
                deleted = true;
            }
        }
    } else {
        deleted = me->deletePost(postID);
    }

    if (!deleted) return;

    saveData();
    if (stack->currentIndex() == 2) {
        showNewsFeed();
    } else if (stack->currentIndex() == 3) {
        showProfile();
    } else if (stack->currentIndex() == 6) {
        showAdminDashboard();
    }
}

void MainWindow::onAdminCreateNews() {
    if (m_loggedInIndex == -1) return;
    User *admin = users[m_loggedInIndex];
    if (admin->role != "admin" && admin->role != "Admin") return;

    QDialog dlg(this);
    dlg.setWindowTitle("Post News");
    dlg.resize(460, 320);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QLabel *title = new QLabel("Create announcement for all users", &dlg);
    QTextEdit *contentEdit = new QTextEdit(&dlg);
    contentEdit->setPlaceholderText("Write news text...");
    contentEdit->setAcceptRichText(false);
    contentEdit->setFixedHeight(150);

    QLabel *imagePathLabel = new QLabel("No image selected", &dlg);
    imagePathLabel->setWordWrap(true);
    QString selectedImagePath;

    QHBoxLayout *imageButtons = new QHBoxLayout();
    QPushButton *selectImageBtn = new QPushButton("Select Image", &dlg);
    QPushButton *clearImageBtn = new QPushButton("Clear", &dlg);
    imageButtons->addWidget(selectImageBtn);
    imageButtons->addWidget(clearImageBtn);
    imageButtons->addStretch();

    connect(selectImageBtn, &QPushButton::clicked, &dlg, [&]() {
        const QString path = QFileDialog::getOpenFileName(
            this,
            "Select News Image",
            "",
            "Images (*.png *.jpg *.jpeg *.gif *.bmp *.webp)"
        );
        if (path.isEmpty()) return;
        selectedImagePath = path;
        imagePathLabel->setText(path);
    });
    connect(clearImageBtn, &QPushButton::clicked, &dlg, [&]() {
        selectedImagePath.clear();
        imagePathLabel->setText("No image selected");
    });

    QDialogButtonBox *buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    layout->addWidget(title);
    layout->addWidget(contentEdit);
    layout->addLayout(imageButtons);
    layout->addWidget(imagePathLabel);
    layout->addWidget(buttons);

    if (dlg.exec() != QDialog::Accepted) return;

    const QString content = contentEdit->toPlainText().trimmed();
    if (content.isEmpty() && selectedImagePath.isEmpty()) {
        QMessageBox::warning(this, "Invalid News", "Add text or an image before posting.");
        return;
    }

    Post *p = new Post(nextID++, content.toStdString(), selectedImagePath.toStdString());
    admin->createPost(p);

    const std::string preview = content.left(40).toStdString();
    const std::string summary = preview + (content.length() > 40 ? "..." : "");
    for (int i = 0; i < userCount; i++) {
        if (users[i]->userID == admin->userID) continue;
        notifSystem.addNotification(
            users[i]->userID,
            "Admin posted news" + (summary.empty() ? std::string(".") : (": \"" + summary + "\""))
        );
    }

    saveData();
    QMessageBox::information(this, "Posted", "News posted to all users.");
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
    std::unordered_set<int> seenOwners;

    if (m_feedViewMode == FeedViewMode::Home) {
        // Home: only my posts.
        for (int i = 0; i < me->postCount; i++) {
            items.append({me->posts[i], me});
        }
    } else {
        // Feed: posts from all friends and followers.
        for (int i = 0; i < me->friendCount; i++) {
            User *u = me->friends[i];
            if (!u || seenOwners.count(u->userID)) continue;
            seenOwners.insert(u->userID);
            for (int j = 0; j < u->postCount; j++) {
                items.append({u->posts[j], u});
            }
        }
        for (int i = 0; i < me->followerCount; i++) {
            User *u = me->follower[i];
            if (!u || seenOwners.count(u->userID)) continue;
            seenOwners.insert(u->userID);
            for (int j = 0; j < u->postCount; j++) {
                items.append({u->posts[j], u});
            }
        }
        // Include admin news posts for everyone.
        for (int i = 0; i < userCount; i++) {
            User *u = users[i];
            if (!u) continue;
            if (u->role != "admin" && u->role != "Admin") continue;
            if (seenOwners.count(u->userID)) continue;
            seenOwners.insert(u->userID);
            for (int j = 0; j < u->postCount; j++) {
                items.append({u->posts[j], u});
            }
        }
    }

    // Sort latest first
    std::sort(items.begin(), items.end(), [](const FeedItem &a, const FeedItem &b) {
        return a.post->timestamp > b.post->timestamp;
    });

    for (auto &item : items) {
        QString ownerName = QString::fromStdString(item.owner->userName);
        QString content   = QString::fromStdString(item.post->content);
        QString imagePath = QString::fromStdString(item.post->imagePath);

        // Time ago
        time_t now  = time(nullptr);
        double diff = difftime(now, item.post->timestamp);
        QString timeAgo;
        if      (diff < 60)    timeAgo = "just now";
        else if (diff < 3600)  timeAgo = QString("%1m ago").arg((int)(diff/60));
        else if (diff < 86400) timeAgo = QString("%1h ago").arg((int)(diff/3600));
        else                   timeAgo = QString("%1d ago").arg((int)(diff/86400));

        bool canInteract = true;

        QStringList commentsList;
        for (int i = 0; i < item.post->commentCount; i++) {
            commentsList.append(QString::fromStdString(item.post->comments[i]));
        }

        feedPage->addPost(
            item.post->postID, ownerName, QString::fromStdString(item.owner->profileImagePath), content,
            imagePath,
            timeAgo,
            item.post->likeCount,
            item.post->commentCount,
            canInteract,
            item.post->hasLiked(me->userID),
            commentsList,
            (item.owner == me)
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
    feedPage->loadCurrentUser(
        users[m_loggedInIndex]->userID,
        name,
        QString::fromStdString(users[m_loggedInIndex]->profileImagePath)
    );
    feedPage->setNotifBadge(notifSystem.countUnseen(users[m_loggedInIndex]->userID));
    loadFeed();
    stack->setCurrentIndex(2);
}

void MainWindow::showProfile() {
    if (m_loggedInIndex == -1) return;
    User *me = users[m_loggedInIndex];
    const bool viewerIsAdmin = (me->role == "admin" || me->role == "Admin");

    User *u = me;
    bool isOwn = true;
    if (m_viewedUserID != -1 && m_viewedUserID != me->userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == m_viewedUserID) {
                u = users[i];
                isOwn = false;
                break;
            }
        }
    }

    profilePage->clearPosts();
    profilePage->clearFriends();
    profilePage->loadProfile(
        u->userID,
        QString::fromStdString(u->userName),
        QString::fromStdString(u->userName),
        QString::fromStdString(u->profileImagePath),
        "", "", "2025",
        QString::fromStdString(u->birthDate),
        QString::fromStdString(u->githubUsername),
        QString::fromStdString(viewerIsAdmin ? me->role : u->role),
        u->postCount, u->friendCount,
        u->followerCount, u->followingCount,
        isOwn
    );
    // Load user's posts into profile
    for (int i = 0; i < u->postCount; i++) {
        QStringList commentsList;
        for (int c = 0; c < u->posts[i]->commentCount; c++) {
            commentsList.append(QString::fromStdString(u->posts[i]->comments[c]));
        }

        const QString postContent = QString::fromStdString(u->posts[i]->content);
        const QString postImagePath = QString::fromStdString(u->posts[i]->imagePath);
        if (postImagePath.isEmpty()) {
            profilePage->addTextPost(
                u->posts[i]->postID,
                QString::fromStdString(u->userName),
                QString::fromStdString(u->userName),
                postContent,
                "recently",
                u->posts[i]->likeCount,
                u->posts[i]->commentCount,
                u->posts[i]->hasLiked(users[m_loggedInIndex]->userID),
                commentsList,
                isOwn
            );
        } else {
            profilePage->addImagePost(
                u->posts[i]->postID,
                QString::fromStdString(u->userName),
                QString::fromStdString(u->userName),
                postContent,
                postImagePath,
                "recently",
                u->posts[i]->likeCount,
                u->posts[i]->commentCount,
                u->posts[i]->hasLiked(users[m_loggedInIndex]->userID),
                commentsList,
                isOwn
            );
        }
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
        QString::fromStdString(users[m_loggedInIndex]->userName),
        QString::fromStdString(users[m_loggedInIndex]->profileImagePath)
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
            "",
            QString::fromStdString(users[i]->profileImagePath),
            isFriend
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
    
    messagePage->clearContacts();
    
    // Load friends as contacts
    User *me = users[m_loggedInIndex];
    for (int i = 0; i < me->friendCount; i++) {
        messagePage->addContact(
            me->friends[i]->userID,
            QString::fromStdString(me->friends[i]->userName),
            QString::fromStdString(me->friends[i]->profileImagePath),
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
    int bannedUsers = 0;
    int postsToReview = 0;
    for (int i = 0; i < userCount; i++)
        totalPosts += users[i]->postCount;
    for (int i = 0; i < userCount; i++)
        if (users[i]->isBanned) bannedUsers++;
    postsToReview = totalPosts;

    adminPage->setStats(userCount, totalPosts, bannedUsers, postsToReview);

    for (int i = 0; i < userCount; i++) {
        adminPage->addUserRow(
            users[i]->userID,
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->profileImagePath),
            QString::fromStdString(users[i]->role),
            users[i]->postCount,
            users[i]->isBanned
        );
    }
    adminPage->refreshUserSearch();

    // Populate moderation post cards.
    for (int i = 0; i < userCount; i++) {
        User* owner = users[i];
        if (owner->role == "admin" || owner->role == "Admin") continue;
        for (int j = 0; j < owner->postCount; j++) {
            Post* p = owner->posts[j];
            adminPage->addModPost(
                p->postID,
                owner->userID,
                QString::fromStdString(owner->userName),
                QString::fromStdString(p->content),
                QString::fromStdString(p->imagePath),
                p->likeCount,
                p->commentCount
            );
        }
    }
    stack->setCurrentIndex(6);
}

// ═══════════════════════════════════════════════════════
//  SHOW NOTIFICATIONS
// ═══════════════════════════════════════════════════════
void MainWindow::showNotifications() {
    if (m_loggedInIndex == -1) return;
    const int currentUserID = users[m_loggedInIndex]->userID;
    notifPage->loadNotifications(currentUserID);
    notifSystem.markAllSeen(currentUserID);
    feedPage->setNotifBadge(0);
    saveData();
    stack->setCurrentIndex(7);
}
