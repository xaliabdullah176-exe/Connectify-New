#include "mainwindow.h"
#include <QDateEdit>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <unordered_set>
#include <vector>
#include <map>

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
    connect(profilePage, &ProfilePage::reportUserClicked, this, [this](int reportedUserID) {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];
        if (me->userID == reportedUserID) return;

        User* target = nullptr;
        for (int i = 0; i < userCount; i++)
            if (users[i]->userID == reportedUserID) { target = users[i]; break; }
        if (!target) return;
        if (target->role == "admin" || target->role == "Admin") {
            QMessageBox::information(this, QStringLiteral("Report"),
                QStringLiteral("Admin accounts cannot be reported."));
            return;
        }

        if (!modSystem.addReport(me, reportedUserID)) {
            QMessageBox::information(this, QStringLiteral("Report"),
                QStringLiteral("You have already reported this user."));
            return;
        }

        const int cnt = modSystem.reportCountAgainst(reportedUserID);
        const int adminId = getFirstAdminUserID();
        if (adminId >= 0) {
            std::string line = "[REPORT] " + std::to_string(cnt) + " report(s) against @"
                + target->userName + ". Latest reporter: @" + me->userName + ".";
            notifSystem.addNotification(adminId, line);
        }
        saveData();
        QMessageBox::information(this, QStringLiteral("Report"),
            QStringLiteral("Thanks — your report was sent to the moderators. The other person will not be notified."));
    });
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

    QLineEdit *emailEdit = new QLineEdit(QString::fromStdString(me->email), &dlg);
    emailEdit->setPlaceholderText("you@example.com");

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
    form->addRow("Email", emailEdit);
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

    const QString newEmail = emailEdit->text().trimmed();
    const std::string newEmailStd = newEmail.toStdString();
    if (!newEmail.isEmpty()) {
        if (!newEmail.contains(QLatin1Char('@'))) {
            QMessageBox::warning(this, "Email", "Please enter a valid email (must include @).");
            return;
        }
        if (emailTaken(newEmailStd, me->userID)) {
            QMessageBox::warning(this, "Email", "This email is already used by another account.");
            return;
        }
    }

    me->userName = newName.toStdString();
    me->email = newEmailStd;
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
        User *me = users[m_loggedInIndex];
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID != userID) continue;
            User *them = users[i];
            if (!me->sendRequest(them))
                return;
            notifSystem.addFriendRequestNotification(
                them->userID,
                me->userID,
                me->userName + " sent you a friend request."
            );
            saveData();
            showSearch();
            break;
        }
    });
    connect(searchPage, &SearchPage::backClicked, this, &MainWindow::showNewsFeed);

    // ── MessagePage (Arslan) ──
    connect(messagePage, &MessagePage::contactSelected, this, [this](int contactUserID) {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];

        if (contactUserID < 0) {
            const int groupID = -contactUserID;
            Group* g = groupSystem.findGroup(groupID);
            if (!g || !GroupSystem::isMember(g, me->userID)) return;

            messagePage->clearMessages();
            messagePage->openGroupConversation(
                groupID,
                QString::fromStdString(g->name),
                QString::fromStdString(g->iconPath),
                static_cast<int>(g->memberIDs.size()),
                me->userID == g->creatorID
            );

            for (int i = 0; i < msgSystem.msgCount; i++) {
                Message* m = msgSystem.msg[i];
                if (m->receiverID != -groupID) continue;
                const bool isSentByMe = (m->senderID == me->userID);
                QString timeStr;
                if (m->timestamp > 0) {
                    QDateTime dt = QDateTime::fromSecsSinceEpoch(m->timestamp);
                    timeStr = dt.toString("hh:mm AP");
                }
                messagePage->addMessage(
                    QString::fromUtf8(m->text.data(), static_cast<int>(m->text.size())),
                    timeStr,
                    isSentByMe);
            }
            return;
        }

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

                messagePage->addMessage(
                    QString::fromUtf8(m->text.data(), static_cast<int>(m->text.size())),
                    timeStr,
                    isSentByMe);
            }
        }
    });

    connect(messagePage, &MessagePage::groupMessageSendRequested,
            this, [this](int groupID, const QString &text) {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];
        const QByteArray utf8 = text.toUtf8();
        const std::string utf8msg(utf8.constData(), static_cast<size_t>(utf8.size()));
        if (!msgSystem.sendGroupMessage(me, groupID, utf8msg))
            return;
        Group* g = groupSystem.findGroup(groupID);
        if (g) {
            std::string snippet = utf8msg;
            if (snippet.size() > 48) snippet = snippet.substr(0, 45) + "...";
            const std::string prefix = me->userName + " in group \"" + g->name + "\": ";
            for (int uid : g->memberIDs)
                notifSystem.addNotification(uid, prefix + snippet);
        }
        saveData();
    });
    connect(messagePage, &MessagePage::dmMessageSendRequested,
            this, [this](int toUserID, const QString &text) {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID != toUserID) continue;
            const QByteArray dmUtf8 = text.toUtf8();
            const std::string dmStr(dmUtf8.constData(), static_cast<size_t>(dmUtf8.size()));
            const bool sent = msgSystem.sendMessage(me, users[i], dmStr);
            if (sent) {
                std::string prev = dmStr.size() > 40 ? dmStr.substr(0, 40) + "..." : dmStr;
                notifSystem.addNotification(
                    toUserID,
                    me->userName + " sent you a message: \"" + prev + "\""
                );
                saveData();
            }
            break;
        }
    });

    connect(messagePage, &MessagePage::createGroupClicked, this, [this]() {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];

        QDialog dlg(this);
        dlg.setWindowTitle("Create group");
        dlg.resize(440, 520);

        auto *mainLay = new QVBoxLayout(&dlg);
        auto *nameEdit = new QLineEdit(&dlg);
        nameEdit->setPlaceholderText("Group name");

        auto *iconLbl = new QLabel(QStringLiteral("No icon selected"), &dlg);
        iconLbl->setWordWrap(true);
        QString iconPath;
        auto *pickIcon = new QPushButton(QStringLiteral("Choose group icon…"), &dlg);
        QObject::connect(pickIcon, &QPushButton::clicked, &dlg, [&]() {
            const QString p = QFileDialog::getOpenFileName(
                this,
                QStringLiteral("Group icon"),
                QString(),
                QStringLiteral("Images (*.png *.jpg *.jpeg *.webp *.bmp *.gif)")
            );
            if (!p.isEmpty()) {
                iconPath = QFileInfo(p).absoluteFilePath();
                iconLbl->setText(iconPath);
            }
        });

        mainLay->addWidget(new QLabel(QStringLiteral("Name"), &dlg));
        mainLay->addWidget(nameEdit);
        mainLay->addWidget(pickIcon);
        mainLay->addWidget(iconLbl);
        mainLay->addWidget(new QLabel(QStringLiteral("Add friends (your friends only):"), &dlg));

        auto *scroll = new QScrollArea(&dlg);
        scroll->setWidgetResizable(true);
        auto *inner = new QWidget;
        auto *innerLay = new QVBoxLayout(inner);
        std::vector<std::pair<QCheckBox*, int>> boxes;
        for (int fi = 0; fi < me->friendCount; fi++) {
            const int fid = me->friends[fi]->userID;
            auto *cb = new QCheckBox(
                QString::fromStdString(me->friends[fi]->userName),
                inner
            );
            innerLay->addWidget(cb);
            boxes.push_back({cb, fid});
        }
        innerLay->addStretch();
        scroll->setWidget(inner);
        mainLay->addWidget(scroll, 1);

        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        mainLay->addWidget(bb);
        QObject::connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() != QDialog::Accepted) return;

        const QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Group"), QStringLiteral("Enter a group name."));
            return;
        }
        std::vector<int> selected;
        for (const auto& pr : boxes) {
            if (pr.first->isChecked())
                selected.push_back(pr.second);
        }
        Group* g = groupSystem.createGroup(
            me,
            name.toStdString(),
            iconPath.toStdString(),
            selected
        );
        if (!g) {
            QMessageBox::warning(
                this,
                QStringLiteral("Group"),
                QStringLiteral("Select at least one friend to create a group.")
            );
            return;
        }
        for (int uid : g->memberIDs) {
            if (uid == me->userID) continue;
            notifSystem.addNotification(
                uid,
                me->userName + " added you to the group \"" + g->name + "\"."
            );
        }
        saveData();
        showMessages();
    });

    connect(messagePage, &MessagePage::renameGroupClicked, this, [this](int groupID) {
        if (m_loggedInIndex == -1) return;
        bool ok = false;
        const QString newName = QInputDialog::getText(
            this,
            QStringLiteral("Rename group"),
            QStringLiteral("New group name:"),
            QLineEdit::Normal,
            QString(),
            &ok
        );
        if (!ok || newName.trimmed().isEmpty()) return;
        if (!groupSystem.setGroupName(groupID, users[m_loggedInIndex], newName.trimmed().toStdString()))
            return;
        saveData();
        Group* g = groupSystem.findGroup(groupID);
        User* meRen = users[m_loggedInIndex];
        if (g)
            messagePage->openGroupConversation(
                groupID,
                QString::fromStdString(g->name),
                QString::fromStdString(g->iconPath),
                static_cast<int>(g->memberIDs.size()),
                meRen && meRen->userID == g->creatorID
            );
        showMessages();
    });

    connect(messagePage, &MessagePage::addMembersToGroupClicked, this, [this](int groupID) {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];
        Group* g = groupSystem.findGroup(groupID);
        if (!g || !GroupSystem::isMember(g, me->userID)) return;

        QDialog dlg(this);
        dlg.setWindowTitle(QStringLiteral("Add members"));
        dlg.resize(400, 420);
        auto *mainLay = new QVBoxLayout(&dlg);
        mainLay->addWidget(new QLabel(
            QStringLiteral("Select friends to add (not already in this group):"), &dlg));

        auto *scroll = new QScrollArea(&dlg);
        scroll->setWidgetResizable(true);
        auto *inner = new QWidget;
        auto *innerLay = new QVBoxLayout(inner);
        std::vector<std::pair<QCheckBox*, int>> boxes;
        int available = 0;
        for (int fi = 0; fi < me->friendCount; fi++) {
            const int fid = me->friends[fi]->userID;
            if (GroupSystem::isMember(g, fid)) continue;
            auto *cb = new QCheckBox(
                QString::fromStdString(me->friends[fi]->userName),
                inner
            );
            innerLay->addWidget(cb);
            boxes.push_back({cb, fid});
            available++;
        }
        innerLay->addStretch();
        scroll->setWidget(inner);
        mainLay->addWidget(scroll, 1);

        if (available == 0) {
            QMessageBox::information(
                this,
                QStringLiteral("Add members"),
                QStringLiteral("No friends left to add, or everyone is already in this group.")
            );
            return;
        }

        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        mainLay->addWidget(bb);
        QObject::connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() != QDialog::Accepted) return;

        std::vector<int> selected;
        for (const auto& pr : boxes) {
            if (pr.first->isChecked())
                selected.push_back(pr.second);
        }
        if (selected.empty()) return;

        if (!groupSystem.addMembersToGroup(groupID, me, selected)) return;

        g = groupSystem.findGroup(groupID);
        for (int fid : selected) {
            notifSystem.addNotification(
                fid,
                me->userName + " added you to the group \"" + g->name + "\"."
            );
        }
        saveData();
        if (g)
            messagePage->openGroupConversation(
                groupID,
                QString::fromStdString(g->name),
                QString::fromStdString(g->iconPath),
                static_cast<int>(g->memberIDs.size()),
                me->userID == g->creatorID
            );
        showMessages();
    });

    connect(messagePage, &MessagePage::manageGroupMembersClicked, this, [this](int groupID) {
        if (m_loggedInIndex == -1) return;
        User* me = users[m_loggedInIndex];
        Group* g = groupSystem.findGroup(groupID);
        if (!g || me->userID != g->creatorID) return;

        QDialog dlg(this);
        dlg.setWindowTitle(QStringLiteral("Manage members"));
        dlg.resize(380, 360);
        auto *vl = new QVBoxLayout(&dlg);
        vl->addWidget(new QLabel(QStringLiteral("Remove members from this group:"), &dlg));

        auto *scroll = new QScrollArea(&dlg);
        scroll->setWidgetResizable(true);
        auto *inner = new QWidget;
        auto *innerLay = new QVBoxLayout(inner);

        for (int mid : g->memberIDs) {
            if (mid == me->userID) continue;
            User* mu = nullptr;
            for (int i = 0; i < userCount; i++)
                if (users[i]->userID == mid) { mu = users[i]; break; }
            if (!mu) continue;

            auto *row = new QHBoxLayout();
            row->addWidget(new QLabel(QString::fromStdString(mu->userName), inner));
            row->addStretch();
            auto *rem = new QPushButton(QStringLiteral("Remove"), inner);
            rem->setCursor(Qt::PointingHandCursor);
            connect(rem, &QPushButton::clicked, &dlg, [this, groupID, me, mid, &dlg]() {
                if (!groupSystem.removeMember(groupID, me, mid)) return;
                Group* gg = groupSystem.findGroup(groupID);
                if (gg) {
                    notifSystem.addNotification(
                        mid,
                        "You were removed from the group \"" + gg->name + "\" by the group admin."
                    );
                }
                saveData();
                dlg.accept();
                showMessages();
            });
            row->addWidget(rem);
            innerLay->addLayout(row);
        }
        innerLay->addStretch();
        scroll->setWidget(inner);
        vl->addWidget(scroll, 1);

        auto *closeBtn = new QPushButton(QStringLiteral("Close"), &dlg);
        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        vl->addWidget(closeBtn);

        dlg.exec();
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
    connect(adminPage, &AdminDashboard::moderationBanFromReport, this, [this](int reportedUserID) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID != reportedUserID) continue;
            if (users[i]->role == "admin" || users[i]->role == "Admin") return;
            const QString targetName = QString::fromStdString(users[i]->userName);
            if (QMessageBox::question(
                    this,
                    QStringLiteral("Ban from report"),
                    QStringLiteral("Ban \"%1\" and clear these reports?").arg(targetName),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) != QMessageBox::Yes)
                return;
            users[i]->isBanned = true;
            modSystem.clearReportsAgainst(reportedUserID);
            notifSystem.addNotification(
                users[i]->userID,
                "Your account has been banned by admin."
            );
            saveData();
            showAdminDashboard();
            return;
        }
    });
    connect(adminPage, &AdminDashboard::moderationIgnoreReports, this, [this](int reportedUserID) {
        modSystem.clearReportsAgainst(reportedUserID);
        saveData();
        showAdminDashboard();
    });
    connect(adminPage, &AdminDashboard::moderationUnbanFromAppeal, this, [this](int userID) {
        if (m_loggedInIndex == -1) return;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID != userID) continue;
            if (users[i]->role == "admin" || users[i]->role == "Admin") return;
            users[i]->isBanned = false;
            modSystem.removeAppealsFor(userID);
            notifSystem.addNotification(
                userID,
                "Your ban appeal was accepted. Your account is active again."
            );
            saveData();
            showAdminDashboard();
            return;
        }
    });
    connect(adminPage, &AdminDashboard::moderationKeepBannedFromAppeal, this, [this](int userID) {
        modSystem.removeAppealsFor(userID);
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID && users[i]->isBanned) {
                notifSystem.addNotification(
                    userID,
                    "Your ban appeal was reviewed. Your account remains banned."
                );
                break;
            }
        }
        saveData();
        showAdminDashboard();
    });
    connect(adminPage, &AdminDashboard::navDashboard, this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::navUsers,     this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::navPosts,     this, &MainWindow::showAdminDashboard);
    connect(adminPage, &AdminDashboard::viewUserClicked, this, [this](int userID) {
        m_viewedUserID = userID;
        showProfile();
    });
    connect(adminPage, &AdminDashboard::userGroupsClicked, this, [this](int userID) {
        QStringList blocks;
        for (int gi = 0; gi < groupSystem.groupCount; gi++) {
            Group* g = groupSystem.groups[gi];
            bool member = false;
            for (int id : g->memberIDs)
                if (id == userID) { member = true; break; }
            if (!member) continue;

            QStringList memberNames;
            for (int mid : g->memberIDs) {
                User* u = nullptr;
                for (int j = 0; j < userCount; j++)
                    if (users[j]->userID == mid) { u = users[j]; break; }
                memberNames.append(
                    u ? QString::fromStdString(u->userName) : QString::number(mid)
                );
            }
            blocks.append(
                QStringLiteral("• %1 (group #%2)\n  With: %3")
                    .arg(QString::fromStdString(g->name))
                    .arg(g->groupID)
                    .arg(memberNames.join(QStringLiteral(", ")))
            );
        }
        if (blocks.isEmpty()) {
            QMessageBox::information(
                this,
                QStringLiteral("Groups"),
                QStringLiteral("This user is not in any group.")
            );
            return;
        }
        QMessageBox::information(
            this,
            QStringLiteral("User groups"),
            blocks.join(QStringLiteral("\n\n"))
        );
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
    connect(notifPage, &NotificationPage::friendRequestAccepted, this, [this](int fromUserId) {
        if (m_loggedInIndex == -1) return;
        User *me = users[m_loggedInIndex];
        User *from = nullptr;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == fromUserId) {
                from = users[i];
                break;
            }
        }
        if (!from) return;

        bool pending = false;
        for (int i = 0; i < me->requestCount; i++) {
            if (me->request[i] == from) {
                pending = true;
                break;
            }
        }

        notifSystem.removeFriendRequestNotifications(me->userID, fromUserId);
        if (pending) {
            me->acceptRequest(from);
            notifSystem.addNotification(
                fromUserId,
                me->userName + " accepted your friend request."
            );
        }
        saveData();
        notifPage->loadNotifications(me->userID);
        feedPage->setNotifBadge(notifSystem.countUnseen(me->userID));
    });
    connect(notifPage, &NotificationPage::friendRequestDeclined, this, [this](int fromUserId) {
        if (m_loggedInIndex == -1) return;
        User *me = users[m_loggedInIndex];
        User *from = nullptr;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == fromUserId) {
                from = users[i];
                break;
            }
        }
        if (from)
            me->rejectRequest(from);
        notifSystem.removeFriendRequestNotifications(me->userID, fromUserId);
        saveData();
        notifPage->loadNotifications(me->userID);
        feedPage->setNotifBadge(notifSystem.countUnseen(me->userID));
    });
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
        User* bu = users[index];
        QDialog banDlg(this);
        banDlg.setWindowTitle(QStringLiteral("Account banned"));
        banDlg.resize(420, 280);
        auto *v = new QVBoxLayout(&banDlg);
        v->addWidget(new QLabel(
            QStringLiteral("Your account has been banned by an administrator.\n"
                           "You can submit a short appeal below. The other party is not involved."),
            &banDlg));
        auto *te = new QTextEdit(&banDlg);
        te->setPlaceholderText(QStringLiteral("Write 1–2 lines for the admin…"));
        te->setMaximumHeight(90);
        v->addWidget(te);

        auto *submit = new QPushButton(QStringLiteral("Submit appeal"), &banDlg);
        submit->setObjectName("appealGreenBtn");
        submit->setCursor(Qt::PointingHandCursor);
        submit->setStyleSheet(
            QStringLiteral("QPushButton#appealGreenBtn { background:#15803d; color:white; "
                           "border:none; border-radius:8px; padding:10px 16px; font-weight:700; }"
                           "QPushButton#appealGreenBtn:hover { background:#16a34a; }"));
        v->addWidget(submit);

        bool sent = false;
        connect(submit, &QPushButton::clicked, &banDlg, [&]() {
            const QString t = te->toPlainText().trimmed();
            if (t.isEmpty()) {
                QMessageBox::warning(&banDlg, QStringLiteral("Appeal"),
                    QStringLiteral("Please write a short message before submitting."));
                return;
            }
            if (modSystem.hasPendingAppeal(bu->userID)) {
                QMessageBox::information(&banDlg, QStringLiteral("Appeal"),
                    QStringLiteral("You already have a pending appeal. Please wait for admin review."));
                return;
            }
            const QByteArray apUtf8 = t.toUtf8();
            modSystem.addAppeal(bu->userID, std::string(apUtf8.constData(), apUtf8.size()));
            const int adminId = getFirstAdminUserID();
            if (adminId >= 0) {
                std::string prev = apUtf8.size() > 200
                    ? std::string(apUtf8.constData(), 200) + "..."
                    : std::string(apUtf8.constData(), apUtf8.size());
                notifSystem.addNotification(
                    adminId,
                    "[BAN APPEAL] @" + bu->userName + ": " + prev
                );
            }
            saveData();
            sent = true;
            banDlg.accept();
        });

        banDlg.exec();
        loginPage->showError(sent
            ? QStringLiteral("Appeal sent. An admin will review it. You remain banned until then.")
            : QStringLiteral("Your account is banned by admin."));
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
    (void)fullName;

    if (userNameExist(username.toStdString())) {
        signupPage->showError("Username already taken. Choose another.");
        return;
    }
    const std::string em = email.trimmed().toStdString();
    if (!em.empty() && emailTaken(em, -1)) {
        signupPage->showError("That email is already registered.");
        return;
    }

    signup(nextID++, username.toStdString(), password.toStdString(), "user", em);

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
    messagePage->resetForNewSession();
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
        QString::fromStdString(users[m_loggedInIndex]->profileImagePath),
        QString::fromStdString(users[m_loggedInIndex]->email)
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
        QString::fromStdString(u->email),
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
        QString::fromStdString(users[m_loggedInIndex]->profileImagePath),
        QString::fromStdString(users[m_loggedInIndex]->email)
    );
    searchPage->clearResults();
    for (int i = 0; i < userCount; i++) {
        if (users[i]->userID == users[m_loggedInIndex]->userID) continue;
        bool isFriend = false;
        for (int j = 0; j < users[m_loggedInIndex]->friendCount; j++)
            if (users[m_loggedInIndex]->friends[j] == users[i])
                { isFriend = true; break; }
        bool requestSent = false;
        for (int j = 0; j < users[i]->requestCount; j++)
            if (users[i]->request[j] == users[m_loggedInIndex])
                { requestSent = true; break; }
        searchPage->addUser(
            users[i]->userID,
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->userName),
            QString::fromStdString(users[i]->email),
            "",
            QString::fromStdString(users[i]->profileImagePath),
            isFriend,
            requestSent
        );
    }
    stack->setCurrentIndex(4);
}

