#pragma once
#include <iostream>
#include <ctime>
#include <string>
#include <vector>

using namespace std;

// ==================== POST CLASS ====================
class Post
{
public:
    int postID;
    string content;
    string imagePath;
    string videoPath;
    int likeCount;
    std::vector<int> likedBy;
    string comments[50];
    int commentCount;
    Post *next;
    time_t timestamp;

    Post()
    {
        postID = 0;
        content = "";
        imagePath = "";
        videoPath = "";
        likeCount = 0;
        commentCount = 0;
        next = nullptr;
        timestamp = time(nullptr);
    }

    Post(int id, string text, string img = "", string vid = "")
    {
        postID = id;
        content = text;
        imagePath = img;
        videoPath = vid;
        likeCount = 0;
        commentCount = 0;
        next = nullptr;
        timestamp = std::time(nullptr);
    }

    // Rule of 3 violations: disable copy and assignment
    Post(const Post &other) = delete;
    Post &operator=(const Post &other) = delete;

    bool toggleLike(int userID)
    {
        for (auto it = likedBy.begin(); it != likedBy.end(); ++it)
        {
            if (*it == userID)
            {
                likedBy.erase(it);
                likeCount--;
                return false;
            }
        }
        likedBy.push_back(userID);
        likeCount++;
        return true;
    }

    bool hasLiked(int userID)
    {
        for (int id : likedBy)
        {
            if (id == userID)
                return true;
        }
        return false;
    }

    void addComment(string c)
    {
        if (commentCount < 50)
            comments[commentCount++] = c;
    }

    void display()
    {
        cout << "Post ID   : " << postID << endl;
        cout << "Content   : " << content << endl;
        cout << "Likes     : " << likeCount << endl;
        cout << "Comments  :" << endl;
        for (int i = 0; i < commentCount; i++)
            cout << "  - " << comments[i] << endl;
    }
};

// ==================== USER CLASS (BASE) ====================
class User
{
public:
    int userID;
    string password;
    string userName;
    string role;
    bool isBanned;
    string birthDate;
    string githubUsername;
    string email;
    string profileImagePath;

    User **friends;
    int friendCount;
    User **request;
    int requestCount;
    User **follower;
    int followerCount;
    User **following;
    int followingCount;
    Post **posts;
    int postCount;

    User()
    {
        userID = 0;
        isBanned = false;
        birthDate = "";
        githubUsername = "";
        email = "";
        profileImagePath = "";
        requestCount = 0;
        followerCount = 0;
        followingCount = 0;
        friendCount = 0;
        postCount = 0;
        friends = new User *[1];
        request = new User *[1];
        follower = new User *[1];
        following = new User *[1];
        posts = new Post *[1];
    }

    virtual ~User()
    {
        delete[] friends;
        delete[] request;
        delete[] follower;
        delete[] following;
        for (int i = 0; i < postCount; i++)
            delete posts[i];
        delete[] posts;
    }

    // Rule of 3 violations: disable copy and assignment to prevent double-free
    User(const User &other) = delete;
    User &operator=(const User &other) = delete;

    // Utility
    void resize(User **&u, int count);

    // FIX: dedicated resize for Post** to avoid undefined-behaviour cast
    void resizePosts(Post **&p, int count);

    // Social
    bool sendRequest(User *u);
    void follow(User *to);
    void acceptRequest(User *u);
    void rejectRequest(User *u);

    // Posts
    void createPost(Post *p);
    bool deletePost(int postID);
    void showPosts();
    void showNewsFeed();
};

// ==================== SUBCLASSES ====================
class NormalUser : public User
{
public:
    NormalUser() { role = "user"; }

    NormalUser(int id, string username, string pass, string em = "")
    {
        userID = id;
        userName = username;
        password = pass;
        role = "user";
        birthDate = "";
        githubUsername = "";
        email = em;
        profileImagePath = "";
    }
};

class Admin : public User
{
public:
    Admin() { role = "admin"; }

    Admin(int id, string username, string pass, string em = "")
    {
        userID = id;
        userName = username;
        password = pass;
        role = "admin";
        birthDate = "";
        githubUsername = "";
        email = em;
        profileImagePath = "";
    }
};

// ==================== MESSAGE CLASS ====================
class Message
{
public:
    int senderID;
    int receiverID;
    string text;
    time_t timestamp;

    Message(int s, int r, string t, time_t ts = 0)
    {
        senderID = s;
        receiverID = r;
        text = t;
        timestamp = ts;
    }

    // Disable copy/assignment
    Message(const Message &other) = delete;
    Message &operator=(const Message &other) = delete;
};

// ==================== MESSAGE SYSTEM CLASS ====================
class MessageSystem
{
public:
    Message **msg;
    int msgCount;

    MessageSystem()
    {
        msg = new Message *[1];
        msgCount = 0;
    }

    ~MessageSystem()
    {
        for (int i = 0; i < msgCount; i++)
            delete msg[i];
        delete[] msg;
    }

