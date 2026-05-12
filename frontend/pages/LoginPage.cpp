#include "LoginPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {

    QFrame* card = new QFrame();
    card->setObjectName("card");
    card->setFixedSize(400, 450);

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

    QLabel* subtitle = new QLabel("Log in to your account");
    subtitle->setObjectName("tagline");
    subtitle->setAlignment(Qt::AlignCenter);

    usernameEdit_ = new QLineEdit();
    usernameEdit_->setObjectName("authInput");
    usernameEdit_->setPlaceholderText("Username");
    usernameEdit_->setFixedHeight(46);

    passwordEdit_ = new QLineEdit();
    passwordEdit_->setObjectName("authInput");
    passwordEdit_->setPlaceholderText("Password");
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setFixedHeight(46);

    showPasswordCheck_ = new QCheckBox("Show password");
    showPasswordCheck_->setObjectName("showPassCheck");

    errorLabel_ = new QLabel("");
    errorLabel_->setObjectName("errorLabel");
    errorLabel_->setAlignment(Qt::AlignCenter);
    errorLabel_->setWordWrap(true);
    errorLabel_->hide();

    loginBtn_ = new QPushButton("Log In");
    loginBtn_->setObjectName("primaryBtn");
    loginBtn_->setFixedHeight(48);
    loginBtn_->setCursor(Qt::PointingHandCursor);

    // ── Link row ─────────────────────────────────────
    QWidget *linkRow = new QWidget();
    QHBoxLayout *linkLayout = new QHBoxLayout(linkRow);
    linkLayout->setContentsMargins(0, 0, 0, 0);
    linkLayout->setSpacing(4);

    QLabel *linkText = new QLabel("Don't have an account?");
    linkText->setObjectName("linkText");

    signupBtn_ = new QPushButton("Sign Up");
    signupBtn_->setObjectName("linkBtn");
    signupBtn_->setCursor(Qt::PointingHandCursor);
    signupBtn_->setFlat(true);

    linkLayout->addStretch();
    linkLayout->addWidget(linkText);
    linkLayout->addWidget(signupBtn_);
    linkLayout->addStretch();

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(0);
    cardLayout->setContentsMargins(40, 36, 40, 36);
    cardLayout->addWidget(logoRow);
    cardLayout->addSpacing(4);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(24);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(usernameEdit_);
    cardLayout->addWidget(passwordEdit_);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(showPasswordCheck_);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(errorLabel_);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(loginBtn_);
    cardLayout->addSpacing(18);
    cardLayout->addWidget(linkRow);

    QHBoxLayout* outer = new QHBoxLayout(this);
    outer->addStretch();
    outer->addWidget(card, 0, Qt::AlignVCenter);
    outer->addStretch();

    connect(loginBtn_,     &QPushButton::clicked,     this, &LoginPage::onLoginClicked);
    connect(signupBtn_,    &QPushButton::clicked,     this, &LoginPage::signupLinkClicked);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
    connect(showPasswordCheck_, &QCheckBox::toggled, this, [this](bool checked) {
        passwordEdit_->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    applyStyles();
}

void LoginPage::onLoginClicked() {
    QString username = usernameEdit_->text().trimmed();
    QString password = passwordEdit_->text();

    if (username.isEmpty() || password.isEmpty()) {
        errorLabel_->setText("Please fill all fields!");
        return;
    }
    emit loginClicked(username, password);
}

void LoginPage::showError(const QString& msg) {
    errorLabel_->setText(msg);
}

void LoginPage::clearFields() {
    usernameEdit_->clear();
    passwordEdit_->clear();
    showPasswordCheck_->setChecked(false);
    errorLabel_->clear();
    errorLabel_->hide();
}

void LoginPage::applyStyles() {
    setStyleSheet(R"(

        LoginPage {
            background: #0d0d1a;
        }
        QWidget {
            background: #0d0d1a;
            color: #eeeeee;
            font-family: "Segoe UI", sans-serif;
        }

        /* ── Card ── */
        #card {
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

        #showPassCheck {
            font-size: 12px;
            color: #9ca3af;
            background: transparent;
            padding-left: 2px;
        }

    )");
}