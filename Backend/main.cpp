#include <iostream>
#include "user.h"
using namespace std;

// ==================== GLOBAL ====================
MessageSystem msgSystem;
int nextID = 1;
int loggedInIndex = -1;

// ==================== HELPER ====================
User* findUserByName(string name) {
    for (int i = 0; i < userCount; i++)
        if (users[i]->userName == name) return users[i];
    return nullptr;
}

int findIndexByName(string name) {
    for (int i = 0; i < userCount; i++)
        if (users[i]->userName == name) return i;
    return -1;
}

void clearScreen() {
    cout << "\n\n";
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

// ==================== MENUS ====================

void showMainMenu() {
    cout << "==================================" << endl;
    cout << "|     SOCIAL MEDIA PLATFORM      |" << endl;
    cout << "==================================" << endl;
    cout << "|  1. Sign Up                   |" << endl;
    cout << "|  2. Log In                    |" << endl;
    cout << "|  0. Exit                      |" << endl;
    cout << "==================================" << endl;
    cout << "Choice: ";
}

void showUserMenu() {
    cout << "\n==================================" << endl;
    cout << "|        USER DASHBOARD          |" << endl;
    cout << "==================================" << endl;
    cout << "|  1.  Send Friend Request      |" << endl;
    cout << "|  2.  Accept Friend Request    |" << endl;
    cout << "|  3.  Reject Friend Request    |" << endl;
    cout << "|  4.  Follow a User            |" << endl;
    cout << "|  5.  Create Post              |" << endl;
    cout << "|  6.  Show My Posts            |" << endl;
    cout << "|  7.  Show News Feed           |" << endl;
    cout << "|  8.  Send Message             |" << endl;
    cout << "|  9.  View Inbox               |" << endl;
    cout << "|  10. Search Users             |" << endl;
    cout << "|  11. Search Posts             |" << endl;
    cout << "|  12. View Friend List         |" << endl;
    cout << "|  13. View Following List      |" << endl;
    cout << "|  14. View Pending Requests    |" << endl;
    cout << "|  15. Like a Post              |" << endl;
    cout << "|  16. Comment on a Post        |" << endl;
    cout << "|  17. Delete My Account        |" << endl;
    cout << "|  18. View Notifications       |" << endl;
    cout << "|  0.  Logout                   |" << endl;
    cout << "==================================" << endl;
    cout << "Choice: ";
}

void showAdminMenu() {
    cout << "\n==================================" << endl;
    cout << "|       ADMIN DASHBOARD         |" << endl;
    cout << "==================================" << endl;
    cout << "|  1.  Display All Users       |" << endl;
    cout << "|  2.  Delete a User           |" << endl;
    cout << "|  3.  Search Users            |" << endl;
    cout << "|  4.  Search Posts            |" << endl;
    cout << "|  5.  View All Posts          |" << endl;
    cout << "|  0.  Logout                  |" << endl;
    cout << "==================================" << endl;
    cout << "Choice: ";
}

// ==================== SIGNUP / LOGIN ====================

void handleSignup() {
    clearScreen();
    cout << "===== SIGN UP =====" << endl;
    string username, password, role;

    cout << "Enter Username : "; cin >> username;
    cout << "Enter Password : "; cin >> password;
    cout << "Enter Role (user/admin): "; cin >> role;

    signup(nextID++, username, password, role);
    pause();
}

int handleLogin() {
    clearScreen();
    cout << "===== LOG IN =====" << endl;
    string username, password;

    cout << "Enter Username : "; cin >> username;
    cout << "Enter Password : "; cin >> password;

    int index = login(username, password);
    if (index == -1) {
        cout << "Invalid credentials." << endl;
        pause();
        return -1;
    }

    cout << "Welcome, " << users[index]->userName << "! (" << users[index]->role << ")" << endl;
    pause();
    return index;
}

// ==================== USER ACTIONS ====================

void handleSendRequest() {
    string name;
    cout << "Enter username to send request: "; cin >> name;
    users[loggedInIndex]->sendRequest(findUserByName(name));
    pause();
}

void handleAcceptRequest() {
    string name;
    cout << "Enter username to accept request: "; cin >> name;
    users[loggedInIndex]->acceptRequest(findUserByName(name));
    pause();
}

void handleRejectRequest() {
    string name;
    cout << "Enter username to reject request: "; cin >> name;
    users[loggedInIndex]->rejectRequest(findUserByName(name));
    pause();
}

void handleFollow() {
    string name;
    cout << "Enter username to follow: "; cin >> name;
    users[loggedInIndex]->follow(findUserByName(name));
    pause();
}

void handleCreatePost() {
    string content;
    cout << "Enter post content: ";
    cin.ignore();
    getline(cin, content);
    Post* p = new Post(nextID++, content);
    users[loggedInIndex]->createPost(p);
    cout << "Post created successfully." << endl;
    pause();
}

void handleShowMyPosts() {
    cout << "\n===== MY POSTS =====" << endl;
    users[loggedInIndex]->showPosts();
    pause();
}

void handleNewsFeed() {
    users[loggedInIndex]->showNewsFeed();
    pause();
}

void handleSendMessage() {
    string name, text;
    cout << "Enter username to message: "; cin >> name;
    cout << "Enter message: ";
    cin.ignore();
    getline(cin, text);
    msgSystem.sendMessage(users[loggedInIndex], findUserByName(name), text);
    pause();
}

void handleViewInbox() {
    msgSystem.viewInbox(users[loggedInIndex]);
    pause();
}

void handleSearchUsers() {
    string keyword;
    cout << "Enter search keyword: "; cin >> keyword;
    searchUsers(keyword);
    pause();
}

void handleSearchPosts() {
    string keyword;
    cout << "Enter search keyword: "; cin >> keyword;
    searchPosts(keyword);
    pause();
}

void handleFriendList() {
    User* u = users[loggedInIndex];
    cout << "\n===== FRIENDS =====" << endl;
    if (u->friendCount == 0) { cout << "No friends yet." << endl; }
    else {
        for (int i = 0; i < u->friendCount; i++)
            cout << "- " << u->friends[i]->userName << endl;
    }
    pause();
}

void handleFollowingList() {
    User* u = users[loggedInIndex];
    cout << "\n===== FOLLOWING =====" << endl;
    if (u->followingCount == 0) { cout << "Not following anyone." << endl; }
    else {
        for (int i = 0; i < u->followingCount; i++)
            cout << "- " << u->following[i]->userName << endl;
    }
    pause();
}

void handlePendingRequests() {
    User* u = users[loggedInIndex];
    cout << "\n===== PENDING REQUESTS =====" << endl;
    if (u->requestCount == 0) { cout << "No pending requests." << endl; }
    else {
        for (int i = 0; i < u->requestCount; i++)
            cout << "- " << u->request[i]->userName << endl;
    }
    pause();
}

void handleLikePost() {
    string name;
    int postID;
    cout << "Enter username whose post to like: "; cin >> name;
    cout << "Enter Post ID: "; cin >> postID;

    User* target = findUserByName(name);
    if (!target) { cout << "User not found." << endl; pause(); return; }

    // sirf friends ki post like ho sakti hai
    bool isFriend = false;
    for (int i = 0; i < users[loggedInIndex]->friendCount; i++) {
        if (users[loggedInIndex]->friends[i] == target) {
            isFriend = true;
            break;
        }
    }
    if (!isFriend && target != users[loggedInIndex]) {
        cout << "You can only like posts of your friends." << endl;
        pause();
        return;
    }

    bool found = false;
    for (int i = 0; i < target->postCount; i++) {
        if (target->posts[i]->postID == postID) {
            target->posts[i]->like();
            cout << "Post liked!" << endl;
            // NOTIFICATION: post owner ko batao
            notifSystem.addNotification(
                target->userID,
                users[loggedInIndex]->userName + " ne aapki post like ki."
            );
            found = true;
            break;
        }
    }
    if (!found) cout << "Post not found." << endl;
    pause();
}

void handleCommentPost() {
    string name, comment;
    int postID;
    cout << "Enter username whose post to comment on: "; cin >> name;
    cout << "Enter Post ID: "; cin >> postID;
    cout << "Enter comment: ";
    cin.ignore();
    getline(cin, comment);

    User* target = findUserByName(name);
    if (!target) { cout << "User not found." << endl; pause(); return; }

    // sirf friends ki post pe comment ho sakta hai
    bool isFriend = false;
    for (int i = 0; i < users[loggedInIndex]->friendCount; i++) {
        if (users[loggedInIndex]->friends[i] == target) {
            isFriend = true;
            break;
        }
    }
    if (!isFriend && target != users[loggedInIndex]) {
        cout << "You can only comment on posts of your friends." << endl;
        pause();
        return;
    }

    bool found = false;
    for (int i = 0; i < target->postCount; i++) {
        if (target->posts[i]->postID == postID) {
            target->posts[i]->addComment(
                users[loggedInIndex]->userName + ": " + comment
            );
            cout << "Comment added!" << endl;
            // NOTIFICATION: post owner ko batao
            notifSystem.addNotification(
                target->userID,
                users[loggedInIndex]->userName + " ne aapki post pe comment kiya."
            );
            found = true;
            break;
        }
    }
    if (!found) cout << "Post not found." << endl;
    pause();
}

void handleViewNotifications() {
    notifSystem.showNotifications(
        users[loggedInIndex]->userID,
        users[loggedInIndex]->userName
    );
    pause();
}

void handleDeleteMyAccount() {
    char confirm;
    cout << "Are you sure you want to delete your account? (y/n): "; cin >> confirm;
    if (confirm == 'y' || confirm == 'Y') {
        int index = loggedInIndex;
        loggedInIndex = -1;
        deleteAccount(index);
    }
    pause();
}

// ==================== ADMIN ACTIONS ====================

void handleAdminViewAllPosts() {
    cout << "\n===== ALL POSTS =====" << endl;
    bool any = false;
    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->postCount; j++) {
            cout << "By: " << users[i]->userName << endl;
            users[i]->posts[j]->display();
            cout << "-----------------------------" << endl;
            any = true;
        }
    }
    if (!any) cout << "No posts found." << endl;
    pause();
}

