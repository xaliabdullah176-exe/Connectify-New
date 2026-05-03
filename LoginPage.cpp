#include "LoginPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {

    QFrame* card = new QFrame();
    card->setObjectName("card");
    card->setFixedSize(400, 450);

    QLabel* title = new QLabel("🔗 Connectify");
    title->setObjectName("title");
    title->setAlignment(Qt::AlignCenter);

    QLabel* subtitle = new QLabel("Connect with the world");
    subtitle->setObjectName("subtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    usernameEdit_ = new QLineEdit();
    usernameEdit_->setPlaceholderText("Username");

    passwordEdit_ = new QLineEdit();
    passwordEdit_->setPlaceholderText("Password");
    passwordEdit_->setEchoMode(QLineEdit::Password);

    errorLabel_ = new QLabel("");
    errorLabel_->setObjectName("errorLabel");
    errorLabel_->setAlignment(Qt::AlignCenter);

    loginBtn_ = new QPushButton("Login");
    loginBtn_->setObjectName("primaryBtn");

    signupBtn_ = new QPushButton("Don't have an account? Sign Up");
    signupBtn_->setObjectName("linkBtn");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(12);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(usernameEdit_);
    cardLayout->addWidget(passwordEdit_);
    cardLayout->addWidget(errorLabel_);
    cardLayout->addWidget(loginBtn_);
    cardLayout->addWidget(signupBtn_);

    QHBoxLayout* outer = new QHBoxLayout(this);
    outer->addStretch();
    outer->addWidget(card, 0, Qt::AlignVCenter);
    outer->addStretch();

    connect(loginBtn_,     &QPushButton::clicked,     this, &LoginPage::onLoginClicked);
    connect(signupBtn_,    &QPushButton::clicked,     this, &LoginPage::signupLinkClicked);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
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
    errorLabel_->clear();
}