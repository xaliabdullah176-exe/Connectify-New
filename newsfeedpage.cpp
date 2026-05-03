#include "newsfeedpage.h"
#include <QScrollBar>
#include <QPixmap>
#include <QFileInfo>
#include <ctime>

// ═══════════════════════════════════════════════════════
//  FeedPostCard — single post card
// ═══════════════════════════════════════════════════════
FeedPostCard::FeedPostCard(int postID,
                           const QString &ownerName,
                           const QString &content,
                           const QString &imagePath,
                           const QString &timeAgo,
                           int likes,
                           int comments,
                           bool canInteract,
                           QWidget *parent)
    : QFrame(parent), m_postID(postID)
{
    setObjectName("postCard");

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    // ── Top row: avatar + name + time ─────────────────
    QWidget *topWidget = new QWidget();
    topWidget->setObjectName("postTop");
    QHBoxLayout *topRow = new QHBoxLayout(topWidget);
    topRow->setContentsMargins(16, 14, 16, 10);
    topRow->setSpacing(10);

    // Avatar initials
    QLabel *av = new QLabel();
    av->setObjectName("miniAvatar");
    av->setFixedSize(40, 40);
    av->setAlignment(Qt::AlignCenter);
    QStringList parts = ownerName.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    av->setText(initials.left(2));

    // Name + time
    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *pname = new QLabel(ownerName);
    pname->setObjectName("postName");
    QLabel *ptime = new QLabel(timeAgo);
    ptime->setObjectName("postHandle");
    nameCol->addWidget(pname);
    nameCol->addWidget(ptime);

    topRow->addWidget(av);
    topRow->addLayout(nameCol);
    topRow->addStretch();
    vl->addWidget(topWidget);

    // ── Content text ──────────────────────────────────
    if (!content.isEmpty()) {
        QLabel *contentLabel = new QLabel(content);
        contentLabel->setObjectName("postContent");
        contentLabel->setWordWrap(true);
        contentLabel->setContentsMargins(16, 4, 16, 10);
        vl->addWidget(contentLabel);
    }

    // ── Image (if present) ────────────────────────────
    if (!imagePath.isEmpty()) {
        QLabel *imgLabel = new QLabel();
        imgLabel->setObjectName("postImage");
        imgLabel->setAlignment(Qt::AlignCenter);
        imgLabel->setFixedHeight(260);

        QPixmap px(imagePath);
        if (!px.isNull()) {
            imgLabel->setPixmap(px.scaledToWidth(500, Qt::SmoothTransformation)
                                  .scaled(imgLabel->width(), 260,
                                          Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation));
        } else {
            imgLabel->setText("🖼️  Image not found");
            imgLabel->setStyleSheet("font-size:14px; color:#6b7280; "
                                    "background:#1a0a3d;");
        }
        vl->addWidget(imgLabel);
    }

    // ── Footer: like / comment ─────────────────────────
    QWidget *footer = new QWidget();
    footer->setObjectName("postFooter");
    QHBoxLayout *footerRow = new QHBoxLayout(footer);
    footerRow->setContentsMargins(12, 8, 12, 8);
    footerRow->setSpacing(8);

    // Like button
    QPushButton *likeBtn = new QPushButton(QString("❤️  %1").arg(likes));
    likeBtn->setObjectName(canInteract ? "likeBtn" : "likeBtnDisabled");
    likeBtn->setCursor(canInteract ? Qt::PointingHandCursor : Qt::ArrowCursor);
    likeBtn->setEnabled(canInteract);
    likesLabel = new QLabel();  // not used here — likeBtn text updates instead

    // Comment button
    QPushButton *cmtBtn = new QPushButton(QString("💬  %1").arg(comments));
    cmtBtn->setObjectName(canInteract ? "cmtBtn" : "cmtBtnDisabled");
    cmtBtn->setCursor(canInteract ? Qt::PointingHandCursor : Qt::ArrowCursor);
    cmtBtn->setEnabled(canInteract);

    if (!canInteract) {
        QLabel *hint = new QLabel("(Friends only)");
        hint->setObjectName("hintLabel");
        footerRow->addWidget(hint);
        footerRow->addStretch();
    }

    footerRow->addWidget(likeBtn);
    footerRow->addWidget(cmtBtn);
    footerRow->addStretch();

    vl->addWidget(footer);

    // Connections
    connect(likeBtn, &QPushButton::clicked, this, [this, likeBtn, likes]() mutable {
        emit likeClicked(m_postID);
        // Optimistic UI update
        int cur = likeBtn->text().split("  ").last().toInt();
        likeBtn->setText(QString("❤️  %1").arg(cur + 1));
    });

    connect(cmtBtn, &QPushButton::clicked, this, [this]() {
        emit commentClicked(m_postID);
    });
}

