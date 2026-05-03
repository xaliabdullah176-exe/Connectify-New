#include "profilepage.h"

// ═══════════════════════════════════════════════════════
//  CONSTRUCTOR
// ═══════════════════════════════════════════════════════
ProfilePage::ProfilePage(QWidget *parent) : QWidget(parent) {
    setupUI();
    applyStyles();
}

// ═══════════════════════════════════════════════════════
//  LOAD PROFILE  — plug in your User* fields here later
// ═══════════════════════════════════════════════════════
void ProfilePage::loadProfile(int    userID,
                               QString name,
                               QString handle,
                               QString bio,
                               QString location,
                               QString joinDate,
                               QString role,
                               int posts, int friends,
                               int followers, int following,
                               bool isOwn)
{
    // Avatar initials
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    avatarLabel->setText(initials.left(2));

    nameLabel->setText(name);
    handleLabel->setText("@" + handle);
    bioLabel->setText(bio);
    locationLabel->setText("📍 " + location);
    joinDateLabel->setText("📅 " + joinDate);

    postsNum->setText(QString::number(posts));
    friendsNum->setText(QString::number(friends));
    followersNum->setText(QString::number(followers));
    followingNum->setText(QString::number(following));

    friendsCountLabel->setText("FRIENDS (" + QString::number(friends) + ")");

    editBtn->setText(isOwn ? "✏️  Edit Profile" : "➕  Follow");
}

// ═══════════════════════════════════════════════════════
//  ADD TEXT POST
// ═══════════════════════════════════════════════════════
void ProfilePage::addTextPost(int postID, QString name, QString handle,
                               QString content, QString timeAgo,
                               int likes, int comments)
{
    postsLayout->insertWidget(
        postsLayout->count() - 1,   // insert before the stretch
        makePostCard(postID, name, handle, content, "", timeAgo,
                     likes, comments, false)
    );
}

// ═══════════════════════════════════════════════════════
//  ADD IMAGE POST
// ═══════════════════════════════════════════════════════
void ProfilePage::addImagePost(int postID, QString name, QString handle,
                                QString content, QString imagePath,
                                QString timeAgo, int likes, int comments)
{
    postsLayout->insertWidget(
        postsLayout->count() - 1,
        makePostCard(postID, name, handle, content, imagePath, timeAgo,
                     likes, comments, true)
    );
}

// ═══════════════════════════════════════════════════════
//  ADD FRIEND TO SIDEBAR
// ═══════════════════════════════════════════════════════
void ProfilePage::addFriend(QString name, QString mutualCount) {
    // Insert before the "View All" button (last item)
    friendsLayout->insertWidget(
        friendsLayout->count() - 1,
        makeFriendItem(name, mutualCount)
    );
}