void MainWindow::showMessages() {
    if (m_loggedInIndex == -1) return;
    messagePage->loadCurrentUser(
        users[m_loggedInIndex]->userID,
        QString::fromStdString(users[m_loggedInIndex]->userName),
        QString::fromStdString(users[m_loggedInIndex]->email)
    );
    
    messagePage->clearContacts();
    
    User *me = users[m_loggedInIndex];

    auto previewDirect = [&](int otherUserId) -> QString {
        Message* last = nullptr;
        for (int mi = 0; mi < msgSystem.msgCount; mi++) {
            Message* m = msgSystem.msg[mi];
            if (m->receiverID < 0) continue;
            if ((m->senderID == me->userID && m->receiverID == otherUserId) ||
                (m->senderID == otherUserId && m->receiverID == me->userID)) {
                if (!last || m->timestamp > last->timestamp) last = m;
            }
        }
        if (!last) return QStringLiteral("Click to chat");
        QString t = QString::fromStdString(last->text);
        return t.size() > 45 ? t.left(42) + QStringLiteral("…") : t;
    };

    auto previewGroup = [&](int gid) -> QString {
        Message* last = nullptr;
        for (int mi = 0; mi < msgSystem.msgCount; mi++) {
            Message* m = msgSystem.msg[mi];
            if (m->receiverID != -gid) continue;
            if (!last || m->timestamp > last->timestamp) last = m;
        }
        if (!last) return QStringLiteral("Group chat");
        QString t = QString::fromStdString(last->text);
        return t.size() > 45 ? t.left(42) + QStringLiteral("…") : t;
    };

    for (int gi = 0; gi < groupSystem.groupCount; gi++) {
        Group* g = groupSystem.groups[gi];
        bool imIn = false;
        for (int id : g->memberIDs)
            if (id == me->userID) { imIn = true; break; }
        if (!imIn) continue;
        messagePage->addContact(
            -g->groupID,
            QString::fromStdString(g->name),
            QString::fromStdString(g->iconPath),
            previewGroup(g->groupID),
            QString(),
            false
        );
    }

    for (int i = 0; i < me->friendCount; i++) {
        messagePage->addContact(
            me->friends[i]->userID,
            QString::fromStdString(me->friends[i]->userName),
            QString::fromStdString(me->friends[i]->profileImagePath),
            previewDirect(me->friends[i]->userID),
            QString(),
            true
        );
    }

    messagePage->syncListSelectionWithOpenChat();
    stack->setCurrentIndex(5);
}

