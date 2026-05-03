#include "searchpage.h"
#include <QStyle>
#include <QMouseEvent>

// ═══════════════════════════════════════════════════════
//  SEARCH RESULT ROW
// ═══════════════════════════════════════════════════════
SearchResultRow::SearchResultRow(const SearchUser &user, QWidget *parent)
    : QFrame(parent), m_userID(user.userID)
{
    setObjectName("resultRow");
    setFixedHeight(76);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(16, 12, 16, 12);
    hl->setSpacing(14);

    // ── Avatar ────────────────────────────────────────
    QLabel *avatar = new QLabel();
    avatar->setObjectName("resultAvatar");
    avatar->setFixedSize(46, 46);
    avatar->setAlignment(Qt::AlignCenter);

    QStringList parts = user.name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    avatar->setText(initials.left(2));
    hl->addWidget(avatar);

    // ── Name + username + bio ─────────────────────────
    QVBoxLayout *infoVl = new QVBoxLayout();
    infoVl->setSpacing(2);
    infoVl->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLbl = new QLabel(user.name);
    nameLbl->setObjectName("resultName");

    QLabel *usernameLbl = new QLabel("@" + user.username);
    usernameLbl->setObjectName("resultUsername");

    infoVl->addWidget(nameLbl);
    infoVl->addWidget(usernameLbl);

    if (!user.bio.isEmpty()) {
        QLabel *bioLbl = new QLabel(user.bio);
        bioLbl->setObjectName("resultBio");
        QFontMetrics fm(bioLbl->font());
        bioLbl->setText(fm.elidedText(user.bio, Qt::ElideRight, 340));
        infoVl->addWidget(bioLbl);
    }

    hl->addLayout(infoVl);
    hl->addStretch();

    // ── Action buttons ────────────────────────────────
    QPushButton *viewBtn = new QPushButton("View Profile");
    viewBtn->setObjectName("btnViewProfile");
    viewBtn->setFixedHeight(34);
    viewBtn->setFixedWidth(110);
    viewBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *friendBtn = new QPushButton(
        user.isFriend ? "✓  Friends" : "+ Add Friend");
    friendBtn->setObjectName(user.isFriend ? "btnAlreadyFriend" : "btnAddFriend");
    friendBtn->setFixedHeight(34);
    friendBtn->setFixedWidth(110);
    friendBtn->setCursor(Qt::PointingHandCursor);

    connect(viewBtn, &QPushButton::clicked, this,
        [this]() { emit viewProfileClicked(m_userID); });
    connect(friendBtn, &QPushButton::clicked, this,
        [this]() { emit addFriendClicked(m_userID); });

    hl->addWidget(viewBtn);
    hl->addWidget(friendBtn);
}

// ═══════════════════════════════════════════════════════
//  SEARCH PAGE — CONSTRUCTOR
// ═══════════════════════════════════════════════════════
SearchPage::SearchPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    applyStyles();
}

// ═══════════════════════════════════════════════════════
//  LOAD CURRENT USER
// ═══════════════════════════════════════════════════════
void SearchPage::loadCurrentUser(int userID, const QString &name) {
    m_currentUserID   = userID;
    m_currentUserName = name;

    currentUserNameLabel->setText(name);

    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    currentUserAvatarLabel->setText(initials.left(2));
}

// ═══════════════════════════════════════════════════════
//  ADD USER
// ═══════════════════════════════════════════════════════
void SearchPage::addUser(int userID, const QString &name,
                          const QString &username, const QString &bio,
                          bool isFriend)
{
    m_allUsers.append({userID, name, username, bio, isFriend});
    // Show all by default
    showResults(m_allUsers);
}