void FeedPostCard::updateLikes(int newCount) {
    // If you need to update from outside, find the likeBtn child and update
    QList<QPushButton*> btns = findChildren<QPushButton*>("likeBtn");
    if (!btns.isEmpty())
        btns[0]->setText(QString("❤️  %1").arg(newCount));
}

// ═══════════════════════════════════════════════════════
//  NewsFeedPage CONSTRUCTOR
// ═══════════════════════════════════════════════════════
NewsFeedPage::NewsFeedPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    applyStyles();
}

// ═══════════════════════════════════════════════════════
//  LOAD CURRENT USER
// ═══════════════════════════════════════════════════════
void NewsFeedPage::loadCurrentUser(int userID, const QString &name) {
    m_currentUserID   = userID;
    m_currentUserName = name;

    // Update navbar avatar + name
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    navAvatarLabel->setText(initials.left(2));
    navUserNameLabel->setText(name);
}

// ═══════════════════════════════════════════════════════
//  ADD POST TO FEED
// ═══════════════════════════════════════════════════════
void NewsFeedPage::addPost(int postID,
                           const QString &ownerName,
                           const QString &content,
                           const QString &imagePath,
                           const QString &timeAgo,
                           int likes,
                           int comments,
                           bool canInteract)
{
    FeedPostCard *card = new FeedPostCard(
        postID, ownerName, content, imagePath,
        timeAgo, likes, comments, canInteract
    );

    connect(card, &FeedPostCard::likeClicked,    this, &NewsFeedPage::likeClicked);
    connect(card, &FeedPostCard::commentClicked, this, &NewsFeedPage::commentClicked);

    // Insert before the bottom stretch
    feedLayout->insertWidget(feedLayout->count() - 1, card);
}

