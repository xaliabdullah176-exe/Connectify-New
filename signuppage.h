#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class SignupPage : public QWidget {
    Q_OBJECT

public:
    explicit SignupPage(QWidget *parent = nullptr);

    void clearFields();
    void showError(const QString &message);
    void clearError();

signals:
    // Emitted when user clicks Sign Up
    // Connect this to: signup(nextID++, username, password, "user")
    void signupClicked(const QString &fullName,
                       const QString &username,
                       const QString &email,
                       const QString &password);

    void loginLinkClicked();   // navigate back to LoginPage

private:
    QLineEdit   *fullNameInput;
    QLineEdit   *usernameInput;
    QLineEdit   *emailInput;
    QLineEdit   *passwordInput;
    QLineEdit   *confirmPasswordInput;
    QPushButton *signupBtn;
    QLabel      *errorLabel;

    void setupUI();
    void applyStyles();
};
