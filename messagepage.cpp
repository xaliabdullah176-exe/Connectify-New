#include "messagepage.h"
#include <QStyle>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

// ═══════════════════════════════════════════════════════
//  MESSAGE BUBBLE
// ═══════════════════════════════════════════════════════
MessageBubble::MessageBubble(const QString &text,
                             const QString &time,
                             bool isSent,
                             QWidget *parent)
    : QFrame(parent)
{
    setObjectName(isSent ? "bubbleSent" : "bubbleReceived");

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->setContentsMargins(10, 7, 10, 7);
    vl->setSpacing(3);

    QLabel *textLbl = new QLabel(text);
    textLbl->setObjectName(isSent ? "bubbleTextSent" : "bubbleTextReceived");
    textLbl->setWordWrap(true);
    textLbl->setMaximumWidth(340);

    QLabel *timeLbl = new QLabel(time);
    timeLbl->setObjectName("bubbleTime");
    timeLbl->setAlignment(isSent ? Qt::AlignRight : Qt::AlignLeft);

    vl->addWidget(textLbl);
    vl->addWidget(timeLbl);
}

// ═══════════════════════════════════════════════════════
//  CHAT LIST ITEM
// ═══════════════════════════════════════════════════════
ChatListItem::ChatListItem(int userID,
                           const QString &name,
                           const QString &lastMessage,
                           const QString &time,
                           bool isOnline,
                           QWidget *parent)
    : QFrame(parent), m_userID(userID)
{
    setObjectName("chatItem");
    setCursor(Qt::PointingHandCursor);
    setFixedHeight(64);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(12, 8, 12, 8);
    hl->setSpacing(10);

    // Avatar with online dot
    QWidget *avatarWrapper = new QWidget();
    avatarWrapper->setFixedSize(40, 40);
    QLabel *avatar = new QLabel(avatarWrapper);
    avatar->setObjectName("chatAvatar");
    avatar->setFixedSize(40, 40);
    avatar->setAlignment(Qt::AlignCenter);

    // Initials
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    avatar->setText(initials.left(2));

    // Online indicator
    if (isOnline) {
        QLabel *dot = new QLabel(avatarWrapper);
        dot->setObjectName("onlineDot");
        dot->setFixedSize(10, 10);
        dot->move(30, 30);
    }

    hl->addWidget(avatarWrapper);

    // Name + last message
    QVBoxLayout *textVl = new QVBoxLayout();
    textVl->setSpacing(2);
    textVl->setContentsMargins(0,0,0,0);

    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0,0,0,0);

    QLabel *nameLbl = new QLabel(name);
    nameLbl->setObjectName("chatItemName");

    QLabel *timeLbl = new QLabel(time);
    timeLbl->setObjectName("chatItemTime");

    topRow->addWidget(nameLbl);
    topRow->addStretch();
    topRow->addWidget(timeLbl);

    QLabel *msgLbl = new QLabel(lastMessage);
    msgLbl->setObjectName("chatItemMsg");
    msgLbl->setMaximumWidth(160);

    // Elide long messages
    QFontMetrics fm(msgLbl->font());
    msgLbl->setText(fm.elidedText(lastMessage, Qt::ElideRight, 160));

    textVl->addLayout(topRow);
    textVl->addWidget(msgLbl);

    hl->addLayout(textVl);
}

void ChatListItem::setActive(bool active) {
    m_active = active;
    setProperty("active", active);
    style()->unpolish(this);
    style()->polish(this);
}

void ChatListItem::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    emit clicked(m_userID);
}

// ═══════════════════════════════════════════════════════
//  MESSAGE PAGE — CONSTRUCTOR
// ═══════════════════════════════════════════════════════
MessagePage::MessagePage(QWidget *parent) : QWidget(parent) {
    setupUI();
    applyStyles();
}

// ═══════════════════════════════════════════════════════
//  LOAD CURRENT USER
// ═══════════════════════════════════════════════════════
void MessagePage::loadCurrentUser(int userID, const QString &name) {
    m_currentUserID   = userID;
    m_currentUserName = name;
}

