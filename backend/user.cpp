#include <iostream>
#include "user.h"
using namespace std;

// ==================== GLOBAL VARIABLES ====================
User** users = nullptr;
int    userCount = 0;int nextID = 1;
MessageSystem msgSystem;
//NotificationSystem notifSystem;

// ==================== USER METHODS ====================

void User::resize(User**& u, int count) {
    User** temp = new User * [count + 1];
    for (int i = 0; i < count; i++)
        temp[i] = u[i];
    delete[] u;
    u = temp;
}

void User::sendRequest(User* u) {
    if (u == nullptr) { cout << "User not found." << endl; return; }
    if (u == this) { cout << "Cannot send request to yourself." << endl; return; }

    for (int i = 0; i < friendCount; i++)
        if (friends[i] == u) { cout << "Already friends." << endl; return; }

    for (int i = 0; i < requestCount; i++)
        if (request[i] == u) { cout << "They already sent you a request � accept it instead." << endl; return; }

    for (int i = 0; i < u->requestCount; i++)
        if (u->request[i] == this) { cout << "Request already sent." << endl; return; }

    u->resize(u->request, u->requestCount);
    u->request[u->requestCount++] = this;
    cout << "Request sent successfully." << endl;
}

void User::follow(User* to) {
    if (to == nullptr || to == this) return;

    for (int i = 0; i < followingCount; i++)
        if (following[i] == to) return;

    resize(following, followingCount);
    following[followingCount++] = to;

    to->resize(to->follower, to->followerCount);
    to->follower[to->followerCount++] = this;
}

void User::acceptRequest(User* u) {
    for (int i = 0; i < friendCount; i++)
        if (friends[i] == u) { cout << "Already friends." << endl; return; }

    bool found = false;
    for (int i = 0; i < requestCount; i++)
        if (request[i] == u) { found = true; break; }
    if (!found) { cout << "No request from this user." << endl; return; }

    // Check if u already follows this user (mutual follow = friendship)
    bool mutual = false;
    for (int i = 0; i < u->followingCount; i++)
        if (u->following[i] == this) { mutual = true; break; }

    follow(u);  // this user now follows u

    if (mutual) {
        resize(friends, friendCount);
        friends[friendCount++] = u;

        u->resize(u->friends, u->friendCount);
        u->friends[u->friendCount++] = this;

        cout << "Request accepted � you are now friends." << endl;
    }
    else {
        cout << "Request accepted." << endl;
    }

    rejectRequest(u);  // remove from request list
}

void User::rejectRequest(User* u) {
    for (int i = 0; i < requestCount; i++) {
        if (request[i] == u) {
            for (int j = i; j < requestCount - 1; j++)
                request[j] = request[j + 1];
            requestCount--;
            return;
        }
    }
}



//void User::showPosts() {
//    if (postCount == 0) { cout << "No posts yet." << endl; return; }
//    for (int i = 0; i < postCount; i++)
//        posts[i]->display();
//}



// ==================== GLOBAL FUNCTIONS ====================

bool userNameExist(string n) {
    for (int i = 0; i < userCount; i++)
        if (users[i]->userName == n) return true;
    return false;
}

bool adminExist() {
    for (int i = 0; i < userCount; i++)
        if (users[i]->role == "admin" || users[i]->role == "Admin") return true;
    return false;
}

void resizeUsers() {
    User** temp = new User * [userCount + 1];
    for (int i = 0; i < userCount; i++)
        temp[i] = users[i];
    delete[] users;
    users = temp;
}

void signup(int id, string usern, string pass, string rol) {
    if (userNameExist(usern)) { cout << "Username already exists." << endl; return; }

    string roleLower = rol;
    for (char& c : roleLower) c = tolower(c);

    if (roleLower == "admin" && adminExist()) { cout << "Admin already exists." << endl; return; }

    resizeUsers();

    if (roleLower == "admin")
        users[userCount] = new Admin(id, usern, pass);
    else
        users[userCount] = new NormalUser(id, usern, pass);

    userCount++;
    cout << "Account created successfully." << endl;
}

int login(string u, string pass) {
    for (int i = 0; i < userCount; i++)
        if (users[i]->userName == u && users[i]->password == pass)
            return i;
    return -1;
}

void removeUserReferences(int targetID) {
    for (int i = 0; i < userCount; i++) {
        User* u = users[i];
        if (u->userID == targetID) continue;

        auto removeFrom = [](User**& arr, int& count, int targetID) {
            for (int j = 0; j < count; j++) {
                if (arr[j]->userID == targetID) {
                    for (int k = j; k < count - 1; k++) arr[k] = arr[k + 1];
                    count--;
                    return;
                }
            }
            };

        removeFrom(u->friends, u->friendCount, targetID);
        removeFrom(u->request, u->requestCount, targetID);
        removeFrom(u->follower, u->followerCount, targetID);
        removeFrom(u->following, u->followingCount, targetID);
    }
}

void deleteAccount(int index) {
    if (index < 0 || index >= userCount) { cout << "No such user found." << endl; return; }
    removeUserReferences(users[index]->userID);
    for (int i = 0; i < users[index]->postCount; i++)
        delete users[index]->posts[i];

    delete users[index];
    for (int i = index; i < userCount - 1; i++)
        users[i] = users[i + 1];
    userCount--;
    cout << "Account deleted successfully." << endl;
}

void adminDelete(int adminIndex, string u) {
    if (users[adminIndex]->role != "admin" && users[adminIndex]->role != "Admin") {
        cout << "Access denied." << endl; return;
    }
  
    int index = -1;
    for (int i = 0; i < userCount; i++)
        if (users[i]->userName == u) { index = i; break; }
    if (users[index]->role == "admin" || users[index]->role == "Admin") return;
    if (index == -1) { cout << "No such user found." << endl; return; }

    deleteAccount(index);
}

void display(int adminIndex) {
    if (users[adminIndex]->role != "admin" && users[adminIndex]->role != "Admin") {
        cout << "Access denied." << endl; return;
    }
    for (int i = 0; i < userCount; i++) {
        cout << "User ID   : " << users[i]->userID << endl;
        cout << "Username  : " << users[i]->userName << endl;
       // cout << "Role      : " << users[i]->role << endl;
        cout << "-------------------------" << endl;
    }
}
void saveData() {
    // File handling - placeholder for now
}

void loadData() {
    // File handling - placeholder for now
}







