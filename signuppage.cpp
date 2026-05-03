#include "signuppage.h"

// ═══════════════════════════════════════════════════════
//  CONSTRUCTOR
// ═══════════════════════════════════════════════════════
SignupPage::SignupPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    applyStyles();
}

// ═══════════════════════════════════════════════════════
//  SETUP UI
// ═══════════════════════════════════════════════════════
void SignupPage::setupUI() {
    // ── Outer centering layout ─────────────────────────
    QVBoxLayout *outerV = new QVBoxLayout(this);
    outerV->setContentsMargins(0, 0, 0, 0);
    outerV->setSpacing(0);

    QHBoxLayout *outerH = new QHBoxLayout();
    outerV->addStretch();
    outerV->addLayout(outerH);
    outerV->addStretch();
    outerH->addStretch();

    // ── Card ───────────────────────────────────────────
    QFrame *card = new QFrame();
    card->setObjectName("signupCard");
    card->setFixedWidth(440);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 36, 40, 36);
    cardLayout->setSpacing(0);

    // ── Logo row ───────────────────────────────────────
    QWidget *logoRow = new QWidget();
    logoRow->setObjectName("logoRow");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoRow);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    logoLayout->setSpacing(8);

    QLabel *logoIcon = new QLabel("🔗");
    logoIcon->setObjectName("logoIcon");
    QLabel *logoText = new QLabel("Connectify");
    logoText->setObjectName("logoText");

    logoLayout->addStretch();
    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(logoText);
    logoLayout->addStretch();
    cardLayout->addWidget(logoRow);
    cardLayout->addSpacing(4);

    // ── Tagline ────────────────────────────────────────
    QLabel *tagline = new QLabel("Create your account");
    tagline->setObjectName("tagline");
    tagline->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(tagline);
    cardLayout->addSpacing(24);

    // ── Full Name ──────────────────────────────────────
    fullNameInput = new QLineEdit();
    fullNameInput->setObjectName("authInput");
    fullNameInput->setPlaceholderText("Full Name");
    fullNameInput->setFixedHeight(46);
    cardLayout->addWidget(fullNameInput);
    cardLayout->addSpacing(10);

    // ── Username ───────────────────────────────────────
    usernameInput = new QLineEdit();
    usernameInput->setObjectName("authInput");
    usernameInput->setPlaceholderText("Username");
    usernameInput->setFixedHeight(46);
    cardLayout->addWidget(usernameInput);
    cardLayout->addSpacing(10);

    // ── Email ──────────────────────────────────────────
    emailInput = new QLineEdit();
    emailInput->setObjectName("authInput");
    emailInput->setPlaceholderText("Email Address");
    emailInput->setFixedHeight(46);
    cardLayout->addWidget(emailInput);
    cardLayout->addSpacing(10);

    // ── Password ───────────────────────────────────────
    passwordInput = new QLineEdit();
    passwordInput->setObjectName("authInput");
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedHeight(46);
    cardLayout->addWidget(passwordInput);
    cardLayout->addSpacing(10);

    // ── Confirm Password ───────────────────────────────
    confirmPasswordInput = new QLineEdit();
    confirmPasswordInput->setObjectName("authInput");
    confirmPasswordInput->setPlaceholderText("Confirm Password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    confirmPasswordInput->setFixedHeight(46);
    cardLayout->addWidget(confirmPasswordInput);
    cardLayout->addSpacing(8);

    // ── Error label ────────────────────────────────────
    errorLabel = new QLabel("");
    errorLabel->setObjectName("errorLabel");
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);
    errorLabel->hide();
    cardLayout->addWidget(errorLabel);
    cardLayout->addSpacing(8);

    // ── Sign Up button ─────────────────────────────────
    signupBtn = new QPushButton("Create Account");
    signupBtn->setObjectName("primaryBtn");
    signupBtn->setFixedHeight(48);
    signupBtn->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(signupBtn);
    cardLayout->addSpacing(18);

    // ── Login link ─────────────────────────────────────
    QWidget *linkRow = new QWidget();
    QHBoxLayout *linkLayout = new QHBoxLayout(linkRow);
    linkLayout->setContentsMargins(0, 0, 0, 0);
    linkLayout->setSpacing(4);

    QLabel *linkText = new QLabel("Already have an account?");
    linkText->setObjectName("linkText");

    QPushButton *loginLink = new QPushButton("Log In");
    loginLink->setObjectName("linkBtn");
    loginLink->setCursor(Qt::PointingHandCursor);
    loginLink->setFlat(true);

    linkLayout->addStretch();
    linkLayout->addWidget(linkText);
    linkLayout->addWidget(loginLink);
    linkLayout->addStretch();
    cardLayout->addWidget(linkRow);

    outerH->addWidget(card);
    outerH->addStretch();

    // ── Connect signals ────────────────────────────────
    connect(signupBtn, &QPushButton::clicked, this, [this]() {
        clearError();

        QString fullName = fullNameInput->text().trimmed();
        QString username = usernameInput->text().trimmed();
        QString email    = emailInput->text().trimmed();
        QString password = passwordInput->text();
        QString confirm  = confirmPasswordInput->text();

        // ── Client-side validation ─────────────────────
        if (fullName.isEmpty() || username.isEmpty() ||
            email.isEmpty()    || password.isEmpty()) {
            showError("Please fill in all fields.");
            return;
        }
        if (username.contains(' ')) {
            showError("Username cannot contain spaces.");
            return;
        }
        if (password.length() < 6) {
            showError("Password must be at least 6 characters.");
            return;
        }
        if (password != confirm) {
            showError("Passwords do not match.");
            return;
        }

        emit signupClicked(fullName, username, email, password);
    });

    connect(loginLink, &QPushButton::clicked,
            this, &SignupPage::loginLinkClicked);

    connect(confirmPasswordInput, &QLineEdit::returnPressed,
            signupBtn, &QPushButton::click);
}