// ═══════════════════════════════════════════════════════
//  ADD CONTACT
// ═══════════════════════════════════════════════════════
void MessagePage::addContact(int userID, const QString &name,
                              const QString &lastMessage,
                              const QString &time, bool isOnline)
{
    ChatListItem *item = new ChatListItem(
        userID, name, lastMessage, time, isOnline);

    connect(item, &ChatListItem::clicked,
            this, &MessagePage::onContactClicked);

    // Insert before the stretch at the end
    chatListLayout->insertWidget(chatListLayout->count() - 1, item);
    chatItems.append(item);
}

// ═══════════════════════════════════════════════════════
//  ADD MESSAGE BUBBLE
// ═══════════════════════════════════════════════════════
void MessagePage::addMessage(const QString &text,
                              const QString &time, bool isSent)
{
    MessageBubble *bubble = new MessageBubble(text, time, isSent);

    QHBoxLayout *row = new QHBoxLayout();
    row->setContentsMargins(12, 2, 12, 2);

    if (isSent) {
        row->addStretch();
        row->addWidget(bubble);
    } else {
        row->addWidget(bubble);
        row->addStretch();
    }

    // Insert before the bottom stretch
    QWidget *rowWidget = new QWidget();
    rowWidget->setLayout(row);

    messagesLayout->insertWidget(messagesLayout->count() - 1, rowWidget);
    scrollToBottom();
}