// ═══════════════════════════════════════════════════════
//  CLEAR POSTS
// ═══════════════════════════════════════════════════════
void ProfilePage::clearPosts() {
    QLayoutItem *item;
    while ((item = postsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    postsLayout->addStretch();
}

// ═══════════════════════════════════════════════════════
//  MAKE POST CARD  (text or image)
// ═══════════════════════════════════════════════════════
QFrame* ProfilePage::makePostCard(int postID, QString name, QString handle,
                                   QString content, QString imagePath,
                                   QString timeAgo, int likes, int comments,
                                   bool hasImage)
{
    QFrame *card = new QFrame();
    card->setObjectName("postCard");

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    // ── Top row: avatar + name + handle + time ──────────
    QWidget *topWidget = new QWidget();
    topWidget->setObjectName("postTop");
    QHBoxLayout *topRow = new QHBoxLayout(topWidget);
    topRow->setContentsMargins(14, 12, 14, 8);
    topRow->setSpacing(10);

    // Mini avatar
    QLabel *av = new QLabel();
    av->setObjectName("miniAvatar");
    av->setFixedSize(36, 36);
    av->setAlignment(Qt::AlignCenter);
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    av->setText(initials.left(2));

    // Name + handle + time
    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(1);
    QLabel *pname = new QLabel(name);
    pname->setObjectName("postName");
    QLabel *phandle = new QLabel("@" + handle + " · " + timeAgo);
    phandle->setObjectName("postHandle");
    nameCol->addWidget(pname);
    nameCol->addWidget(phandle);

    QLabel *menu = new QLabel("⋯");
    menu->setObjectName("postMenu");
    menu->setCursor(Qt::PointingHandCursor);

    topRow->addWidget(av);
    topRow->addLayout(nameCol);
    topRow->addStretch();
    topRow->addWidget(menu);
    vl->addWidget(topWidget);

    // ── Content text ─────────────────────────────────────
    QLabel *text = new QLabel(content);
    text->setObjectName("postContent");
    text->setWordWrap(true);
    text->setContentsMargins(14, 0, 14, 10);
    vl->addWidget(text);

    // ── Image (if ImagePost) ──────────────────────────────
    if (hasImage) {
        QLabel *imgLabel = new QLabel();
        imgLabel->setFixedHeight(180);
        imgLabel->setAlignment(Qt::AlignCenter);
        imgLabel->setObjectName("postImage");

        QPixmap px(imagePath);
        if (!px.isNull()) {
            imgLabel->setPixmap(
                px.scaled(600, 180, Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation)
            );
        } else {
            // Placeholder until real image
            imgLabel->setText("🌅");
            imgLabel->setStyleSheet(
                "font-size:40px; background:#1a0a3d;"
            );
        }
        vl->addWidget(imgLabel);
    }

    // ── Footer: likes / comments / share ─────────────────
    QFrame *footer = new QFrame();
    footer->setObjectName("postFooter");
    QHBoxLayout *footerRow = new QHBoxLayout(footer);
    footerRow->setContentsMargins(14, 8, 14, 10);
    footerRow->setSpacing(16);

    QPushButton *likeBtn = new QPushButton("♥  " + QString::number(likes));
    likeBtn->setObjectName("likeBtn");
    likeBtn->setFlat(true);
    likeBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *cmtBtn = new QPushButton("💬  " + QString::number(comments));
    cmtBtn->setObjectName("cmtBtn");
    cmtBtn->setFlat(true);
    cmtBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *shareBtn = new QPushButton("↗  Share");
    shareBtn->setObjectName("shareBtn");
    shareBtn->setFlat(true);
    shareBtn->setCursor(Qt::PointingHandCursor);

    footerRow->addWidget(likeBtn);
    footerRow->addWidget(cmtBtn);
    footerRow->addStretch();
    footerRow->addWidget(shareBtn);
    vl->addWidget(footer);

    return card;
}

// ═══════════════════════════════════════════════════════
//  MAKE FRIEND ITEM
// ═══════════════════════════════════════════════════════
QFrame* ProfilePage::makeFriendItem(QString name, QString mutual) {
    QFrame *item = new QFrame();
    item->setObjectName("friendItem");
    QHBoxLayout *hl = new QHBoxLayout(item);
    hl->setContentsMargins(0, 6, 0, 6);
    hl->setSpacing(10);

    // Avatar
    QLabel *av = new QLabel();
    av->setObjectName("friendAvatar");
    av->setFixedSize(36, 36);
    av->setAlignment(Qt::AlignCenter);
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    av->setText(initials.left(2));

    QVBoxLayout *info = new QVBoxLayout();
    info->setSpacing(1);
    QLabel *fn = new QLabel(name);
    fn->setObjectName("friendName");
    QLabel *fm = new QLabel(mutual);
    fm->setObjectName("friendMutual");
    info->addWidget(fn);
    info->addWidget(fm);

    QLabel *msgIcon = new QLabel("💬");
    msgIcon->setCursor(Qt::PointingHandCursor);
    msgIcon->setObjectName("friendMsg");

    hl->addWidget(av);
    hl->addLayout(info);
    hl->addStretch();
    hl->addWidget(msgIcon);
    return item;
}

// ═══════════════════════════════════════════════════════
//  STAT CELL HELPER
// ═══════════════════════════════════════════════════════
QWidget* ProfilePage::makeStatCell(QLabel *&numLabel, const QString &labelText) {
    QWidget *cell = new QWidget();
    cell->setObjectName("statCell");
    QVBoxLayout *vl = new QVBoxLayout(cell);
    vl->setContentsMargins(16, 10, 16, 10);
    vl->setSpacing(2);
    vl->setAlignment(Qt::AlignCenter);
    numLabel = new QLabel("0");
    numLabel->setObjectName("statNum");
    numLabel->setAlignment(Qt::AlignCenter);
    QLabel *lbl = new QLabel(labelText);
    lbl->setObjectName("statLbl");
    lbl->setAlignment(Qt::AlignCenter);
    vl->addWidget(numLabel);
    vl->addWidget(lbl);
    return cell;
}

// ═══════════════════════════════════════════════════════
//  SETUP UI
// ═══════════════════════════════════════════════════════
void ProfilePage::setupUI() {
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Cover banner ─────────────────────────────────────
    QFrame *cover = new QFrame();
    cover->setObjectName("coverFrame");
    cover->setFixedHeight(160);
    root->addWidget(cover);

    // ── Profile header bar ────────────────────────────────
    QFrame *headerBar = new QFrame();
    headerBar->setObjectName("headerBar");
    QVBoxLayout *headerVl = new QVBoxLayout(headerBar);
    headerVl->setContentsMargins(28, 0, 28, 14);
    headerVl->setSpacing(4);

    // Avatar overlapping cover (positioned via negative margin trick)
    avatarLabel = new QLabel("?");
    avatarLabel->setObjectName("avatarLabel");
    avatarLabel->setFixedSize(88, 88);
    avatarLabel->setAlignment(Qt::AlignCenter);

    // Row: avatar left, edit button right
    QHBoxLayout *avatarRow = new QHBoxLayout();
    avatarRow->setContentsMargins(0, -44, 0, 0);  // pulls avatar up over cover
    avatarRow->addWidget(avatarLabel);
    avatarRow->addStretch();
    editBtn = new QPushButton("✏️  Edit Profile");
    editBtn->setObjectName("editBtn");
    editBtn->setFixedHeight(34);
    editBtn->setCursor(Qt::PointingHandCursor);
    avatarRow->addWidget(editBtn);
    headerVl->addLayout(avatarRow);

    nameLabel = new QLabel("...");
    nameLabel->setObjectName("profileName");
    headerVl->addWidget(nameLabel);

    handleLabel = new QLabel("@...");
    handleLabel->setObjectName("profileHandle");
    headerVl->addWidget(handleLabel);

    bioLabel = new QLabel("...");
    bioLabel->setObjectName("profileBio");
    bioLabel->setWordWrap(true);
    headerVl->addWidget(bioLabel);

    QHBoxLayout *metaRow = new QHBoxLayout();
    metaRow->setSpacing(20);
    locationLabel = new QLabel("📍 ...");
    joinDateLabel = new QLabel("📅 ...");
    locationLabel->setObjectName("metaLabel");
    joinDateLabel->setObjectName("metaLabel");
    metaRow->addWidget(locationLabel);
    metaRow->addWidget(joinDateLabel);
    metaRow->addStretch();
    headerVl->addLayout(metaRow);

    root->addWidget(headerBar);

    // ── Stats row ─────────────────────────────────────────
    QFrame *statsBar = new QFrame();
    statsBar->setObjectName("statsBar");
    QHBoxLayout *sl = new QHBoxLayout(statsBar);
    sl->setContentsMargins(20, 0, 20, 0);
    sl->setSpacing(0);
    sl->addWidget(makeStatCell(postsNum,     "Posts"));
    sl->addWidget(makeStatCell(friendsNum,   "Friends"));
    sl->addWidget(makeStatCell(followersNum, "Followers"));
    sl->addWidget(makeStatCell(followingNum, "Following"));
    sl->addStretch();
    root->addWidget(statsBar);

    // ── Scrollable main body ──────────────────────────────
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("scrollArea");

    QWidget *body = new QWidget();
    body->setObjectName("bodyWidget");
    QHBoxLayout *bodyRow = new QHBoxLayout(body);
    bodyRow->setContentsMargins(20, 16, 20, 20);
    bodyRow->setSpacing(16);
    bodyRow->setAlignment(Qt::AlignTop);

    // ── LEFT COLUMN ───────────────────────────────────────
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(12);
    leftCol->setAlignment(Qt::AlignTop);

    // About card
    QFrame *aboutCard = new QFrame();
    aboutCard->setObjectName("sideCard");
    aboutCard->setFixedWidth(240);
    QVBoxLayout *aboutVl = new QVBoxLayout(aboutCard);
    aboutVl->setContentsMargins(14, 12, 14, 12);
    aboutVl->setSpacing(8);
    QLabel *aboutTitle = new QLabel("ABOUT");
    aboutTitle->setObjectName("cardTitle");
    aboutVl->addWidget(aboutTitle);
    // About rows (static for now — link to backend later)
    auto addAboutRow = [&](QString icon, QString text, QString objName = "aboutRow") {
        QLabel *row = new QLabel(icon + "  " + text);
        row->setObjectName(objName);
        row->setWordWrap(true);
        aboutVl->addWidget(row);
    };
    addAboutRow("✉️", "user@email.com");
    addAboutRow("📅", "Date of Birth");
    addAboutRow("🔗", "github.com/username");
    leftCol->addWidget(aboutCard);

    // Friends card
    QFrame *friendsCard = new QFrame();
    friendsCard->setObjectName("sideCard");
    friendsCard->setFixedWidth(240);
    QVBoxLayout *friendsVl = new QVBoxLayout(friendsCard);
    friendsVl->setContentsMargins(14, 12, 14, 12);
    friendsVl->setSpacing(0);

    QHBoxLayout *fHeader = new QHBoxLayout();
    friendsCountLabel = new QLabel("FRIENDS");
    friendsCountLabel->setObjectName("cardTitle");
    QPushButton *viewAllTop = new QPushButton("View All");
    viewAllTop->setObjectName("viewAllBtn");
    viewAllTop->setFlat(true);
    viewAllTop->setCursor(Qt::PointingHandCursor);
    fHeader->addWidget(friendsCountLabel);
    fHeader->addStretch();
    fHeader->addWidget(viewAllTop);
    friendsVl->addLayout(fHeader);

    // Friends list layout — addFriend() inserts here
    friendsLayout = new QVBoxLayout();
    friendsLayout->setSpacing(0);
    friendsVl->addLayout(friendsLayout);

    QPushButton *viewAllBtn = new QPushButton("View All Friends");
    viewAllBtn->setObjectName("viewAllFullBtn");
    viewAllBtn->setCursor(Qt::PointingHandCursor);
    viewAllBtn->setFixedHeight(32);
    friendsLayout->addWidget(viewAllBtn);   // addFriend inserts BEFORE this

    leftCol->addWidget(friendsCard);
    leftCol->addStretch();
    bodyRow->addLayout(leftCol);

    // ── RIGHT COLUMN (Posts) ──────────────────────────────
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(12);
    rightCol->setAlignment(Qt::AlignTop);

    // Posts header
    QHBoxLayout *postsHeader = new QHBoxLayout();
    QLabel *postsTitle = new QLabel("Posts");
    postsTitle->setObjectName("postsTitle");
    QPushButton *createPost = new QPushButton("✏️  Create Post");
    createPost->setObjectName("createPostBtn");
    createPost->setCursor(Qt::PointingHandCursor);
    createPost->setFixedHeight(32);
    postsHeader->addWidget(postsTitle);
    postsHeader->addStretch();
    postsHeader->addWidget(createPost);
    rightCol->addLayout(postsHeader);

    // Posts container
    postsLayout = new QVBoxLayout();
    postsLayout->setSpacing(12);
    postsLayout->addStretch();  // clearPosts() and addPost() work around this
    rightCol->addLayout(postsLayout);

    bodyRow->addLayout(rightCol);

    scroll->setWidget(body);
    root->addWidget(scroll);

    // ── Signals ───────────────────────────────────────────
    connect(editBtn,    &QPushButton::clicked, this, &ProfilePage::editProfileClicked);
    connect(createPost, &QPushButton::clicked, this, &ProfilePage::createPostClicked);
    connect(viewAllBtn, &QPushButton::clicked, this, &ProfilePage::viewAllFriendsClicked);
    connect(viewAllTop, &QPushButton::clicked, this, &ProfilePage::viewAllFriendsClicked);
}

// ═══════════════════════════════════════════════════════
//  STYLESHEET — Purple / Black theme
// ═══════════════════════════════════════════════════════
void ProfilePage::applyStyles() {
    setStyleSheet(R"(
        /* Base */
        QWidget        { background:#0d0d1a; color:#eeeeee;
                         font-family:'Segoe UI'; }
        QScrollArea    { border:none; background:#0d0d1a; }
        #bodyWidget    { background:#0d0d1a; }

        /* Cover */
        #coverFrame    { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #2d1b69, stop:1 #1a0a3d); }

        /* Header bar */
        #headerBar     { background:#12122a;
                         border-bottom:1px solid #1e1e3a; }

        /* Avatar */
        #avatarLabel   { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #4c1d95);
                         color:white; border-radius:44px;
                         font-size:26px; font-weight:bold;
                         border:3px solid #12122a; }

        /* Profile text */
        #profileName   { font-size:20px; font-weight:bold; color:#ffffff; }
        #profileHandle { font-size:13px; color:#a78bfa; }
        #profileBio    { font-size:12px; color:#9ca3af; }
        #metaLabel     { font-size:11px; color:#6b7280; }

        /* Edit button */
        #editBtn       { background:transparent; color:#a78bfa;
                         border:1.5px solid #7c3aed; border-radius:8px;
                         padding:0 16px; font-size:12px; font-weight:600; }
        #editBtn:hover { background:rgba(124,58,237,0.15); }

        /* Stats bar */
        #statsBar      { background:#12122a;
                         border-bottom:1px solid #1e1e3a; }
        #statCell      { border-right:1px solid #1e1e3a; }
        #statNum       { font-size:17px; font-weight:bold; color:#a78bfa; }
        #statLbl       { font-size:10px; color:#6b7280; }

        /* Side cards */
        #sideCard      { background:#12122a; border:1px solid #1e1e3a;
                         border-radius:12px; }
        #cardTitle     { font-size:10px; font-weight:bold; color:#6b7280;
                         letter-spacing:1px; margin-bottom:6px; }
        #aboutRow      { font-size:12px; color:#9ca3af; }

        /* Friend items */
        #friendItem    { border-bottom:1px solid #1e1e3a; }
        #friendAvatar  { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #4c1d95);
                         color:white; border-radius:18px;
                         font-size:11px; font-weight:bold; }
        #friendName    { font-size:12px; font-weight:600; color:#e5e7eb; }
        #friendMutual  { font-size:10px; color:#6b7280; }
        #friendMsg     { font-size:14px; }
        #viewAllBtn    { font-size:11px; color:#a78bfa;
                         background:transparent; border:none; }
        #viewAllFullBtn { background:transparent; color:#a78bfa;
                          border:1px solid #2d1b69; border-radius:8px;
                          font-size:12px; margin-top:8px; }
        #viewAllFullBtn:hover { background:rgba(124,58,237,0.1); }

        /* Posts section */
        #postsTitle    { font-size:14px; font-weight:bold; color:#ffffff; }
        #createPostBtn { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #4c1d95);
                         color:white; border:none; border-radius:8px;
                         padding:0 14px; font-size:12px; font-weight:600; }
        #createPostBtn:hover { background:#6d28d9; }

        /* Post card */
        #postCard      { background:#12122a; border:1px solid #1e1e3a;
                         border-radius:12px; }
        #postCard:hover { border:1px solid #4c1d95; }
        #postTop       { background:transparent; }
        #miniAvatar    { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #4c1d95);
                         color:white; border-radius:18px;
                         font-size:11px; font-weight:bold; }
        #postName      { font-size:13px; font-weight:600; color:#ffffff; }
        #postHandle    { font-size:11px; color:#6b7280; }
        #postMenu      { font-size:18px; color:#6b7280; }
        #postContent   { font-size:13px; color:#d1d5db; }
        #postImage     { background:#1a0a3d; }
        #postFooter    { background:transparent;
                         border-top:1px solid #1e1e3a; }
        #likeBtn       { font-size:12px; color:#a78bfa;
                         background:transparent; border:none; }
        #likeBtn:hover { color:#7c3aed; }
        #cmtBtn        { font-size:12px; color:#6b7280;
                         background:transparent; border:none; }
        #cmtBtn:hover  { color:#a78bfa; }
        #shareBtn      { font-size:12px; color:#6b7280;
                         background:transparent; border:none; }
        #shareBtn:hover { color:#a78bfa; }
    )");
}
