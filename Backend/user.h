#pragma once
#include <iostream>
#include <ctime>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

// ==================== POST CLASS ====================
class Post {
public:
    int postID;
    string content;
    int likeCount;
    string comments[50];
    int commentCount;
    Post* next;
    time_t timestamp;

    Post() {
        postID = 0;
        content = "";
        likeCount = 0;
        commentCount = 0;
        next = NULL;
        timestamp = time(NULL);
    }

    Post(int id, string c) {
        postID = id;
        content = c;
        likeCount = 0;
        commentCount = 0;
        next = NULL;
    }

    void like() { likeCount++; }

    void addComment(string c) {
        if (commentCount < 50)
            comments[commentCount++] = c;
    }

    void display() {
        cout << "Post ID   : " << postID << endl;
        cout << "Content   : " << content << endl;
        cout << "Likes     : " << likeCount << endl;
        cout << "Comments:" << endl;
        for (int i = 0; i < commentCount; i++)
            cout << "- " << comments[i] << endl;
    }
};

// ==================== USER CLASS ====================
class User {
public:
    int userID;
    string password;
    string userName;
    string role;
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

    ~User() {
        delete[] friends;
        delete[] request;
        delete[] follower;
        delete[] following;
        for (int i = 0; i < postCount; i++)
            delete posts[i];
        delete[] posts;
    }

    User(const User& other) {
        userID = other.userID;
        password = other.password;
        userName = other.userName;
        role = other.role;
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

    void resize(User**& u, int count);
    void createPost(Post* p);
    void sendRequest(User* u);
    void follow(User* to);
    void acceptRequest(User* u);
    void rejectRequest(User* u);
    void showPosts();
    void showNewsFeed();
};

// ==================== MESSAGE CLASS ====================
class Message {
public:
    int senderID;
    int receiverID;
    string text;

    Message(int s, int r, string t) {
        senderID = s;
        receiverID = r;
        text = t;
    }
};

// ==================== Message System CLASS ====================
class MessageSystem {
public:
    Message** msg;
    int msgCount;
    MessageSystem() {
        msg = new Message * [1];
        msgCount = 0;
    }
    void resize();
    void sendMessage(User* from, User* to, string text);
    void viewInbox(User* u);
};

// ==================== GLOBAL VARIABLES ====================
extern User* user;
extern int userCount;

// ==================== GLOBAL FUNCTIONS ====================
bool userNameExist(string n);
bool adminExist();
void resizeUsers();
void signup(int id, string usern, string pass, string rol);
int login(string u, string pass);
void removeUserReferences(int targetID);
void deleteAccount(int index);
void adminDelete(int adminIndex, string u);
void display(int adminIndex);
