#include "notificationpage.h"
#include "user.h"
#include <QDateTime>

extern NotificationSystem notifSystem;

// ═══════════════════════════════════════════════════════
//  SINGLE NOTIF ROW
// ═══════════════════════════════════════════════════════
NotifRow::NotifRow(const QString &message, const QString &time, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("notifRow");
    setFixedHeight(70);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(16, 10, 16, 10);
    hl->setSpacing(14);

    // Bell icon
    QLabel *icon = new QLabel("🔔");
    icon->setObjectName("notifIcon");
    icon->setFixedSize(36, 36);
    icon->setAlignment(Qt::AlignCenter);

    // Text + time
    QVBoxLayout *textVl = new QVBoxLayout();
    textVl->setSpacing(4);
    textVl->setContentsMargins(0, 0, 0, 0);

    QLabel *msgLbl = new QLabel(message);
    msgLbl->setObjectName("notifMsg");
    msgLbl->setWordWrap(true);

    QLabel *timeLbl = new QLabel(time);
    timeLbl->setObjectName("notifTime");

    textVl->addWidget(msgLbl);
    textVl->addWidget(timeLbl);

    hl->addWidget(icon);
    hl->addLayout(textVl);
}

// ═══════════════════════════════════════════════════════
//  FRIEND REQUEST ROW
// ═══════════════════════════════════════════════════════
FriendRequestNotifRow::FriendRequestNotifRow(const QString &message, const QString &time,
                                             int fromUserId, QWidget *parent)
    : QFrame(parent), m_fromUserId(fromUserId)
{
    setObjectName("friendReqRow");
    setMinimumHeight(108);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(16, 10, 16, 10);
    hl->setSpacing(14);

    QLabel *icon = new QLabel("👤");
    icon->setObjectName("notifIcon");
    icon->setFixedSize(36, 36);
    icon->setAlignment(Qt::AlignCenter);

    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(8);
    rightCol->setContentsMargins(0, 0, 0, 0);

    QLabel *msgLbl = new QLabel(message);
    msgLbl->setObjectName("notifMsg");
    msgLbl->setWordWrap(true);

    QLabel *timeLbl = new QLabel(time);
    timeLbl->setObjectName("notifTime");

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);
    QPushButton *acceptBtn = new QPushButton("Accept");
    acceptBtn->setObjectName("frAcceptBtn");
    acceptBtn->setFixedHeight(32);
    acceptBtn->setCursor(Qt::PointingHandCursor);
    QPushButton *declineBtn = new QPushButton("Decline");
    declineBtn->setObjectName("frDeclineBtn");
    declineBtn->setFixedHeight(32);
    declineBtn->setCursor(Qt::PointingHandCursor);
    btnRow->addWidget(acceptBtn);
    btnRow->addWidget(declineBtn);
    btnRow->addStretch();

    rightCol->addWidget(msgLbl);
    rightCol->addWidget(timeLbl);
    rightCol->addLayout(btnRow);

    hl->addWidget(icon);
    hl->addLayout(rightCol);

    connect(acceptBtn, &QPushButton::clicked, this, [acceptBtn, declineBtn, this]()
            {
        acceptBtn->setEnabled(false);
        declineBtn->setEnabled(false);
        emit accepted(m_fromUserId); });
    connect(declineBtn, &QPushButton::clicked, this, [acceptBtn, declineBtn, this]()
            {
        acceptBtn->setEnabled(false);
        declineBtn->setEnabled(false);
        emit declined(m_fromUserId); });
}

// ═══════════════════════════════════════════════════════
//  NOTIFICATION PAGE
// ═══════════════════════════════════════════════════════
NotificationPage::NotificationPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void NotificationPage::clearNotifications()
{
    // Remove only dynamic notification rows; keep persistent emptyLabel + stretch.
    for (int i = listLayout->count() - 1; i >= 0; --i)
    {
        QLayoutItem *item = listLayout->itemAt(i);
        if (!item || !item->widget())
            continue;

        QWidget *w = item->widget();
        if (w == emptyLabel)
            continue;

        listLayout->removeWidget(w);
        w->deleteLater();
    }
    emptyLabel->show();
}