void MainWindow::showAdminDashboard() {
    if (m_loggedInIndex == -1) return;
    User *u = users[m_loggedInIndex];
    adminPage->clearUsers();
    adminPage->clearModPosts();
    adminPage->clearModerationLists();
    adminPage->loadAdmin(u->userID, QString::fromStdString(u->userName));

    auto userNameById = [](int uid) -> QString {
        for (int i = 0; i < userCount; i++)
            if (users[i]->userID == uid)
                return QString::fromStdString(users[i]->userName);
        return QString::number(uid);
    };

    std::map<int, std::vector<int>> reportsByVictim;
    for (const auto& r : modSystem.reports)
        reportsByVictim[r.reportedID].push_back(r.reporterID);

    for (const auto& pr : reportsByVictim) {
        const int rid = pr.first;
        const int cnt = static_cast<int>(pr.second.size());
        QStringList repNames;
        for (int repId : pr.second)
            repNames.append(userNameById(repId));
        const QString sum = QStringLiteral("@%1 — reported by: %2")
                                .arg(userNameById(rid))
                                .arg(repNames.join(QStringLiteral(", ")));
        adminPage->addReportRow(rid, cnt, sum);
    }

    for (const auto& ap : modSystem.pendingAppeals) {
        adminPage->addAppealRow(
            ap.userID,
            userNameById(ap.userID),
            QString::fromStdString(ap.message)
        );
    }

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
            QString::fromStdString(users[i]->email),
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