    void resize();
    bool sendMessage(User *from, User *to, string text);
    bool sendGroupMessage(User *from, int groupID, const string &text);
    void removeMessagesForGroup(int groupID);
    void viewInbox(User *u);
};

// ==================== GLOBAL VARIABLES ====================
extern User **users; // pointer array
extern int userCount;

// ==================== GLOBAL FUNCTIONS ====================
bool userNameExist(string n);
bool emailTaken(string e, int exceptUserID = -1);
bool adminExist();
void resizeUsers();
void signup(int id, string usern, string pass, string rol, string email = "");
int login(string u, string pass);
void removeUserReferences(int targetID);
void deleteAccount(int index);
void adminDelete(int adminIndex, string u);
void display(int adminIndex);

// ==================== NOTIFICATION CLASS ====================
enum NotificationKind : int
{
    NotifNormal = 0,
    NotifFriendRequest = 1
};

class Notification
{
public:
    int targetUserID;
    string message;
    time_t timestamp;
    bool seen;
    int kind;          // NotificationKind
    int relatedUserID; // e.g. friend-request sender when kind == NotifFriendRequest

    Notification()
    {
        targetUserID = -1;
        message = "";
        timestamp = 0;
        seen = false;
        kind = NotifNormal;
        relatedUserID = -1;
    }
};

class NotificationSystem
{
    Notification **notifications;
    int notifCount;

    void resize();

public:
    NotificationSystem()
    {
        notifications = nullptr;
        notifCount = 0;
    }

    ~NotificationSystem()
    {
        for (int i = 0; i < notifCount; i++)
            delete notifications[i];
        delete[] notifications;
    }

    void addNotification(int targetID, string msg);
    void addFriendRequestNotification(int targetUserID, int fromUserID, const string &message);
    void removeFriendRequestNotifications(int targetUserID, int fromUserID);
    void showNotifications(int userID, string userName);

    // UI accessors
    int getCount() const { return notifCount; }
    Notification *getAt(int i) const { return (i >= 0 && i < notifCount) ? notifications[i] : nullptr; }

    // Count unseen notifications for a user
    int countUnseen(int userID) const
    {
        int cnt = 0;
        for (int i = 0; i < notifCount; i++)
            if (notifications[i]->targetUserID == userID && !notifications[i]->seen)
                cnt++;
        return cnt;
    }

    // Mark all notifications for a user as seen
    void markAllSeen(int userID)
    {
        for (int i = 0; i < notifCount; i++)
            if (notifications[i]->targetUserID == userID)
                notifications[i]->seen = true;
    }

    // ===== FILE HANDLING =====
    void saveToFile(const string &filename);
    void loadFromFile(const string &filename);
};

extern NotificationSystem notifSystem;

void freeAllData();

// ==================== GROUP CHAT ====================
class Group
{
public:
    int groupID;
    string name;
    string iconPath;
    int creatorID;
    std::vector<int> memberIDs;

    Group() : groupID(0), name(""), iconPath(""), creatorID(0) {}

    // Disable copy/assignment
    Group(const Group &other) = delete;
    Group &operator=(const Group &other) = delete;
};

class GroupSystem
{
public:
    Group **groups;
    int groupCount;

    GroupSystem();
    ~GroupSystem();

    void resizeGroups();
    Group *createGroup(User *creator, const string &name, const string &iconPath,
                       const std::vector<int> &friendUserIDs);
    Group *findGroup(int groupID);
    bool setGroupName(int groupID, User *actor, const string &newName);
    bool addMembersToGroup(int groupID, User *actor, const std::vector<int> &friendUserIDs);
    bool removeMember(int groupID, User *actor, int memberUserID);
    static bool isMember(Group *g, int userID);
    void removeUserFromAllGroups(int userID);
};

extern GroupSystem groupSystem;
extern int nextGroupID;

// ==================== REPORTS & BAN APPEALS ====================
struct UserReport
{
    int reporterID;
    int reportedID;
    string reason;      // WHY the user was reported (filled from dialog)
    time_t timestamp;
};

struct BanAppeal
{
    int userID;
    string message;
    time_t timestamp;
};

class ModerationSystem
{
public:
    std::vector<UserReport> reports;
    std::vector<BanAppeal> pendingAppeals;

    // reason is the text the reporter typed in the dialog (e.g. "Harassment", "Spam")
    bool addReport(User *reporter, int reportedUserID, const string &reason = "");
    void clearReportsAgainst(int reportedUserID);
    int reportCountAgainst(int reportedUserID) const;
    bool alreadyReportedPair(int reporterID, int reportedID) const;

    void addAppeal(int userID, const string &msg);
    bool hasPendingAppeal(int userID) const;
    void removeAppealsFor(int userID);
};

extern ModerationSystem modSystem;

int getFirstAdminUserID();

// ==================== FILE HANDLING ====================
void saveData(); // call on exit
void loadData(); // call on startup