void NotificationPage::loadNotifications(int userID)
{
    clearNotifications();

    int count = 0;
    // Iterate in reverse to show newest first
    for (int i = notifSystem.getCount() - 1; i >= 0; i--)
    {
        auto *n = notifSystem.getAt(i);
        if (n->targetUserID != userID)
            continue;

        QString timeStr = "";
        if (n->timestamp > 0)
        {
            QDateTime dt = QDateTime::fromSecsSinceEpoch(n->timestamp);
            timeStr = dt.toString("dd MMM, hh:mm AP");
        }

        const QString msg = QString::fromStdString(n->message);
        if (n->kind == NotifFriendRequest && n->relatedUserID >= 0)
        {
            FriendRequestNotifRow *fr = new FriendRequestNotifRow(msg, timeStr, n->relatedUserID);
            connect(fr, &FriendRequestNotifRow::accepted, this, &NotificationPage::friendRequestAccepted);
            connect(fr, &FriendRequestNotifRow::declined, this, &NotificationPage::friendRequestDeclined);
            listLayout->insertWidget(listLayout->count() - 1, fr);
        }
        else
        {
            NotifRow *row = new NotifRow(msg, timeStr);
            listLayout->insertWidget(listLayout->count() - 1, row);
        }
        count++;
    }

    if (count > 0)
        emptyLabel->hide();
    else
        emptyLabel->show();
}

void NotificationPage::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Top bar ───────────────────────────────────────
    QFrame *topBar = new QFrame();
    topBar->setObjectName("notifTopBar");
    topBar->setFixedHeight(60);

    QHBoxLayout *tbHl = new QHBoxLayout(topBar);
    tbHl->setContentsMargins(20, 0, 20, 0);

    QLabel *title = new QLabel("🔔  Notifications");
    title->setObjectName("notifTitle");

    QPushButton *backBtn = new QPushButton("← Back");
    backBtn->setObjectName("backBtn");
    backBtn->setFixedHeight(34);
    backBtn->setFixedWidth(90);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &NotificationPage::backClicked);

    tbHl->addWidget(title);
    tbHl->addStretch();
    tbHl->addWidget(backBtn);

    root->addWidget(topBar);

    // ── Scroll area ───────────────────────────────────
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("notifScroll");

    QWidget *inner = new QWidget();
    inner->setObjectName("notifInner");

    listLayout = new QVBoxLayout(inner);
    listLayout->setContentsMargins(20, 16, 20, 16);
    listLayout->setSpacing(10);

    emptyLabel = new QLabel("No notifications yet.\nAdd friends, like posts, and send messages to see activity here.");
    emptyLabel->setObjectName("emptyLabel");
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setWordWrap(true);
    listLayout->addWidget(emptyLabel);
    listLayout->addStretch();

    scroll->setWidget(inner);
    root->addWidget(scroll);
}

void NotificationPage::applyStyles()
{
    setStyleSheet(R"(
        QWidget        { background:#0d0d1a; color:#eeeeee;
                         font-family:'Segoe UI'; }

        #notifTopBar   { background:#0a0a15;
                         border-bottom: 1px solid #1e1e3a; }
        #notifTitle    { font-size:18px; font-weight:800; color:#a78bfa; }

        #backBtn       { background:rgba(124,58,237,0.15);
                         border:1px solid #2d1b69; border-radius:8px;
                         color:#a78bfa; font-size:13px; }
        #backBtn:hover { background:rgba(124,58,237,0.3); }

        #notifScroll   { background:#0d0d1a; border:none; }
        #notifInner    { background:#0d0d1a; }

        #notifRow      { background:#12122a;
                         border:1px solid #1e1e3a;
                         border-radius:12px; }
        #notifRow:hover { border:1px solid #7c3aed; }

        #friendReqRow   { background:#12122a;
                         border:1px solid #1e1e3a;
                         border-radius:12px; }
        #friendReqRow:hover { border:1px solid #7c3aed; }

        #frAcceptBtn   { background:rgba(34,197,94,0.25);
                         border:1px solid #166534; border-radius:8px;
                         color:#86efac; font-size:13px; font-weight:600;
                         padding:0 16px; min-width:72px; }
        #frAcceptBtn:hover { background:rgba(34,197,94,0.4); }

        #frDeclineBtn  { background:rgba(239,68,68,0.15);
                         border:1px solid #7f1d1d; border-radius:8px;
                         color:#fca5a5; font-size:13px; font-weight:600;
                         padding:0 16px; min-width:72px; }
        #frDeclineBtn:hover { background:rgba(239,68,68,0.28); }

        #notifIcon     { background:rgba(124,58,237,0.2);
                         border-radius:18px; font-size:16px; }
        #notifMsg      { font-size:13px; color:#e5e7eb; font-weight:500; }
        #notifTime     { font-size:11px; color:#6b7280; }

        #emptyLabel    { font-size:14px; color:#4b5563;
                         padding:60px 20px; line-height:1.8; }

        QScrollBar:vertical { background:#0d0d1a; width:4px; }
        QScrollBar::handle:vertical { background:#2d1b69; border-radius:2px; min-height:20px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
    )");
}
