#pragma once
#include <iostream>
#include <ctime>
#include <string>
#include <vector>

using namespace std;

// ==================== POST CLASS ====================
class Post {
public:
    int postID;
    string content;
    string imagePath;
    int likeCount;
    std::vector<int> likedBy;
    string comments[50];
    int commentCount;
    Post* next;
    time_t timestamp;

    Post() {
        postID = 0;
        content = "";
        imagePath = "";
        likeCount = 0;
        commentCount = 0;
        next = nullptr;
        timestamp = time(nullptr);
    }

    Post(int id, string c, string imgPath = "") {
        postID = id;
        content = c;
        imagePath = imgPath;
        likeCount = 0;
        commentCount = 0;
        next = nullptr;
        timestamp = time(nullptr);
    }

    bool toggleLike(int userID) {
        for (auto it = likedBy.begin(); it != likedBy.end(); ++it) {
            if (*it == userID) {
                likedBy.erase(it);
                likeCount--;
                return false;
            }
        }
        likedBy.push_back(userID);
        likeCount++;
        return true;
    }

    bool hasLiked(int userID) {
        for (int id : likedBy) {
            if (id == userID) return true;
        }
        return false;
    }

    void addComment(string c) {
        if (commentCount < 50)
            comments[commentCount++] = c;
    }

    void display() {
        cout << "Post ID   : " << postID << endl;
        cout << "Content   : " << content << endl;
        cout << "Likes     : " << likeCount << endl;
        cout << "Comments  :" << endl;
        for (int i = 0; i < commentCount; i++)
            cout << "  - " << comments[i] << endl;
    }
};

// ==================== USER CLASS (BASE) ====================
class User {
public:
    int userID;
    string password;
    string userName;
    string role;
    bool isBanned;
    string birthDate;
    string githubUsername;
    string profileImagePath;

    User** friends;
    int friendCount;
    User** request;
    int requestCount;
    User** follower;
    int followerCount;
    User** following;
    int followingCount;
    Post** posts;
    int postCount;

    User() {
        userID = 0;
        isBanned = false;
        birthDate = "";
        githubUsername = "";
        profileImagePath = "";
        requestCount = 0;
        followerCount = 0;
        followingCount = 0;
        friendCount = 0;
        postCount = 0;
        friends = new User * [1];
        request = new User * [1];
        follower = new User * [1];
        following = new User * [1];
        posts = new Post * [1];
    }

    virtual ~User() {
        delete[] friends;
        delete[] request;
        delete[] follower;
        delete[] following;
        for (int i = 0; i < postCount; i++)
            delete posts[i];
        delete[] posts;
    }

    // Copy constructor (shallow copy of pointer arrays)
    User(const User& other) {
        userID = other.userID;
        password = other.password;
        userName = other.userName;
        role = other.role;
        isBanned = other.isBanned;
        birthDate = other.birthDate;
        githubUsername = other.githubUsername;
        profileImagePath = other.profileImagePath;
        friendCount = other.friendCount;
        requestCount = other.requestCount;
        followerCount = other.followerCount;
        followingCount = other.followingCount;
        postCount = other.postCount;

        friends = new User * [friendCount + 1];
        request = new User * [requestCount + 1];
        follower = new User * [followerCount + 1];
        following = new User * [followingCount + 1];
        posts = new Post * [postCount + 1];

        for (int i = 0; i < friendCount; i++) friends[i] = other.friends[i];
        for (int i = 0; i < requestCount; i++) request[i] = other.request[i];
        for (int i = 0; i < followerCount; i++) follower[i] = other.follower[i];
        for (int i = 0; i < followingCount; i++) following[i] = other.following[i];
        for (int i = 0; i < postCount; i++) posts[i] = other.posts[i];
    }

    User& operator=(const User& other) {
        if (this != &other) {
            delete[] friends;
            delete[] request;
            delete[] follower;
            delete[] following;
            delete[] posts;

            userID = other.userID;
            password = other.password;
            userName = other.userName;
            role = other.role;
            isBanned = other.isBanned;
            birthDate = other.birthDate;
            githubUsername = other.githubUsername;
            profileImagePath = other.profileImagePath;
            friendCount = other.friendCount;
            requestCount = other.requestCount;
            followerCount = other.followerCount;
            followingCount = other.followingCount;
            postCount = other.postCount;

            friends = new User * [friendCount + 1];
            request = new User * [requestCount + 1];
            follower = new User * [followerCount + 1];
            following = new User * [followingCount + 1];
            posts = new Post * [postCount + 1];

            for (int i = 0; i < friendCount; i++) friends[i] = other.friends[i];
            for (int i = 0; i < requestCount; i++) request[i] = other.request[i];
            for (int i = 0; i < followerCount; i++) follower[i] = other.follower[i];
            for (int i = 0; i < followingCount; i++) following[i] = other.following[i];
            for (int i = 0; i < postCount; i++) posts[i] = other.posts[i];
        }
        return *this;
    }

