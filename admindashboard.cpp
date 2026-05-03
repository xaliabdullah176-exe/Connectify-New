#include "admindashboard.h"
#include <QStyle>
// ═══════════════════════════════════════════════════════
//  CONSTRUCTOR
// ═══════════════════════════════════════════════════════
AdminDashboard::AdminDashboard(QWidget *parent) : QWidget(parent) {
    setupUI();
    applyStyles();
}

// ═══════════════════════════════════════════════════════
//  LOAD ADMIN
// ═══════════════════════════════════════════════════════
void AdminDashboard::loadAdmin(int adminID, QString adminName) {
    QStringList parts = adminName.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    adminAvatarLabel->setText(initials.left(2));
    adminNameLabel->setText(adminName);
}

// ═══════════════════════════════════════════════════════
//  SET STATS
// ═══════════════════════════════════════════════════════
void AdminDashboard::setStats(int totalUsers, int totalPosts,
                              int bannedUsers, int postsToReview)
{
    totalUsersNum->setText(QString::number(totalUsers));
    totalPostsNum->setText(QString::number(totalPosts));
    bannedUsersNum->setText(QString::number(bannedUsers));
    reviewPostsNum->setText(QString::number(postsToReview));
}

// ═══════════════════════════════════════════════════════
//  ADD USER ROW
// ═══════════════════════════════════════════════════════
void AdminDashboard::addUserRow(int userID, QString name, QString role,
                                int postCount, bool isBanned)
{
    // Insert before stretch at end
    userTableLayout->insertWidget(
        userTableLayout->count() - 1,
        makeUserRow(userID, name, role, postCount, isBanned)
        );
}

// ═══════════════════════════════════════════════════════
//  ADD MOD POST
// ═══════════════════════════════════════════════════════
void AdminDashboard::addModPost(int postID, int ownerID, QString ownerName,
                                QString content, int likes, int comments)
{
    int row = modPostCount / 2;
    int col = modPostCount % 2;
    modPostsLayout->addWidget(
        makeModPostCard(postID, ownerID, ownerName, content, likes, comments),
        row, col
        );
    modPostCount++;
}