// ==================== DASHBOARD LOOPS ====================

void userDashboard() {
    int choice;
    while (loggedInIndex != -1) {
        clearScreen();
        cout << "Logged in as: " << users[loggedInIndex]->userName << endl;
        showUserMenu();
        cin >> choice;

        switch (choice) {
        case 1:  handleSendRequest();      break;
        case 2:  handleAcceptRequest();    break;
        case 3:  handleRejectRequest();    break;
        case 4:  handleFollow();           break;
        case 5:  handleCreatePost();       break;
        case 6:  handleShowMyPosts();      break;
        case 7:  handleNewsFeed();         break;
        case 8:  handleSendMessage();      break;
        case 9:  handleViewInbox();        break;
        case 10: handleSearchUsers();      break;
        case 11: handleSearchPosts();      break;
        case 12: handleFriendList();       break;
        case 13: handleFollowingList();    break;
        case 14: handlePendingRequests();  break;
        case 15: handleLikePost();         break;
        case 16: handleCommentPost();      break;
        case 17: handleDeleteMyAccount(); return;
        case 18: handleViewNotifications(); break; // ADDED
        case 0:
            cout << "Logged out successfully." << endl;
            loggedInIndex = -1;
            pause();
            return;
        default:
            cout << "Invalid choice." << endl;
            pause();
        }
    }
}