// ═══════════════════════════════════════════════════════
//  CLEAR RESULTS
// ═══════════════════════════════════════════════════════
void SearchPage::clearResults() {
    m_allUsers.clear();
    QLayoutItem *item;
    while ((item = resultsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    resultsLayout->addStretch();
    resultsCountLabel->setText("0 users found");
    noResultsWidget->hide();
}

// ═══════════════════════════════════════════════════════
//  ON SEARCH CHANGED
// ═══════════════════════════════════════════════════════
void SearchPage::onSearchChanged(const QString &query) {
    QString q = query.trimmed().toLower();

    if (q.isEmpty()) {
        searchQueryLabel->setText("Showing all users");
        showResults(m_allUsers);
        return;
    }

    searchQueryLabel->setText(QString("Showing results for: \"%1\"").arg(query.trimmed()));

    QList<SearchUser> filtered;
    for (auto &u : m_allUsers) {
        if (u.name.toLower().contains(q) ||
            u.username.toLower().contains(q) ||
            u.bio.toLower().contains(q))
        {
            filtered.append(u);
        }
    }
    showResults(filtered);
}

// ═══════════════════════════════════════════════════════
//  SHOW RESULTS
// ═══════════════════════════════════════════════════════
void SearchPage::showResults(const QList<SearchUser> &users) {
    // Clear old rows (keep stretch at end)
    QLayoutItem *item;
    while ((item = resultsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    resultsLayout->addStretch();

    if (users.isEmpty()) {
        noResultsWidget->show();
        resultsCountLabel->setText("0 users found");
        return;
    }

    noResultsWidget->hide();
    resultsCountLabel->setText(
        QString::number(users.size()) +
        (users.size() == 1 ? " user found" : " users found"));

    for (auto &u : users) {
        SearchResultRow *row = new SearchResultRow(u);
        connect(row, &SearchResultRow::viewProfileClicked,
                this, &SearchPage::viewProfileClicked);
        connect(row, &SearchResultRow::addFriendClicked,
                this, &SearchPage::addFriendClicked);
        // Insert before stretch
        resultsLayout->insertWidget(resultsLayout->count() - 1, row);
    }
}

// ═══════════════════════════════════════════════════════
//  HELPERS
// ═══════════════════════════════════════════════════════
QString SearchPage::makeInitials(const QString &name) {
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    return initials.left(2);
}

// ═══════════════════════════════════════════════════════
//  SETUP UI
// ═══════════════════════════════════════════════════════
void SearchPage::setupUI() {

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ════════════════════════════════════════════════════
    //  TOPBAR
    // ════════════════════════════════════════════════════
    QFrame *topbar = new QFrame();
    topbar->setObjectName("topbar");
    topbar->setFixedHeight(56);

    QHBoxLayout *topHl = new QHBoxLayout(topbar);
    topHl->setContentsMargins(20, 0, 20, 0);
    topHl->setSpacing(10);

    QLabel *logoText = new QLabel("⚡ CONNECTIFY");
    logoText->setObjectName("logoText");

    topHl->addWidget(logoText);
    topHl->addStretch();

    // Notification bell
    QPushButton *notifBtn = new QPushButton("🔔");
    notifBtn->setObjectName("notifBtn");
    notifBtn->setFixedSize(36, 36);
    notifBtn->setCursor(Qt::PointingHandCursor);

    // Current user chip
    QFrame *userChip = new QFrame();
    userChip->setObjectName("userChip");
    QHBoxLayout *chipHl = new QHBoxLayout(userChip);
    chipHl->setContentsMargins(8, 4, 12, 4);
    chipHl->setSpacing(8);

    currentUserAvatarLabel = new QLabel("AU");
    currentUserAvatarLabel->setObjectName("chipAvatar");
    currentUserAvatarLabel->setFixedSize(28, 28);
    currentUserAvatarLabel->setAlignment(Qt::AlignCenter);

    currentUserNameLabel = new QLabel("User");
    currentUserNameLabel->setObjectName("chipName");

    QLabel *chevron = new QLabel("▾");
    chevron->setObjectName("chipChevron");

    chipHl->addWidget(currentUserAvatarLabel);
    chipHl->addWidget(currentUserNameLabel);
    chipHl->addWidget(chevron);

    // Back button
    QPushButton *backBtn = new QPushButton("← Back");
    backBtn->setObjectName("backBtn");
    backBtn->setFixedHeight(32);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked,
            this, &SearchPage::backClicked);

    topHl->addWidget(notifBtn);
    topHl->addWidget(userChip);
    topHl->addWidget(backBtn);
    root->addWidget(topbar);

    // ════════════════════════════════════════════════════
    //  CONTENT AREA (scrollable)
    // ════════════════════════════════════════════════════
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("mainScroll");

    QWidget *inner = new QWidget();
    inner->setObjectName("innerWidget");
    QVBoxLayout *innerVl = new QVBoxLayout(inner);
    innerVl->setContentsMargins(32, 24, 32, 24);
    innerVl->setSpacing(16);

    // ── Page title ────────────────────────────────────
    QLabel *pageTitle = new QLabel("Search");
    pageTitle->setObjectName("pageTitle");
    innerVl->addWidget(pageTitle);

    // ── Search input ──────────────────────────────────
    QFrame *searchFrame = new QFrame();
    searchFrame->setObjectName("searchFrame");
    searchFrame->setFixedHeight(48);

    QHBoxLayout *sfHl = new QHBoxLayout(searchFrame);
    sfHl->setContentsMargins(14, 0, 14, 0);
    sfHl->setSpacing(10);

    QLabel *searchIcon = new QLabel("🔍");
    searchIcon->setObjectName("searchIcon");

    searchInput = new QLineEdit();
    searchInput->setObjectName("searchInput");
    searchInput->setPlaceholderText("Search users by name or username...");
    searchInput->setFrame(false);

    sfHl->addWidget(searchIcon);
    sfHl->addWidget(searchInput);
    innerVl->addWidget(searchFrame);

    // ── Users tab ─────────────────────────────────────
    QHBoxLayout *tabsHl = new QHBoxLayout();
    tabsHl->setContentsMargins(0, 0, 0, 0);
    tabsHl->setSpacing(0);

    QPushButton *usersTab = new QPushButton("👤  Users");
    usersTab->setObjectName("activeTab");
    usersTab->setFixedHeight(36);
    usersTab->setFixedWidth(110);
    usersTab->setCursor(Qt::PointingHandCursor);

    tabsHl->addWidget(usersTab);
    tabsHl->addStretch();
    innerVl->addLayout(tabsHl);

    // ── Results header ────────────────────────────────
    QHBoxLayout *resultsHeader = new QHBoxLayout();
    resultsHeader->setContentsMargins(0, 0, 0, 0);

    searchQueryLabel = new QLabel("Showing all users");
    searchQueryLabel->setObjectName("searchQueryLabel");

    resultsCountLabel = new QLabel("0 users found");
    resultsCountLabel->setObjectName("resultsCountLabel");

    resultsHeader->addWidget(searchQueryLabel);
    resultsHeader->addStretch();
    resultsHeader->addWidget(resultsCountLabel);
    innerVl->addLayout(resultsHeader);

    // ── Divider ───────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setObjectName("divider");
    divider->setFixedHeight(1);
    innerVl->addWidget(divider);

    // ── Results list ──────────────────────────────────
    QWidget *resultsWidget = new QWidget();
    resultsLayout = new QVBoxLayout(resultsWidget);
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->setSpacing(0);
    resultsLayout->addStretch();
    innerVl->addWidget(resultsWidget);

    // ── No results widget ─────────────────────────────
    noResultsWidget = new QWidget();
    noResultsWidget->hide();
    QVBoxLayout *nrVl = new QVBoxLayout(noResultsWidget);
    nrVl->setAlignment(Qt::AlignCenter);
    nrVl->setContentsMargins(0, 30, 0, 30);

    QLabel *nrIcon = new QLabel("😕");
    nrIcon->setObjectName("nrIcon");
    nrIcon->setAlignment(Qt::AlignCenter);

    QLabel *nrText = new QLabel("Can't find the user you're looking for?");
    nrText->setObjectName("nrText");
    nrText->setAlignment(Qt::AlignCenter);

    QLabel *nrSub = new QLabel("Try a different name or check your spelling.");
    nrSub->setObjectName("nrSub");
    nrSub->setAlignment(Qt::AlignCenter);

    nrVl->addWidget(nrIcon);
    nrVl->addSpacing(8);
    nrVl->addWidget(nrText);
    nrVl->addWidget(nrSub);
    innerVl->addWidget(noResultsWidget);

    innerVl->addStretch();

    scroll->setWidget(inner);
    root->addWidget(scroll);

    // ── Connect search input ──────────────────────────
    connect(searchInput, &QLineEdit::textChanged,
            this, &SearchPage::onSearchChanged);
}

// ═══════════════════════════════════════════════════════
//  STYLESHEET
// ═══════════════════════════════════════════════════════
void SearchPage::applyStyles() {
    setStyleSheet(R"(

        /* ── Base ─────────────────────────────────────── */
        QWidget      { background:#0d0d1a; color:#eeeeee;
                       font-family:'Segoe UI'; }
        QScrollArea  { border:none; background:#0d0d1a; }
        QScrollBar:vertical { background:#0d0d1a; width:4px; }
        QScrollBar::handle:vertical { background:#2d1b69;
                       border-radius:2px; min-height:20px; }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height:0; }

        /* ── Topbar ───────────────────────────────────── */
        #topbar      { background:#0a0a15;
                       border-bottom:1px solid #1e1e3a; }
        #logoText    { font-size:14px; font-weight:800;
                       color:#a78bfa; letter-spacing:1px; }
        #notifBtn    { background:rgba(124,58,237,0.15);
                       border:1px solid #2d1b69; border-radius:8px;
                       font-size:16px; }
        #notifBtn:hover { background:rgba(124,58,237,0.25); }

        #userChip    { background:#12122a;
                       border:1px solid #2d1b69;
                       border-radius:20px; }
        #chipAvatar  { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                           stop:0 #7c3aed, stop:1 #4c1d95);
                       color:white; border-radius:14px;
                       font-size:10px; font-weight:bold; }
        #chipName    { font-size:12px; font-weight:600; color:#e5e7eb; }
        #chipChevron { font-size:10px; color:#6b7280; }

        #backBtn     { background:rgba(124,58,237,0.15);
                       color:#a78bfa;
                       border:1px solid #2d1b69; border-radius:8px;
                       padding:0 14px; font-size:12px; font-weight:600; }
        #backBtn:hover { background:rgba(124,58,237,0.3); }

        /* ── Content ──────────────────────────────────── */
        #innerWidget { background:#0d0d1a; }
        #mainScroll  { background:#0d0d1a; }

        #pageTitle   { font-size:22px; font-weight:700; color:#ffffff; }

        /* ── Search frame ─────────────────────────────── */
        #searchFrame { background:#12122a;
                       border:1px solid #2d2d5a;
                       border-radius:12px; }
        #searchFrame:focus-within {
                       border:1px solid #7c3aed; }
        #searchIcon  { font-size:16px; }
        #searchInput { background:transparent; border:none;
                       font-size:14px; color:#eee;
                       selection-background-color:#7c3aed; }

        /* ── Tabs ─────────────────────────────────────── */
        #activeTab   { background:rgba(124,58,237,0.15);
                       color:#a78bfa;
                       border:1px solid rgba(124,58,237,0.4);
                       border-radius:20px;
                       font-size:12px; font-weight:600; }

        /* ── Results header ───────────────────────────── */
        #searchQueryLabel { font-size:13px; color:#9ca3af; }
        #searchQueryLabel QLabel { color:#a78bfa; }
        #resultsCountLabel { font-size:13px; color:#6b7280; }
        #divider     { background:#1e1e3a; }

        /* ── Result rows ──────────────────────────────── */
        #resultRow   { background:#0d0d1a;
                       border-bottom:1px solid #1a1a2e; }
        #resultRow:hover { background:#12122a; }

        #resultAvatar { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                            stop:0 #7c3aed, stop:1 #4c1d95);
                        color:white; border-radius:23px;
                        font-size:14px; font-weight:bold; }
        #resultName   { font-size:14px; font-weight:600; color:#ffffff; }
        #resultUsername { font-size:12px; color:#7c3aed; }
        #resultBio    { font-size:12px; color:#6b7280; }

        /* ── Action buttons ───────────────────────────── */
        #btnViewProfile { background:transparent;
                          color:#a78bfa;
                          border:1px solid rgba(124,58,237,0.5);
                          border-radius:8px;
                          font-size:12px; font-weight:600; }
        #btnViewProfile:hover { background:rgba(124,58,237,0.15); }

        #btnAddFriend { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                            stop:0 #7c3aed, stop:1 #5b21b6);
                        color:white; border:none;
                        border-radius:8px;
                        font-size:12px; font-weight:600; }
        #btnAddFriend:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                            stop:0 #8b5cf6, stop:1 #6d28d9); }

        #btnAlreadyFriend { background:rgba(52,211,153,0.1);
                            color:#34d399;
                            border:1px solid rgba(52,211,153,0.3);
                            border-radius:8px;
                            font-size:12px; font-weight:600; }

        /* ── No results ───────────────────────────────── */
        #nrIcon  { font-size:40px; }
        #nrText  { font-size:14px; color:#9ca3af; font-weight:600; }
        #nrSub   { font-size:12px; color:#7c3aed; }

    )");
}