    // Utility
    void resize(User**& u, int count);

    // Social
    void sendRequest(User* u);
    void follow(User* to);
    void acceptRequest(User* u);
    void rejectRequest(User* u);

    // Posts
    void createPost(Post* p);
    bool deletePost(int postID);
    void showPosts();
    void showNewsFeed();
};

// ==================== SUBCLASSES ====================
class NormalUser : public User {
public:
    NormalUser() { role = "user"; }

    NormalUser(int id, string username, string pass) {
        userID = id;
        userName = username;
        password = pass;
        role = "user";
        birthDate = "";
        githubUsername = "";
        profileImagePath = "";
    }
};

class Admin : public User {
public:
    Admin() { role = "admin"; }

    Admin(int id, string username, string pass) {
        userID = id;
        userName = username;
        password = pass;
        role = "admin";
        birthDate = "";
        githubUsername = "";
        profileImagePath = "";
    }
};

// ==================== MESSAGE CLASS ====================
class Message {
public:
    int senderID;
    int receiverID;
    string text;
    time_t timestamp;

    Message(int s, int r, string t, time_t ts = 0) {
        senderID = s;
        receiverID = r;
        text = t;
        timestamp = ts;
    }
};

// ==================== MESSAGE SYSTEM CLASS ====================
class MessageSystem {
public:
    Message** msg;
    int msgCount;

    MessageSystem() {
        msg = new Message * [1];
        msgCount = 0;
    }

    ~MessageSystem() {
        for (int i = 0; i < msgCount; i++)
            delete msg[i];
        delete[] msg;
    }

    void resize();
    bool sendMessage(User* from, User* to, string text);
    void viewInbox(User* u);
};

// ==================== GLOBAL VARIABLES ====================
extern User** users;   // pointer array
extern int userCount;

// ==================== GLOBAL FUNCTIONS ====================
bool userNameExist(string n);
bool adminExist();
void resizeUsers();
void signup(int id, string usern, string pass, string rol);
int  login(string u, string pass);
void removeUserReferences(int targetID);
void deleteAccount(int index);
void adminDelete(int adminIndex, string u);
void display(int adminIndex);

// ==================== NOTIFICATION CLASS ====================
class Notification {
public:
    int    targetUserID;
    string message;
    time_t timestamp;
    bool   seen;

    Notification() {
        targetUserID = -1;
        message = "";
        timestamp = 0;
        seen = false;
    }
};

class NotificationSystem {
    Notification** notifications;
    int notifCount;

    void resize();

public:
    NotificationSystem() {
        notifications = nullptr;
        notifCount = 0;
    }

    ~NotificationSystem() {
        for (int i = 0; i < notifCount; i++)
            delete notifications[i];
        delete[] notifications;
    }

    void addNotification(int targetID, string msg);
    void showNotifications(int userID, string userName);

    // UI accessors
    int getCount() const { return notifCount; }
    Notification* getAt(int i) const { return (i >= 0 && i < notifCount) ? notifications[i] : nullptr; }

    // Count unseen notifications for a user
    int countUnseen(int userID) const {
        int cnt = 0;
        for (int i = 0; i < notifCount; i++)
            if (notifications[i]->targetUserID == userID && !notifications[i]->seen)
                cnt++;
        return cnt;
    }

    // Mark all notifications for a user as seen
    void markAllSeen(int userID) {
        for (int i = 0; i < notifCount; i++)
            if (notifications[i]->targetUserID == userID)
                notifications[i]->seen = true;
    }

    // ===== FILE HANDLING =====
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);
};

void searchUsers(string keyword);
void searchPosts(string keyword);

extern NotificationSystem notifSystem;

// ==================== FILE HANDLING ====================
void saveData();   // call on exit
void loadData();   // call on startup