// ═══════════════════════════════════════════════════════
//  CLEAR FEED
// ═══════════════════════════════════════════════════════
void NewsFeedPage::clearFeed() {
    QLayoutItem *item;
    while ((item = feedLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    feedLayout->addStretch();
}

// ═══════════════════════════════════════════════════════
//  UPDATE POST LIKES
// ═══════════════════════════════════════════════════════
void NewsFeedPage::updatePostLikes(int postID, int newCount) {
    QList<FeedPostCard*> cards = findChildren<FeedPostCard*>();
    for (auto *c : cards) {
        // FeedPostCard doesn't expose postID publicly, handled via optimistic UI
        // You can extend FeedPostCard to expose getPostID() if needed
    }
}

// ═══════════════════════════════════════════════════════
//  ON POST BUTTON CLICKED
// ═══════════════════════════════════════════════════════
void NewsFeedPage::onPostBtnClicked() {
    QString content = postInput->toPlainText().trimmed();
    if (content.isEmpty() && m_selectedImagePath.isEmpty()) return;

    emit createPostClicked(content, m_selectedImagePath);

    // Clear the compose box
    postInput->clear();
    m_selectedImagePath.clear();
    imagePreviewLabel->hide();
    selectImageBtn->setText("📷  Add Photo");
    charCountLabel->setText("0 / 280");
}

// ═══════════════════════════════════════════════════════
//  SELECT IMAGE
// ═══════════════════════════════════════════════════════
void NewsFeedPage::onSelectImageClicked() {
    if (!m_selectedImagePath.isEmpty()) {
        // Clear selected image
        m_selectedImagePath.clear();
        imagePreviewLabel->hide();
        selectImageBtn->setText("📷  Add Photo");
        return;
    }

    QString path = QFileDialog::getOpenFileName(
        this, "Select Image", "",
        "Images (*.png *.jpg *.jpeg *.gif *.bmp *.webp)"
    );
    if (path.isEmpty()) return;

    m_selectedImagePath = path;
    QPixmap px(path);
    if (!px.isNull()) {
        imagePreviewLabel->setPixmap(
            px.scaled(300, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
        imagePreviewLabel->show();
    }
    QFileInfo fi(path);
    selectImageBtn->setText("❌  Remove  " + fi.fileName());
}

// ═══════════════════════════════════════════════════════
//  SETUP UI
// ═══════════════════════════════════════════════════════
void NewsFeedPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ══════════════════════════════════════════════════
    //  TOP NAVBAR
    // ══════════════════════════════════════════════════
    QWidget *navbar = new QWidget();
    navbar->setObjectName("navbar");
    navbar->setFixedHeight(56);
    QHBoxLayout *navLayout = new QHBoxLayout(navbar);
    navLayout->setContentsMargins(20, 0, 20, 0);
    navLayout->setSpacing(12);

    // Logo
    QLabel *navLogo = new QLabel("🔗  Connectify");
    navLogo->setObjectName("navLogo");

    // Spacer
    navLayout->addWidget(navLogo);
    navLayout->addStretch();

    // Nav buttons
    QPushButton *searchNavBtn = new QPushButton("🔍");
    searchNavBtn->setObjectName("navIconBtn");
    searchNavBtn->setToolTip("Search");
    searchNavBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *msgNavBtn = new QPushButton("💬");
    msgNavBtn->setObjectName("navIconBtn");
    msgNavBtn->setToolTip("Messages");
    msgNavBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *notifNavBtn = new QPushButton("🔔");
    notifNavBtn->setObjectName("navIconBtn");
    notifNavBtn->setToolTip("Notifications");
    notifNavBtn->setCursor(Qt::PointingHandCursor);

    navLayout->addWidget(searchNavBtn);
    navLayout->addWidget(msgNavBtn);
    navLayout->addWidget(notifNavBtn);
    navLayout->addSpacing(8);

    // User avatar + name
    navAvatarLabel = new QLabel("?");
    navAvatarLabel->setObjectName("navAvatar");
    navAvatarLabel->setFixedSize(34, 34);
    navAvatarLabel->setAlignment(Qt::AlignCenter);
    navAvatarLabel->setCursor(Qt::PointingHandCursor);

    navUserNameLabel = new QLabel("");
    navUserNameLabel->setObjectName("navUserName");
    navUserNameLabel->setCursor(Qt::PointingHandCursor);

    QPushButton *logoutBtn = new QPushButton("Logout");
    logoutBtn->setObjectName("logoutBtn");
    logoutBtn->setCursor(Qt::PointingHandCursor);

    navLayout->addWidget(navAvatarLabel);
    navLayout->addWidget(navUserNameLabel);
    navLayout->addSpacing(6);
    navLayout->addWidget(logoutBtn);

    mainLayout->addWidget(navbar);

    // ══════════════════════════════════════════════════
    //  BODY — 3 columns: left sidebar | feed | right sidebar
    // ══════════════════════════════════════════════════
    QWidget *body = new QWidget();
    body->setObjectName("bodyWidget");
    QHBoxLayout *bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(16, 16, 16, 16);
    bodyLayout->setSpacing(16);

    // ── LEFT SIDEBAR ──────────────────────────────────
    QWidget *leftSidebar = new QWidget();
    leftSidebar->setObjectName("sideCard");
    leftSidebar->setFixedWidth(220);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftSidebar);
    leftLayout->setContentsMargins(14, 16, 14, 16);
    leftLayout->setSpacing(6);

    QLabel *menuTitle = new QLabel("MENU");
    menuTitle->setObjectName("cardTitle");
    leftLayout->addWidget(menuTitle);
    leftLayout->addSpacing(6);

    // Sidebar nav items
    auto makeMenuItem = [&](const QString &icon, const QString &label) -> QPushButton* {
        QPushButton *btn = new QPushButton(icon + "  " + label);
        btn->setObjectName("menuItem");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);
        leftLayout->addWidget(btn);
        return btn;
    };

    QPushButton *homeBtn      = makeMenuItem("🏠", "Home");
    QPushButton *profileBtn   = makeMenuItem("👤", "My Profile");
    QPushButton *friendsBtn   = makeMenuItem("👥", "Friends");
    QPushButton *msgBtn       = makeMenuItem("💬", "Messages");
    QPushButton *searchBtn    = makeMenuItem("🔍", "Search");
    QPushButton *notifBtn     = makeMenuItem("🔔", "Notifications");
    (void)homeBtn; (void)friendsBtn; // suppress unused warning

    leftLayout->addStretch();
    bodyLayout->addWidget(leftSidebar);

    // ── CENTER FEED ───────────────────────────────────
    QWidget *centerCol = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerCol);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(12);

    // ── Create Post Box ───────────────────────────────
    QFrame *createBox = new QFrame();
    createBox->setObjectName("createPostBox");
    QVBoxLayout *createLayout = new QVBoxLayout(createBox);
    createLayout->setContentsMargins(16, 14, 16, 14);
    createLayout->setSpacing(10);

    // Top row: avatar + input
    QHBoxLayout *createTopRow = new QHBoxLayout();
    createTopRow->setSpacing(10);

    QLabel *createAvatar = new QLabel("?");
    createAvatar->setObjectName("miniAvatar");
    createAvatar->setFixedSize(40, 40);
    createAvatar->setAlignment(Qt::AlignCenter);

    postInput = new QTextEdit();
    postInput->setObjectName("postInput");
    postInput->setPlaceholderText("What's on your mind?");
    postInput->setFixedHeight(70);
    postInput->setAcceptRichText(false);

    createTopRow->addWidget(createAvatar);
    createTopRow->addWidget(postInput);
    createLayout->addLayout(createTopRow);

    // Image preview (hidden until image selected)
    imagePreviewLabel = new QLabel();
    imagePreviewLabel->setObjectName("imgPreview");
    imagePreviewLabel->setAlignment(Qt::AlignLeft);
    imagePreviewLabel->hide();
    createLayout->addWidget(imagePreviewLabel);

    // Bottom row: char count + image btn + post btn
    QHBoxLayout *createBtnRow = new QHBoxLayout();
    createBtnRow->setSpacing(8);

    charCountLabel = new QLabel("0 / 280");
    charCountLabel->setObjectName("charCount");

    selectImageBtn = new QPushButton("📷  Add Photo");
    selectImageBtn->setObjectName("secondaryBtn");
    selectImageBtn->setCursor(Qt::PointingHandCursor);
    selectImageBtn->setFixedHeight(36);

    postBtn = new QPushButton("Post");
    postBtn->setObjectName("primaryBtn");
    postBtn->setCursor(Qt::PointingHandCursor);
    postBtn->setFixedHeight(36);
    postBtn->setFixedWidth(80);

    createBtnRow->addWidget(charCountLabel);
    createBtnRow->addStretch();
    createBtnRow->addWidget(selectImageBtn);
    createBtnRow->addWidget(postBtn);
    createLayout->addLayout(createBtnRow);

    centerLayout->addWidget(createBox);

    // ── Feed scroll area ──────────────────────────────
    QScrollArea *feedScroll = new QScrollArea();
    feedScroll->setObjectName("feedScroll");
    feedScroll->setWidgetResizable(true);
    feedScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    feedScroll->verticalScrollBar()->setObjectName("feedScrollBar");

    QWidget *feedContainer = new QWidget();
    feedLayout = new QVBoxLayout(feedContainer);
    feedLayout->setContentsMargins(0, 0, 0, 0);
    feedLayout->setSpacing(12);
    feedLayout->addStretch();

    feedScroll->setWidget(feedContainer);
    centerLayout->addWidget(feedScroll);

    bodyLayout->addWidget(centerCol, 1);

    // ── RIGHT SIDEBAR ─────────────────────────────────
    QWidget *rightSidebar = new QWidget();
    rightSidebar->setObjectName("sideCard");
    rightSidebar->setFixedWidth(220);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightSidebar);
    rightLayout->setContentsMargins(14, 16, 14, 16);
    rightLayout->setSpacing(8);

    QLabel *suggestTitle = new QLabel("SUGGESTED USERS");
    suggestTitle->setObjectName("cardTitle");
    rightLayout->addWidget(suggestTitle);
    rightLayout->addSpacing(4);

    // Placeholder suggestion items
    auto makeSuggestRow = [&](const QString &initials, const QString &name) {
        QWidget *row = new QWidget();
        QHBoxLayout *rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 4, 0, 4);
        rl->setSpacing(10);
        QLabel *av = new QLabel(initials);
        av->setObjectName("miniAvatar");
        av->setFixedSize(34, 34);
        av->setAlignment(Qt::AlignCenter);
        QLabel *nm = new QLabel(name);
        nm->setObjectName("friendName");
        QPushButton *flw = new QPushButton("Follow");
        flw->setObjectName("followBtn");
        flw->setCursor(Qt::PointingHandCursor);
        flw->setFixedHeight(26);
        rl->addWidget(av);
        rl->addWidget(nm);
        rl->addStretch();
        rl->addWidget(flw);
        rightLayout->addWidget(row);
    };

    makeSuggestRow("AK", "Ali Khan");
    makeSuggestRow("SM", "Sara Malik");
    makeSuggestRow("UH", "Usman Haider");

    rightLayout->addStretch();
    bodyLayout->addWidget(rightSidebar);

    mainLayout->addWidget(body, 1);

    // ── Wire up signals ────────────────────────────────
    connect(postBtn,        &QPushButton::clicked, this, &NewsFeedPage::onPostBtnClicked);
    connect(selectImageBtn, &QPushButton::clicked, this, &NewsFeedPage::onSelectImageClicked);
    connect(logoutBtn,      &QPushButton::clicked, this, &NewsFeedPage::logoutClicked);
    connect(profileBtn,     &QPushButton::clicked, this, &NewsFeedPage::goToProfile);
    connect(msgBtn,         &QPushButton::clicked, this, &NewsFeedPage::goToMessages);
    connect(searchBtn,      &QPushButton::clicked, this, &NewsFeedPage::goToSearch);
    connect(notifBtn,       &QPushButton::clicked, this, &NewsFeedPage::goToNotifications);
    connect(msgNavBtn,      &QPushButton::clicked, this, &NewsFeedPage::goToMessages);
    connect(searchNavBtn,   &QPushButton::clicked, this, &NewsFeedPage::goToSearch);
    connect(notifNavBtn,    &QPushButton::clicked, this, &NewsFeedPage::goToNotifications);
    connect(navAvatarLabel, &QLabel::linkActivated, this, &NewsFeedPage::goToProfile);

    // Char counter
    connect(postInput, &QTextEdit::textChanged, this, [this]() {
        int len = postInput->toPlainText().length();
        charCountLabel->setText(QString("%1 / 280").arg(len));
        charCountLabel->setStyleSheet(
            len > 260 ? "color:#f87171; font-size:11px;" : "color:#6b7280; font-size:11px;"
        );
        if (len > 280) {
            QString txt = postInput->toPlainText().left(280);
            postInput->setPlainText(txt);
            QTextCursor c = postInput->textCursor();
            c.movePosition(QTextCursor::End);
            postInput->setTextCursor(c);
        }
    });
}