// ═══════════════════════════════════════════════════════
//  CLEAR
// ═══════════════════════════════════════════════════════
void AdminDashboard::clearUsers() {
    QLayoutItem *item;
    while ((item = userTableLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    userTableLayout->addStretch();
}

void AdminDashboard::clearModPosts() {
    QLayoutItem *item;
    while ((item = modPostsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    modPostCount = 0;
}

// ═══════════════════════════════════════════════════════
//  SET ACTIVE SIDEBAR BUTTON
// ═══════════════════════════════════════════════════════
void AdminDashboard::setActiveSidebarBtn(QPushButton *active) {
    for (auto btn : {navDashBtn, navUsersBtn, navPostsBtn}) {
        btn->setProperty("active", btn == active);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}

// ═══════════════════════════════════════════════════════
//  MAKE STAT CARD
// ═══════════════════════════════════════════════════════
QFrame* AdminDashboard::makeStatCard(QLabel *&numLabel, QString icon,
                                     QString labelText, QString changeText,
                                     bool isWarning)
{
    QFrame *card = new QFrame();
    card->setObjectName("statCard");

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(14, 14, 14, 14);
    vl->setSpacing(4);

    QLabel *iconLbl = new QLabel(icon);
    iconLbl->setObjectName("statIcon");
    vl->addWidget(iconLbl);

    numLabel = new QLabel("0");
    numLabel->setObjectName("statNum");
    vl->addWidget(numLabel);

    QLabel *lbl = new QLabel(labelText);
    lbl->setObjectName("statLbl");
    vl->addWidget(lbl);

    QLabel *change = new QLabel(changeText);
    change->setObjectName(isWarning ? "statChangeRed" : "statChangeGreen");
    vl->addWidget(change);

    return card;
}

// ═══════════════════════════════════════════════════════
//  MAKE USER ROW
// ═══════════════════════════════════════════════════════
QFrame* AdminDashboard::makeUserRow(int userID, QString name,
                                    QString role, int postCount,
                                    bool isBanned)
{
    QFrame *row = new QFrame();
    row->setObjectName("tableRow");

    QHBoxLayout *hl = new QHBoxLayout(row);
    hl->setContentsMargins(14, 10, 14, 10);
    hl->setSpacing(0);

    // ── Avatar + name + ID (flex: 2) ─────────────────────
    QWidget *userCell = new QWidget();
    QHBoxLayout *uc = new QHBoxLayout(userCell);
    uc->setContentsMargins(0,0,0,0);
    uc->setSpacing(8);

    QLabel *av = new QLabel();
    av->setObjectName("tableAvatar");
    av->setFixedSize(30, 30);
    av->setAlignment(Qt::AlignCenter);
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    av->setText(initials.left(2));

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(1);
    QLabel *uname = new QLabel(name);
    uname->setObjectName("tableUserName");
    QLabel *uid   = new QLabel("ID: " + QString::number(userID));
    uid->setObjectName("tableUserID");
    nameCol->addWidget(uname);
    nameCol->addWidget(uid);

    uc->addWidget(av);
    uc->addLayout(nameCol);
    userCell->setFixedWidth(180);
    hl->addWidget(userCell);

    // ── Role badge ────────────────────────────────────────
    QLabel *roleLbl = new QLabel(role);
    roleLbl->setObjectName(role == "admin" ? "badgeAdmin" : "badgeUser");
    roleLbl->setFixedWidth(80);
    hl->addWidget(roleLbl);

    // ── Post count ────────────────────────────────────────
    QLabel *posts = new QLabel(QString::number(postCount));
    posts->setObjectName("tableCell");
    posts->setFixedWidth(80);
    hl->addWidget(posts);

    // ── Status badge ──────────────────────────────────────
    QLabel *status = new QLabel(isBanned ? "Banned" : "Active");
    status->setObjectName(isBanned ? "badgeBanned" : "badgeActive");
    status->setFixedWidth(80);
    hl->addWidget(status);

    // ── Action buttons ────────────────────────────────────
    QHBoxLayout *actions = new QHBoxLayout();
    actions->setSpacing(6);

    QPushButton *viewBtn = new QPushButton("View");
    viewBtn->setObjectName("btnView");
    viewBtn->setFixedHeight(26);
    viewBtn->setCursor(Qt::PointingHandCursor);
    actions->addWidget(viewBtn);

    if (role != "admin") {
        if (!isBanned) {
            QPushButton *banBtn = new QPushButton("Ban");
            banBtn->setObjectName("btnBan");
            banBtn->setFixedHeight(26);
            banBtn->setCursor(Qt::PointingHandCursor);
            connect(banBtn, &QPushButton::clicked, this,
                    [this, userID]() { emit banUserClicked(userID); });
            actions->addWidget(banBtn);

            QPushButton *delBtn = new QPushButton("Delete");
            delBtn->setObjectName("btnDelete");
            delBtn->setFixedHeight(26);
            delBtn->setCursor(Qt::PointingHandCursor);
            connect(delBtn, &QPushButton::clicked, this,
                    [this, userID]() { emit deleteUserClicked(userID); });
            actions->addWidget(delBtn);
        } else {
            QPushButton *unbanBtn = new QPushButton("Unban");
            unbanBtn->setObjectName("btnUnban");
            unbanBtn->setFixedHeight(26);
            unbanBtn->setCursor(Qt::PointingHandCursor);
            connect(unbanBtn, &QPushButton::clicked, this,
                    [this, userID]() { emit unbanUserClicked(userID); });
            actions->addWidget(unbanBtn);
        }
    }

    connect(viewBtn, &QPushButton::clicked, this,
            [this, userID]() { emit viewUserClicked(userID); });

    actions->addStretch();
    QWidget *actWidget = new QWidget();
    actWidget->setLayout(actions);
    hl->addWidget(actWidget);

    return row;
}

// ═══════════════════════════════════════════════════════
//  MAKE MOD POST CARD
// ═══════════════════════════════════════════════════════
QFrame* AdminDashboard::makeModPostCard(int postID, int ownerID,
                                        QString ownerName, QString content,
                                        int likes, int comments)
{
    QFrame *card = new QFrame();
    card->setObjectName("modCard");

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(14, 12, 14, 12);
    vl->setSpacing(6);

    QLabel *user = new QLabel("@" + ownerName + " · Post #" +
                              QString::number(postID));
    user->setObjectName("modUser");
    vl->addWidget(user);

    QLabel *text = new QLabel(content);
    text->setObjectName("modContent");
    text->setWordWrap(true);
    vl->addWidget(text);

    QHBoxLayout *footer = new QHBoxLayout();
    footer->setSpacing(8);

    QPushButton *delBtn  = new QPushButton("🗑  Delete");
    delBtn->setObjectName("btnModDelete");
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setFixedHeight(28);

    QPushButton *keepBtn = new QPushButton("✓  Keep");
    keepBtn->setObjectName("btnModKeep");
    keepBtn->setCursor(Qt::PointingHandCursor);
    keepBtn->setFixedHeight(28);

    QLabel *meta = new QLabel("♥ " + QString::number(likes) +
                              "  💬 " + QString::number(comments));
    meta->setObjectName("modMeta");

    connect(delBtn, &QPushButton::clicked, this,
            [this, postID]() { emit deletePostClicked(postID); });

    footer->addWidget(delBtn);
    footer->addWidget(keepBtn);
    footer->addStretch();
    footer->addWidget(meta);
    vl->addLayout(footer);

    return card;
}

// ═══════════════════════════════════════════════════════
//  SETUP UI
// ═══════════════════════════════════════════════════════
void AdminDashboard::setupUI() {
    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ════════════════════════════════════════════════════
    //  SIDEBAR
    // ════════════════════════════════════════════════════
    QFrame *sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(200);

    QVBoxLayout *sideVl = new QVBoxLayout(sidebar);
    sideVl->setContentsMargins(0, 0, 0, 0);
    sideVl->setSpacing(0);

    // Logo
    QWidget *logoWidget = new QWidget();
    logoWidget->setObjectName("sideLogoArea");
    QVBoxLayout *logoVl = new QVBoxLayout(logoWidget);
    logoVl->setContentsMargins(16, 18, 16, 18);
    QLabel *logoText = new QLabel("⚡ CONNECTIFY");
    logoText->setObjectName("logoText");
    QLabel *logoSub  = new QLabel("Admin Panel");
    logoSub->setObjectName("logoSub");
    logoVl->addWidget(logoText);
    logoVl->addWidget(logoSub);
    sideVl->addWidget(logoWidget);

    // Nav buttons
    QWidget *navArea = new QWidget();
    navArea->setObjectName("navArea");
    QVBoxLayout *navVl = new QVBoxLayout(navArea);
    navVl->setContentsMargins(0, 8, 0, 8);
    navVl->setSpacing(2);

    auto makeNavBtn = [&](QPushButton *&btn, QString icon, QString text) {
        btn = new QPushButton(icon + "   " + text);
        btn->setObjectName("navBtn");
        btn->setFixedHeight(42);
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);
        navVl->addWidget(btn);
    };

    makeNavBtn(navDashBtn,  "📊", "Dashboard");
    makeNavBtn(navUsersBtn, "👥", "All Users");
    makeNavBtn(navPostsBtn, "📝", "Moderate Posts");

    // Extra nav items (non-functional for now)
    for (auto &item : QStringList{"🔔   Notifications",
                                  "🔍   Search",
                                  "⚙️   Settings"}) {
        QPushButton *btn = new QPushButton(item);
        btn->setObjectName("navBtn");
        btn->setFixedHeight(42);
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        navVl->addWidget(btn);
    }

    navVl->addStretch();
    sideVl->addWidget(navArea);

    // Sidebar footer — admin info
    QFrame *sideFooter = new QFrame();
    sideFooter->setObjectName("sideFooter");
    QHBoxLayout *sfHl = new QHBoxLayout(sideFooter);
    sfHl->setContentsMargins(14, 12, 14, 12);
    sfHl->setSpacing(8);

    adminAvatarLabel = new QLabel("AD");
    adminAvatarLabel->setObjectName("sideAvatar");
    adminAvatarLabel->setFixedSize(32, 32);
    adminAvatarLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *adminInfo = new QVBoxLayout();
    adminInfo->setSpacing(1);
    adminNameLabel = new QLabel("Admin");
    adminNameLabel->setObjectName("sideAdminName");
    QLabel *onlineStatus = new QLabel("● Online");
    onlineStatus->setObjectName("onlineStatus");
    adminInfo->addWidget(adminNameLabel);
    adminInfo->addWidget(onlineStatus);

    sfHl->addWidget(adminAvatarLabel);
    sfHl->addLayout(adminInfo);
    sideVl->addWidget(sideFooter);

    root->addWidget(sidebar);

    // ════════════════════════════════════════════════════
    //  RIGHT SIDE  (topbar + content)
    // ════════════════════════════════════════════════════
    QWidget *rightSide = new QWidget();
    rightSide->setObjectName("rightSide");
    QVBoxLayout *rightVl = new QVBoxLayout(rightSide);
    rightVl->setContentsMargins(0, 0, 0, 0);
    rightVl->setSpacing(0);

    // ── Topbar ────────────────────────────────────────────
    QFrame *topbar = new QFrame();
    topbar->setObjectName("topbar");
    topbar->setFixedHeight(60);

    QHBoxLayout *topHl = new QHBoxLayout(topbar);
    topHl->setContentsMargins(20, 0, 20, 0);

    QWidget *titleWidget = new QWidget();
    QVBoxLayout *titleVl = new QVBoxLayout(titleWidget);
    titleVl->setContentsMargins(0,0,0,0);
    titleVl->setSpacing(1);
    pageTitleLabel = new QLabel("Dashboard Overview");
    pageTitleLabel->setObjectName("pageTitle");
    pageSubLabel   = new QLabel("Welcome back, Admin");
    pageSubLabel->setObjectName("pageSub");
    titleVl->addWidget(pageTitleLabel);
    titleVl->addWidget(pageSubLabel);

    logoutBtn = new QPushButton("⬅  Logout");
    logoutBtn->setObjectName("logoutBtn");
    logoutBtn->setFixedHeight(32);
    logoutBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *notifBtn = new QPushButton("🔔");
    notifBtn->setObjectName("notifBtn");
    notifBtn->setFixedSize(34, 34);
    notifBtn->setCursor(Qt::PointingHandCursor);

    topHl->addWidget(titleWidget);
    topHl->addStretch();
    topHl->addWidget(notifBtn);
    topHl->addWidget(logoutBtn);
    rightVl->addWidget(topbar);

    // ── Content stack ─────────────────────────────────────
    contentStack = new QStackedWidget();
    contentStack->setObjectName("contentStack");

    // ── PAGE 0: Dashboard ─────────────────────────────────
    dashboardPage = new QWidget();
    dashboardPage->setObjectName("contentPage");
    QScrollArea *dashScroll = new QScrollArea();
    dashScroll->setWidgetResizable(true);
    dashScroll->setFrameShape(QFrame::NoFrame);

    QWidget *dashInner = new QWidget();
    QVBoxLayout *dashVl = new QVBoxLayout(dashInner);
    dashVl->setContentsMargins(20, 16, 20, 20);
    dashVl->setSpacing(20);

    // Stat cards grid
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsHl = new QHBoxLayout(statsWidget);
    statsHl->setSpacing(12);
    statsHl->setContentsMargins(0,0,0,0);
    statsHl->addWidget(makeStatCard(totalUsersNum,  "👥", "Total Users",      "↑ new today"));
    statsHl->addWidget(makeStatCard(totalPostsNum,  "📝", "Total Posts",      "↑ today"));
    statsHl->addWidget(makeStatCard(bannedUsersNum, "🚫", "Banned Users",     "↑ this week", true));
    statsHl->addWidget(makeStatCard(reviewPostsNum, "⚠️", "Posts to Review",  "Needs attention", true));
    dashVl->addWidget(statsWidget);

    // Quick summary label
    QLabel *quickTitle = new QLabel("Platform at a Glance");
    quickTitle->setObjectName("sectionTitle");
    dashVl->addWidget(quickTitle);

    QLabel *quickDesc = new QLabel(
        "Use the sidebar to manage users, moderate posts, and monitor activity.\n"
        "Switch to 'All Users' to ban or delete accounts.\n"
        "Switch to 'Moderate Posts' to review flagged content."
        );
    quickDesc->setObjectName("quickDesc");
    quickDesc->setWordWrap(true);
    dashVl->addWidget(quickDesc);
    dashVl->addStretch();

    dashScroll->setWidget(dashInner);
    QVBoxLayout *dashPageVl = new QVBoxLayout(dashboardPage);
    dashPageVl->setContentsMargins(0,0,0,0);
    dashPageVl->addWidget(dashScroll);
    contentStack->addWidget(dashboardPage);   // index 0

    // ── PAGE 1: All Users ─────────────────────────────────
    QWidget *usersPage = new QWidget();
    QScrollArea *usersScroll = new QScrollArea();
    usersScroll->setWidgetResizable(true);
    usersScroll->setFrameShape(QFrame::NoFrame);

    QWidget *usersInner = new QWidget();
    QVBoxLayout *usersVl = new QVBoxLayout(usersInner);
    usersVl->setContentsMargins(20, 16, 20, 20);
    usersVl->setSpacing(12);

    // Search bar
    QHBoxLayout *usersHeader = new QHBoxLayout();
    QLabel *usersTitle = new QLabel("All Users");
    usersTitle->setObjectName("sectionTitle");
    QLineEdit *searchBox = new QLineEdit();
    searchBox->setObjectName("searchBox");
    searchBox->setPlaceholderText("🔍  Search users...");
    searchBox->setFixedWidth(200);
    searchBox->setFixedHeight(32);
    usersHeader->addWidget(usersTitle);
    usersHeader->addStretch();
    usersHeader->addWidget(searchBox);
    usersVl->addLayout(usersHeader);

    // Table header
    QFrame *tableHead = new QFrame();
    tableHead->setObjectName("tableHead");
    QHBoxLayout *headHl = new QHBoxLayout(tableHead);
    headHl->setContentsMargins(14, 8, 14, 8);
    for (auto &col : QStringList{"User", "Role", "Posts", "Status", "Actions"}) {
        QLabel *th = new QLabel(col.toUpper());
        th->setObjectName("tableHeader");
        if (col == "User")    th->setFixedWidth(180);
        else if (col != "Actions") th->setFixedWidth(80);
        headHl->addWidget(th);
    }
    headHl->addStretch();
    usersVl->addWidget(tableHead);

    // User rows container
    userTableLayout = new QVBoxLayout();
    userTableLayout->setSpacing(0);
    userTableLayout->addStretch();
    usersVl->addLayout(userTableLayout);
    usersVl->addStretch();

    usersScroll->setWidget(usersInner);
    QVBoxLayout *usersPageVl = new QVBoxLayout(usersPage);
    usersPageVl->setContentsMargins(0,0,0,0);
    usersPageVl->addWidget(usersScroll);
    contentStack->addWidget(usersPage);   // index 1

    // ── PAGE 2: Moderate Posts ────────────────────────────
    QWidget *postsPage = new QWidget();
    QScrollArea *postsScroll = new QScrollArea();
    postsScroll->setWidgetResizable(true);
    postsScroll->setFrameShape(QFrame::NoFrame);

    QWidget *postsInner = new QWidget();
    QVBoxLayout *postsVl = new QVBoxLayout(postsInner);
    postsVl->setContentsMargins(20, 16, 20, 20);
    postsVl->setSpacing(12);

    QLabel *postsTitle2 = new QLabel("Moderate Posts");
    postsTitle2->setObjectName("sectionTitle");
    QLabel *postsSub = new QLabel("Review and delete inappropriate content");
    postsSub->setObjectName("pageSub");
    postsVl->addWidget(postsTitle2);
    postsVl->addWidget(postsSub);

    QWidget *gridWidget = new QWidget();
    modPostsLayout = new QGridLayout(gridWidget);
    modPostsLayout->setSpacing(12);
    postsVl->addWidget(gridWidget);
    postsVl->addStretch();

    postsScroll->setWidget(postsInner);
    QVBoxLayout *postsPageVl = new QVBoxLayout(postsPage);
    postsPageVl->setContentsMargins(0,0,0,0);
    postsPageVl->addWidget(postsScroll);
    contentStack->addWidget(postsPage);   // index 2

    rightVl->addWidget(contentStack);
    root->addWidget(rightSide);

    // ── Default: show dashboard ───────────────────────────
    navDashBtn->setChecked(true);
    contentStack->setCurrentIndex(0);

    // ── Sidebar nav signals ───────────────────────────────
    connect(navDashBtn, &QPushButton::clicked, this, [this]() {
        contentStack->setCurrentIndex(0);
        pageTitleLabel->setText("Dashboard Overview");
        pageSubLabel->setText("Welcome back, Admin");
        setActiveSidebarBtn(navDashBtn);
        emit navDashboard();
    });
    connect(navUsersBtn, &QPushButton::clicked, this, [this]() {
        contentStack->setCurrentIndex(1);
        pageTitleLabel->setText("All Users");
        pageSubLabel->setText("Manage, ban, or remove users");
        setActiveSidebarBtn(navUsersBtn);
        emit navUsers();
    });
    connect(navPostsBtn, &QPushButton::clicked, this, [this]() {
        contentStack->setCurrentIndex(2);
        pageTitleLabel->setText("Moderate Posts");
        pageSubLabel->setText("Review and delete inappropriate content");
        setActiveSidebarBtn(navPostsBtn);
        emit navPosts();
    });
    connect(logoutBtn, &QPushButton::clicked,
            this, &AdminDashboard::logoutClicked);
}

// ═══════════════════════════════════════════════════════
//  STYLESHEET
// ═══════════════════════════════════════════════════════
void AdminDashboard::applyStyles() {
    setStyleSheet(R"(
        QWidget          { background:#0d0d1a; color:#eeeeee;
                           font-family:'Segoe UI'; }
        QScrollArea      { border:none; background:#0d0d1a; }

        /* Sidebar */
        #sidebar         { background:#0a0a15;
                           border-right:1px solid #1e1e3a; }
        #sideLogoArea    { background:#0a0a15;
                           border-bottom:1px solid #1e1e3a; }
        #logoText        { font-size:14px; font-weight:800;
                           color:#a78bfa; letter-spacing:1px; }
        #logoSub         { font-size:10px; color:#6b7280; }
        #navArea         { background:#0a0a15; }
        #navBtn          { text-align:left; padding:0 16px;
                           font-size:13px; color:#6b7280;
                           background:transparent; border:none;
                           border-left:3px solid transparent; }
        #navBtn:hover    { color:#a78bfa;
                           background:rgba(124,58,237,0.08); }
        #navBtn:checked  { color:#a78bfa; font-weight:600;
                           background:rgba(124,58,237,0.12);
                           border-left:3px solid #7c3aed; }
        #sideFooter      { background:#0a0a15;
                           border-top:1px solid #1e1e3a; }
        #sideAvatar      { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                               stop:0 #7c3aed, stop:1 #4c1d95);
                           color:white; border-radius:16px;
                           font-size:11px; font-weight:bold; }
        #sideAdminName   { font-size:12px; font-weight:600;
                           color:#e5e7eb; }
        #onlineStatus    { font-size:10px; color:#34d399; }

        /* Topbar */
        #topbar          { background:#0a0a15;
                           border-bottom:1px solid #1e1e3a; }
        #pageTitle       { font-size:16px; font-weight:700;
                           color:#ffffff; }
        #pageSub         { font-size:11px; color:#6b7280; }
        #notifBtn        { background:rgba(124,58,237,0.15);
                           border:1px solid #2d1b69; border-radius:8px;
                           font-size:16px; }
        #notifBtn:hover  { background:rgba(124,58,237,0.25); }
        #logoutBtn       { background:rgba(239,68,68,0.1);
                           color:#f87171;
                           border:1px solid rgba(239,68,68,0.3);
                           border-radius:8px; padding:0 14px;
                           font-size:12px; font-weight:600; }
        #logoutBtn:hover { background:rgba(239,68,68,0.2); }

        /* Stat cards */
        #statCard        { background:#12122a;
                           border:1px solid #1e1e3a;
                           border-radius:12px; }
        #statIcon        { font-size:22px; }
        #statNum         { font-size:24px; font-weight:700;
                           color:#a78bfa; }
        #statLbl         { font-size:11px; color:#6b7280; }
        #statChangeGreen { font-size:10px; color:#34d399; }
        #statChangeRed   { font-size:10px; color:#f87171; }

        /* Section titles */
        #sectionTitle    { font-size:14px; font-weight:700;
                           color:#ffffff; }
        #quickDesc       { font-size:13px; color:#9ca3af;
                           line-height:1.8; background:#12122a;
                           border:1px solid #1e1e3a;
                           border-radius:10px; padding:14px; }

        /* User table */
        #tableHead       { background:#0d0d1a;
                           border:1px solid #1e1e3a;
                           border-radius:10px 10px 0 0; }
        #tableHeader     { font-size:10px; font-weight:700;
                           color:#6b7280; letter-spacing:1px; }
        #tableRow        { background:#12122a;
                           border-bottom:1px solid #1e1e3a; }
        #tableRow:hover  { background:#16163a; }
        #tableAvatar     { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                               stop:0 #7c3aed, stop:1 #4c1d95);
                           color:white; border-radius:15px;
                           font-size:10px; font-weight:bold; }
        #tableUserName   { font-size:12px; font-weight:600;
                           color:#e5e7eb; }
        #tableUserID     { font-size:10px; color:#6b7280; }
        #tableCell       { font-size:12px; color:#9ca3af; }

        /* Badges */
        #badgeAdmin      { background:rgba(167,139,250,0.1);
                           color:#a78bfa;
                           border:1px solid rgba(167,139,250,0.3);
                           border-radius:10px; padding:2px 8px;
                           font-size:10px; font-weight:600; }
        #badgeUser       { background:rgba(156,163,175,0.1);
                           color:#9ca3af;
                           border:1px solid #374151;
                           border-radius:10px; padding:2px 8px;
                           font-size:10px; font-weight:600; }
        #badgeActive     { background:rgba(52,211,153,0.1);
                           color:#34d399;
                           border:1px solid rgba(52,211,153,0.3);
                           border-radius:10px; padding:2px 8px;
                           font-size:10px; font-weight:600; }
        #badgeBanned     { background:rgba(248,113,113,0.1);
                           color:#f87171;
                           border:1px solid rgba(248,113,113,0.3);
                           border-radius:10px; padding:2px 8px;
                           font-size:10px; font-weight:600; }

        /* Action buttons */
        #btnView         { background:rgba(167,139,250,0.1);
                           color:#a78bfa;
                           border:1px solid rgba(167,139,250,0.3);
                           border-radius:6px; padding:0 10px;
                           font-size:11px; font-weight:600; }
        #btnBan          { background:rgba(251,191,36,0.1);
                           color:#fbbf24;
                           border:1px solid rgba(251,191,36,0.3);
                           border-radius:6px; padding:0 10px;
                           font-size:11px; font-weight:600; }
        #btnDelete       { background:rgba(248,113,113,0.1);
                           color:#f87171;
                           border:1px solid rgba(248,113,113,0.3);
                           border-radius:6px; padding:0 10px;
                           font-size:11px; font-weight:600; }
        #btnUnban        { background:rgba(52,211,153,0.1);
                           color:#34d399;
                           border:1px solid rgba(52,211,153,0.3);
                           border-radius:6px; padding:0 10px;
                           font-size:11px; font-weight:600; }

        /* Mod post cards */
        #modCard         { background:#12122a;
                           border:1px solid #1e1e3a;
                           border-radius:12px; }
        #modCard:hover   { border:1px solid #4c1d95; }
        #modUser         { font-size:11px; color:#a78bfa; }
        #modContent      { font-size:12px; color:#d1d5db; }
        #modMeta         { font-size:10px; color:#6b7280; }
        #btnModDelete    { background:rgba(248,113,113,0.1);
                           color:#f87171;
                           border:1px solid rgba(248,113,113,0.3);
                           border-radius:6px; padding:0 10px;
                           font-size:11px; font-weight:600; }
        #btnModKeep      { background:rgba(52,211,153,0.1);
                           color:#34d399;
                           border:1px solid rgba(52,211,153,0.3);
                           border-radius:6px; padding:0 10px;
                           font-size:11px; font-weight:600; }

        /* Search box */
        #searchBox       { background:#12122a;
                           border:1px solid #1e1e3a;
                           border-radius:8px; padding:0 10px;
                           font-size:12px; color:#eee; }
        #searchBox:focus { border:1px solid #7c3aed; }
    )");
}


