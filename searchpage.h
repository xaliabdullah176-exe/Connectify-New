#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QTimer>

// ─────────────────────────────────────────────────────
//  Data structure for a search result user
// ─────────────────────────────────────────────────────
struct SearchUser {
    int     userID;
    QString name;
    QString username;
    QString bio;
    bool    isFriend;
};

// ─────────────────────────────────────────────────────
//  A single user result row
// ─────────────────────────────────────────────────────
class SearchResultRow : public QFrame {
    Q_OBJECT
public:
    SearchResultRow(const SearchUser &user, QWidget *parent = nullptr);

signals:
    void viewProfileClicked(int userID);
    void addFriendClicked(int userID);

private:
    int m_userID;
};

// ─────────────────────────────────────────────────────
//  Main SearchPage
// ─────────────────────────────────────────────────────
class SearchPage : public QWidget {
    Q_OBJECT

public:
    explicit SearchPage(QWidget *parent = nullptr);

    // Call after login
    void loadCurrentUser(int userID, const QString &name);

    // Add a user to the full list (call for every user on load)
    void addUser(int userID, const QString &name,
                 const QString &username, const QString &bio,
                 bool isFriend = false);

    // Clear all results
    void clearResults();

signals:
    void viewProfileClicked(int userID);
    void addFriendClicked(int userID);
    void backClicked();

private slots:
    void onSearchChanged(const QString &query);

private:
    // State
    int     m_currentUserID = -1;
    QString m_currentUserName;
    QList<SearchUser> m_allUsers;   // full list for filtering

    // UI elements
    QLineEdit   *searchInput;
    QLabel      *resultsCountLabel;
    QLabel      *searchQueryLabel;
    QVBoxLayout *resultsLayout;     // holds result rows
    QWidget     *noResultsWidget;
    QLabel      *currentUserNameLabel;
    QLabel      *currentUserAvatarLabel;

    // Helpers
    void setupUI();
    void applyStyles();
    void showResults(const QList<SearchUser> &users);
    QString makeInitials(const QString &name);
};