// ═══════════════════════════════════════════════════════
//  CLEAR FIELDS
// ═══════════════════════════════════════════════════════
void SignupPage::clearFields() {
    fullNameInput->clear();
    usernameInput->clear();
    emailInput->clear();
    passwordInput->clear();
    confirmPasswordInput->clear();
    clearError();
}

// ═══════════════════════════════════════════════════════
//  ERROR HANDLING
// ═══════════════════════════════════════════════════════
void SignupPage::showError(const QString &message) {
    errorLabel->setText(message);
    errorLabel->show();
}

void SignupPage::clearError() {
    errorLabel->clear();
    errorLabel->hide();
}

// ═══════════════════════════════════════════════════════
//  APPLY STYLES  — matches Connectify dark purple theme
// ═══════════════════════════════════════════════════════
void SignupPage::applyStyles() {
    setStyleSheet(R"(

        SignupPage {
            background: #0d0d1a;
        }
        QWidget {
            background: #0d0d1a;
            color: #eeeeee;
            font-family: "Segoe UI", sans-serif;
        }

        /* ── Card ── */
        #signupCard {
            background: #12122a;
            border: 1.5px solid #2d1b69;
            border-radius: 16px;
        }

        /* ── Logo ── */
        #logoRow {
            background: #1a1a35;
            border-radius: 8px;
            padding: 4px 0px;
        }
        #logoIcon {
            font-size: 22px;
            background: transparent;
        }
        #logoText {
            font-size: 24px;
            font-weight: bold;
            color: #a78bfa;
            background: transparent;
        }

        /* ── Tagline ── */
        #tagline {
            font-size: 13px;
            color: #6b7280;
            background: transparent;
        }

        /* ── Inputs ── */
        #authInput {
            background: #1a1a35;
            border: 1.5px solid #2d1b69;
            border-radius: 8px;
            padding: 0px 14px;
            font-size: 14px;
            color: #eeeeee;
        }
        #authInput:focus {
            border: 1.5px solid #7c3aed;
            background: #1e1e3a;
        }
        #authInput::placeholder {
            color: #4b5563;
        }

        /* ── Error label ── */
        #errorLabel {
            font-size: 12px;
            color: #f87171;
            background: rgba(248, 113, 113, 0.08);
            border: 1px solid rgba(248, 113, 113, 0.25);
            border-radius: 6px;
            padding: 6px 10px;
        }

        /* ── Primary button ── */
        #primaryBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                            stop:0 #7c3aed, stop:1 #4c1d95);
            color: #ffffff;
            font-size: 15px;
            font-weight: bold;
            border: none;
            border-radius: 10px;
        }
        #primaryBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                            stop:0 #6d28d9, stop:1 #3b0764);
        }
        #primaryBtn:pressed {
            background: #5b21b6;
        }

        /* ── Link row ── */
        #linkText {
            font-size: 13px;
            color: #6b7280;
            background: transparent;
        }
        #linkBtn {
            font-size: 13px;
            color: #a78bfa;
            background: transparent;
            border: none;
            font-weight: 600;
        }
        #linkBtn:hover {
            color: #7c3aed;
        }

    )");
}
