#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>

class LoginPage : public QWidget {
    Q_OBJECT

public:
    explicit LoginPage(QWidget* parent = nullptr);
    void showError(const QString& msg);
    void clearFields();

signals:
    void loginClicked(const QString& username, const QString& password);
    void signupLinkClicked();

private slots:
    void onLoginClicked();

private:
    void applyStyles();

    QLineEdit*   usernameEdit_;
    QLineEdit*   passwordEdit_;
    QCheckBox*   showPasswordCheck_;
    QLabel*      errorLabel_;
    QPushButton* loginBtn_;
    QPushButton* signupBtn_;
};