void adminDashboard() {
    int choice;
    while (true) {
        clearScreen();
        cout << "Logged in as: " << users[loggedInIndex]->userName << " (Admin)" << endl;
        showAdminMenu();
        cin >> choice;

        switch (choice) {
        case 1: {
            display(loggedInIndex);
            pause();
            break;
        }
        case 2: {
            string name;
            cout << "Enter username to delete: "; cin >> name;
            adminDelete(loggedInIndex, name);
            pause();
            break;
        }
        case 3: handleSearchUsers(); break;
        case 4: handleSearchPosts(); break;
        case 5: handleAdminViewAllPosts(); break;
        case 0:
            cout << "Logged out successfully." << endl;
            loggedInIndex = -1;
            pause();
            return;
        default:
            cout << "Invalid choice." << endl;
            pause();
        }
    }
}

// ==================== MAIN ====================

int main() {
    int choice;

    while (true) {
        clearScreen();
        showMainMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            handleSignup();
            break;

        case 2:
            loggedInIndex = handleLogin();
            if (loggedInIndex != -1) {
                if (users[loggedInIndex]->role == "admin" ||
                    users[loggedInIndex]->role == "Admin")
                    adminDashboard();
                else
                    userDashboard();
            }
            break;

        case 0:
            cout << "\nGoodbye!\n";
            for (int i = 0; i < userCount; i++)
                delete users[i];
            delete[] users;
            return 0;

        default:
            cout << "Invalid choice." << endl;
            pause();
        }
    }
}