// ═══════════════════════════════════════════════════════
//  HELPERS
// ═══════════════════════════════════════════════════════
QString NewsFeedPage::makeInitials(const QString &name) {
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString ini;
    for (auto &p : parts) ini += p[0].toUpper();
    return ini.left(2);
}

QString NewsFeedPage::formatTimeAgo(time_t timestamp) {
    time_t now = time(nullptr);
    double diff = difftime(now, timestamp);
    if (diff < 60)         return "just now";
    if (diff < 3600)       return QString("%1m ago").arg((int)(diff/60));
    if (diff < 86400)      return QString("%1h ago").arg((int)(diff/3600));
    return                        QString("%1d ago").arg((int)(diff/86400));
}

// ═══════════════════════════════════════════════════════
//  APPLY STYLES
// ═══════════════════════════════════════════════════════
void NewsFeedPage::applyStyles() {
    setStyleSheet(R"(

        NewsFeedPage, QWidget { background:#0d0d1a; color:#eeeeee;
                                font-family:"Segoe UI",sans-serif; }
        QScrollArea  { border:none; background:#0d0d1a; }

        /* ── Navbar ── */
        #navbar {
            background:#12122a;
            border-bottom:1px solid #1e1e3a;
        }
        #navLogo {
            font-size:18px; font-weight:bold; color:#a78bfa;
            background:transparent;
        }
        #navIconBtn {
            font-size:18px; background:transparent;
            border:none; min-width:36px; min-height:36px;
            border-radius:8px;
        }
        #navIconBtn:hover { background:rgba(124,58,237,0.15); }
        #navAvatar {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 #7c3aed,stop:1 #4c1d95);
            border-radius:17px;
            font-size:13px; font-weight:bold; color:#ffffff;
        }
        #navUserName {
            font-size:13px; font-weight:600; color:#e5e7eb;
            background:transparent;
        }
        #logoutBtn {
            background:transparent; color:#a78bfa;
            border:1.5px solid #7c3aed; border-radius:6px;
            padding:4px 12px; font-size:12px;
        }
        #logoutBtn:hover { background:rgba(124,58,237,0.15); }

        /* ── Body ── */
        #bodyWidget { background:#0d0d1a; }

        /* ── Sidebars ── */
        #sideCard {
            background:#12122a; border:1px solid #1e1e3a;
            border-radius:12px;
        }
        #cardTitle {
            font-size:10px; font-weight:bold; color:#6b7280;
            background:transparent; letter-spacing:1px;
        }
        #menuItem {
            font-size:13px; color:#d1d5db; background:transparent;
            border:none; text-align:left; padding:6px 8px;
            border-radius:8px;
        }
        #menuItem:hover { background:rgba(124,58,237,0.15); color:#a78bfa; }
        #friendName { font-size:12px; font-weight:600;
                      color:#e5e7eb; background:transparent; }
        #followBtn {
            font-size:11px; color:#a78bfa; background:transparent;
            border:1px solid #7c3aed; border-radius:6px; padding:2px 8px;
        }
        #followBtn:hover { background:rgba(124,58,237,0.15); }

        /* ── Create post box ── */
        #createPostBox {
            background:#12122a; border:1px solid #1e1e3a;
            border-radius:12px;
        }
        #postInput {
            background:#1a1a35; border:1.5px solid #2d1b69;
            border-radius:8px; font-size:14px; color:#eeeeee;
            padding:8px;
        }
        #postInput:focus { border:1.5px solid #7c3aed; }
        #charCount { font-size:11px; color:#6b7280; background:transparent; }
        #imgPreview {
            border-radius:8px; max-height:160px;
        }

        /* ── Buttons ── */
        #primaryBtn {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #7c3aed,stop:1 #4c1d95);
            color:#ffffff; font-size:13px; font-weight:bold;
            border:none; border-radius:8px;
        }
        #primaryBtn:hover { background:#6d28d9; }
        #secondaryBtn {
            background:transparent; color:#a78bfa;
            border:1.5px solid #7c3aed; border-radius:8px;
            font-size:12px; padding:0px 12px;
        }
        #secondaryBtn:hover { background:rgba(124,58,237,0.12); }

        /* ── Feed scroll bar ── */
        QScrollBar:vertical {
            background:#0d0d1a; width:6px; border-radius:3px;
        }
        QScrollBar::handle:vertical {
            background:#2d1b69; border-radius:3px; min-height:30px;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height:0px; }

        /* ── Post cards ── */
        #postCard {
            background:#12122a; border:1px solid #1e1e3a;
            border-radius:12px;
        }
        #postCard:hover { border:1px solid #4c1d95; }
        #postTop  { background:transparent; }
        #miniAvatar {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 #7c3aed,stop:1 #4c1d95);
            border-radius:20px;
            font-size:13px; font-weight:bold; color:#ffffff;
        }
        #postName   { font-size:13px; font-weight:600;
                      color:#ffffff; background:transparent; }
        #postHandle { font-size:11px; color:#6b7280; background:transparent; }
        #postContent{ font-size:14px; color:#d1d5db;
                      background:transparent; padding:0 16px 10px 16px; }
        #postImage  { background:#1a0a3d; border-radius:0px; }
        #postFooter { background:transparent;
                      border-top:1px solid #1e1e3a; }
        #likeBtn {
            font-size:13px; color:#a78bfa;
            background:transparent; border:none; padding:2px 8px;
        }
        #likeBtn:hover { color:#7c3aed; }
        #cmtBtn {
            font-size:13px; color:#6b7280;
            background:transparent; border:none; padding:2px 8px;
        }
        #cmtBtn:hover { color:#a78bfa; }
        #likeBtnDisabled, #cmtBtnDisabled {
            font-size:13px; color:#374151;
            background:transparent; border:none; padding:2px 8px;
        }
        #hintLabel { font-size:10px; color:#374151; background:transparent; }

    )");
}