// ═══════════════════════════════════════════════════════
//  CLEAR MESSAGES
// ═══════════════════════════════════════════════════════
void MessagePage::clearMessages() {
    QLayoutItem *item;
    while ((item = messagesLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    messagesLayout->addStretch();
}

// ═══════════════════════════════════════════════════════
//  OPEN CONVERSATION
// ═══════════════════════════════════════════════════════
void MessagePage::openConversation(int userID,
                                    const QString &contactName,
                                    bool isOnline)
{
    m_openChatUserID = userID;

    chatNameLabel->setText(contactName);
    chatStatusLabel->setText(isOnline ? "● Online" : "○ Offline");
    chatStatusLabel->setObjectName(isOnline ? "statusOnline" : "statusOffline");
    chatStatusLabel->style()->unpolish(chatStatusLabel);
    chatStatusLabel->style()->polish(chatStatusLabel);

    chatAvatarLabel->setText(makeInitials(contactName));

    placeholderWidget->hide();
    conversationWidget->show();

    messageInput->setFocus();
}

// ═══════════════════════════════════════════════════════
//  SLOTS
// ═══════════════════════════════════════════════════════
void MessagePage::onSendClicked() {
    QString text = messageInput->text().trimmed();
    if (text.isEmpty() || m_openChatUserID == -1) return;

    QString time = QDateTime::currentDateTime().toString("hh:mm AP");
    addMessage(text, time, true);
    messageInput->clear();

    emit sendMessageClicked(m_openChatUserID, text);
}

void MessagePage::onContactClicked(int userID) {
    // Deactivate all, activate clicked
    for (auto *item : chatItems) {
        item->setActive(item->getUserID() == userID);
    }
    emit contactSelected(userID);
}

// ═══════════════════════════════════════════════════════
//  HELPERS
// ═══════════════════════════════════════════════════════
void MessagePage::scrollToBottom() {
    QTimer::singleShot(50, this, [this]() {
        messagesScrollArea->verticalScrollBar()->setValue(
            messagesScrollArea->verticalScrollBar()->maximum()
        );
    });
}

QString MessagePage::makeInitials(const QString &name) {
    QStringList parts = name.split(" ", Qt::SkipEmptyParts);
    QString initials;
    for (auto &p : parts) initials += p[0].toUpper();
    return initials.left(2);
}

// ═══════════════════════════════════════════════════════
//  SETUP UI
// ═══════════════════════════════════════════════════════
void MessagePage::setupUI() {

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ════════════════════════════════════════════════════
    //  LEFT PANEL — chat list
    // ════════════════════════════════════════════════════
    QFrame *leftPanel = new QFrame();
    leftPanel->setObjectName("leftPanel");
    leftPanel->setFixedWidth(280);

    QVBoxLayout *leftVl = new QVBoxLayout(leftPanel);
    leftVl->setContentsMargins(0, 0, 0, 0);
    leftVl->setSpacing(0);

    // ── Left topbar ───────────────────────────────────
    QFrame *leftTop = new QFrame();
    leftTop->setObjectName("leftTop");
    leftTop->setFixedHeight(60);

    QHBoxLayout *ltHl = new QHBoxLayout(leftTop);
    ltHl->setContentsMargins(14, 0, 14, 0);

    QLabel *logoText = new QLabel("⚡ CONNECTIFY");
    logoText->setObjectName("logoText");

    QPushButton *backBtn = new QPushButton("←");
    backBtn->setObjectName("backBtn");
    backBtn->setFixedSize(32, 32);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked,
            this, &MessagePage::backClicked);

    ltHl->addWidget(logoText);
    ltHl->addStretch();
    ltHl->addWidget(backBtn);
    leftVl->addWidget(leftTop);

    // ── Messages header ────────────────────────────────
    QWidget *listHeader = new QWidget();
    listHeader->setObjectName("listHeader");
    QVBoxLayout *lhVl = new QVBoxLayout(listHeader);
    lhVl->setContentsMargins(14, 12, 14, 8);
    lhVl->setSpacing(10);

    QLabel *msgTitle = new QLabel("Messages");
    msgTitle->setObjectName("msgTitle");

    // Search box
    searchBox = new QLineEdit();
    searchBox->setObjectName("searchBox");
    searchBox->setPlaceholderText("🔍  Search conversations...");
    searchBox->setFixedHeight(34);

    lhVl->addWidget(msgTitle);
    lhVl->addWidget(searchBox);
    leftVl->addWidget(listHeader);

    // ── Scrollable chat list ───────────────────────────
    QScrollArea *listScroll = new QScrollArea();
    listScroll->setWidgetResizable(true);
    listScroll->setFrameShape(QFrame::NoFrame);
    listScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *listInner = new QWidget();
    chatListLayout = new QVBoxLayout(listInner);
    chatListLayout->setContentsMargins(0, 4, 0, 4);
    chatListLayout->setSpacing(0);
    chatListLayout->addStretch();

    listScroll->setWidget(listInner);
    leftVl->addWidget(listScroll);

    root->addWidget(leftPanel);

    // ════════════════════════════════════════════════════
    //  RIGHT PANEL — conversation
    // ════════════════════════════════════════════════════
    QWidget *rightPanel = new QWidget();
    rightPanel->setObjectName("rightPanel");

    QVBoxLayout *rightVl = new QVBoxLayout(rightPanel);
    rightVl->setContentsMargins(0, 0, 0, 0);
    rightVl->setSpacing(0);

    // ── Placeholder (no chat selected) ────────────────
    placeholderWidget = new QWidget();
    placeholderWidget->setObjectName("placeholder");

    QVBoxLayout *phVl = new QVBoxLayout(placeholderWidget);
    phVl->setAlignment(Qt::AlignCenter);

    QLabel *phIcon = new QLabel("💬");
    phIcon->setObjectName("phIcon");
    phIcon->setAlignment(Qt::AlignCenter);

    QLabel *phTitle = new QLabel("Your Messages");
    phTitle->setObjectName("phTitle");
    phTitle->setAlignment(Qt::AlignCenter);

    QLabel *phSub = new QLabel("Select a conversation from the left\nto start messaging");
    phSub->setObjectName("phSub");
    phSub->setAlignment(Qt::AlignCenter);

    phVl->addWidget(phIcon);
    phVl->addSpacing(12);
    phVl->addWidget(phTitle);
    phVl->addSpacing(6);
    phVl->addWidget(phSub);

    rightVl->addWidget(placeholderWidget);

    // ── Conversation widget ────────────────────────────
    conversationWidget = new QWidget();
    conversationWidget->setObjectName("conversationWidget");
    conversationWidget->hide();

    QVBoxLayout *convVl = new QVBoxLayout(conversationWidget);
    convVl->setContentsMargins(0, 0, 0, 0);
    convVl->setSpacing(0);

    // Chat topbar
    QFrame *chatTopbar = new QFrame();
    chatTopbar->setObjectName("chatTopbar");
    chatTopbar->setFixedHeight(60);

    QHBoxLayout *ctHl = new QHBoxLayout(chatTopbar);
    ctHl->setContentsMargins(16, 0, 16, 0);
    ctHl->setSpacing(10);

    chatAvatarLabel = new QLabel("??");
    chatAvatarLabel->setObjectName("chatTopAvatar");
    chatAvatarLabel->setFixedSize(38, 38);
    chatAvatarLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *chatInfoVl = new QVBoxLayout();
    chatInfoVl->setSpacing(2);
    chatInfoVl->setContentsMargins(0,0,0,0);

    chatNameLabel   = new QLabel("Contact");
    chatNameLabel->setObjectName("chatTopName");

    chatStatusLabel = new QLabel("● Online");
    chatStatusLabel->setObjectName("statusOnline");

    chatInfoVl->addWidget(chatNameLabel);
    chatInfoVl->addWidget(chatStatusLabel);

    ctHl->addWidget(chatAvatarLabel);
    ctHl->addLayout(chatInfoVl);
    ctHl->addStretch();

    convVl->addWidget(chatTopbar);

    // Messages scroll area
    messagesScrollArea = new QScrollArea();
    messagesScrollArea->setWidgetResizable(true);
    messagesScrollArea->setFrameShape(QFrame::NoFrame);
    messagesScrollArea->setObjectName("messagesArea");

    QWidget *messagesInner = new QWidget();
    messagesInner->setObjectName("messagesInner");

    messagesLayout = new QVBoxLayout(messagesInner);
    messagesLayout->setContentsMargins(0, 12, 0, 12);
    messagesLayout->setSpacing(4);
    messagesLayout->addStretch();

    messagesScrollArea->setWidget(messagesInner);
    convVl->addWidget(messagesScrollArea);

    // Input bar
    QFrame *inputBar = new QFrame();
    inputBar->setObjectName("inputBar");
    inputBar->setFixedHeight(64);

    QHBoxLayout *ibHl = new QHBoxLayout(inputBar);
    ibHl->setContentsMargins(14, 10, 14, 10);
    ibHl->setSpacing(10);

    messageInput = new QLineEdit();
    messageInput->setObjectName("messageInput");
    messageInput->setPlaceholderText("Type a message...");
    messageInput->setFixedHeight(40);

    sendBtn = new QPushButton("Send  ➤");
    sendBtn->setObjectName("sendBtn");
    sendBtn->setFixedHeight(40);
    sendBtn->setFixedWidth(90);
    sendBtn->setCursor(Qt::PointingHandCursor);

    connect(sendBtn, &QPushButton::clicked,
            this, &MessagePage::onSendClicked);
    connect(messageInput, &QLineEdit::returnPressed,
            this, &MessagePage::onSendClicked);

    ibHl->addWidget(messageInput);
    ibHl->addWidget(sendBtn);
    convVl->addWidget(inputBar);

    rightVl->addWidget(conversationWidget);
    root->addWidget(rightPanel);
}

// ═══════════════════════════════════════════════════════
//  STYLESHEET
// ═══════════════════════════════════════════════════════
void MessagePage::applyStyles() {
    setStyleSheet(R"(

        /* ── Base ─────────────────────────────────────── */
        QWidget        { background:#0d0d1a; color:#eeeeee;
                         font-family:'Segoe UI'; }
        QScrollArea    { border:none; background:#0d0d1a; }
        QScrollBar:vertical { background:#0d0d1a; width:4px; }
        QScrollBar::handle:vertical { background:#2d1b69;
                         border-radius:2px; min-height:20px; }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height:0; }

        /* ── Left panel ───────────────────────────────── */
        #leftPanel     { background:#0a0a15;
                         border-right:1px solid #1e1e3a; }
        #leftTop       { background:#0a0a15;
                         border-bottom:1px solid #1e1e3a; }
        #logoText      { font-size:14px; font-weight:800;
                         color:#a78bfa; letter-spacing:1px; }
        #backBtn       { background:rgba(124,58,237,0.15);
                         border:1px solid #2d1b69; border-radius:8px;
                         font-size:16px; color:#a78bfa; }
        #backBtn:hover { background:rgba(124,58,237,0.3); }

        #listHeader    { background:#0a0a15;
                         border-bottom:1px solid #1e1e3a; }
        #msgTitle      { font-size:16px; font-weight:700; color:#ffffff; }

        #searchBox     { background:#12122a;
                         border:1px solid #1e1e3a; border-radius:10px;
                         padding:0 12px; font-size:12px; color:#eee; }
        #searchBox:focus { border:1px solid #7c3aed; }

        /* ── Chat list items ─────────────────────────── */
        #chatItem      { background:#0a0a15;
                         border-bottom:1px solid #12122a; }
        #chatItem:hover { background:#12122a; }
        #chatItem[active="true"] {
                         background:#1a1a35;
                         border-left:3px solid #7c3aed; }

        #chatAvatar    { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #4c1d95);
                         color:white; border-radius:20px;
                         font-size:12px; font-weight:bold; }
        #onlineDot     { background:#34d399; border-radius:5px;
                         border:2px solid #0a0a15; }

        #chatItemName  { font-size:13px; font-weight:600; color:#e5e7eb; }
        #chatItemTime  { font-size:10px; color:#6b7280; }
        #chatItemMsg   { font-size:11px; color:#6b7280; }

        /* ── Right panel ─────────────────────────────── */
        #rightPanel    { background:#0d0d1a; }

        /* Placeholder */
        #placeholder   { background:#0d0d1a; }
        #phIcon        { font-size:52px; }
        #phTitle       { font-size:18px; font-weight:700; color:#ffffff; }
        #phSub         { font-size:13px; color:#6b7280;
                         line-height:1.8; }

        /* Chat topbar */
        #chatTopbar    { background:#0a0a15;
                         border-bottom:1px solid #1e1e3a; }
        #chatTopAvatar { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #4c1d95);
                         color:white; border-radius:19px;
                         font-size:12px; font-weight:bold; }
        #chatTopName   { font-size:14px; font-weight:700; color:#ffffff; }
        #statusOnline  { font-size:11px; color:#34d399; }
        #statusOffline { font-size:11px; color:#6b7280; }

        /* Messages area */
        #messagesArea  { background:#0d0d1a; }
        #messagesInner { background:#0d0d1a; }

        /* Bubbles */
        #bubbleSent    { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #5b21b6);
                         border-radius:16px 16px 4px 16px;
                         max-width:340px; }
        #bubbleReceived { background:#1a1a35;
                          border:1px solid #2d2d5a;
                          border-radius:16px 16px 16px 4px;
                          max-width:340px; }
        #bubbleTextSent     { font-size:13px; color:#ffffff; }
        #bubbleTextReceived { font-size:13px; color:#e5e7eb; }
        #bubbleTime         { font-size:9px; color:rgba(255,255,255,0.45); }

        /* Input bar */
        #inputBar      { background:#0a0a15;
                         border-top:1px solid #1e1e3a; }
        #messageInput  { background:#12122a;
                         border:1px solid #1e1e3a; border-radius:20px;
                         padding:0 16px; font-size:13px; color:#eee; }
        #messageInput:focus { border:1px solid #7c3aed; }
        #sendBtn       { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #7c3aed, stop:1 #5b21b6);
                         color:white; border:none; border-radius:20px;
                         font-size:12px; font-weight:700; }
        #sendBtn:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                             stop:0 #8b5cf6, stop:1 #6d28d9); }
        #sendBtn:pressed { background:#4c1d95; }

        /* Conversation widget bg */
        #conversationWidget { background:#0d0d1a; }
    )");
}
