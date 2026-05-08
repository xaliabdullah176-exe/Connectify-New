#include <iostream>
#include "user.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QVariant>
using namespace std;

// ==================== GLOBAL VARIABLES ====================
User** users = nullptr;
int    userCount = 0;int nextID = 1;
MessageSystem msgSystem;
//NotificationSystem notifSystem;

static void ensureDefaultAdminAccount() {
    const string defaultAdminUser = "Jeffrey_epstien";
    const string defaultAdminPass = "Mustafa_tech123";

    for (int i = 0; i < userCount; i++) {
        if (users[i] && users[i]->role == "admin") {
            // Keep existing admin IDs/relations; only enforce credentials.
            users[i]->userName = defaultAdminUser;
            users[i]->password = defaultAdminPass;
            return;
        }
    }

    resizeUsers();
    users[userCount++] = new Admin(nextID++, defaultAdminUser, defaultAdminPass);
}

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

    follow(u);
    u->follow(this);

    resize(friends, friendCount);
    friends[friendCount++] = u;

    u->resize(u->friends, u->friendCount);
    u->friends[u->friendCount++] = this;

    cout << "Request accepted. You are now friends." << endl;

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
    // User destructor already frees posts; avoid double-delete crash.
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
    if (index == -1) { cout << "No such user found." << endl; return; }
    if (users[index]->role == "admin" || users[index]->role == "Admin") return;

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
    QJsonObject root;
    root["nextID"] = nextID;
    root["userCount"] = userCount;
    
    QJsonArray usersArr;
    for (int i = 0; i < userCount; i++) {
        QJsonObject uObj;
        uObj["userID"] = users[i]->userID;
        uObj["userName"] = QString::fromStdString(users[i]->userName);
        uObj["password"] = QString::fromStdString(users[i]->password);
        uObj["role"] = QString::fromStdString(users[i]->role);
        uObj["isBanned"] = users[i]->isBanned;
        uObj["birthDate"] = QString::fromStdString(users[i]->birthDate);
        uObj["githubUsername"] = QString::fromStdString(users[i]->githubUsername);
        uObj["profileImagePath"] = QString::fromStdString(users[i]->profileImagePath);
        usersArr.append(uObj);
    }
    root["users"] = usersArr;

    QJsonArray postsArr;
    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->postCount; j++) {
            Post* p = users[i]->posts[j];
            QJsonObject pObj;
            pObj["postID"] = p->postID;
            pObj["ownerID"] = users[i]->userID;
            pObj["content"] = QString::fromStdString(p->content);
            pObj["imagePath"] = QString::fromStdString(p->imagePath);
            pObj["timestamp"] = (qint64)p->timestamp;
            pObj["likeCount"] = p->likeCount;
            
            QJsonArray likedByArr;
            for (int id : p->likedBy) {
                likedByArr.append(id);
            }
            pObj["likedBy"] = likedByArr;
            
            QJsonArray commentsArr;
            for (int k = 0; k < p->commentCount; k++) {
                commentsArr.append(QString::fromStdString(p->comments[k]));
            }
            pObj["comments"] = commentsArr;
            
            postsArr.append(pObj);
        }
    }
    root["posts"] = postsArr;

    QJsonArray friendsArr;
    QJsonArray reqArr;
    QJsonArray folArr;
    QJsonArray followingArr;
    
    for (int i = 0; i < userCount; i++) {
        int u1 = users[i]->userID;
        for (int j = 0; j < users[i]->friendCount; j++) {
            QJsonArray edge; edge.append(u1); edge.append(users[i]->friends[j]->userID);
            friendsArr.append(edge);
        }
        for (int j = 0; j < users[i]->requestCount; j++) {
            QJsonArray edge; edge.append(u1); edge.append(users[i]->request[j]->userID);
            reqArr.append(edge);
        }
        for (int j = 0; j < users[i]->followerCount; j++) {
            QJsonArray edge; edge.append(u1); edge.append(users[i]->follower[j]->userID);
            folArr.append(edge);
        }
        for (int j = 0; j < users[i]->followingCount; j++) {
            QJsonArray edge; edge.append(u1); edge.append(users[i]->following[j]->userID);
            followingArr.append(edge);
        }
    }
    QJsonObject rel;
    rel["friends"] = friendsArr;
    rel["requests"] = reqArr;
    rel["followers"] = folArr;
    rel["following"] = followingArr;
    root["relations"] = rel;

    QJsonArray msgArr;
    for (int i = 0; i < msgSystem.msgCount; i++) {
        QJsonObject mObj;
        mObj["senderID"] = msgSystem.msg[i]->senderID;
        mObj["receiverID"] = msgSystem.msg[i]->receiverID;
        mObj["text"] = QString::fromStdString(msgSystem.msg[i]->text);
        mObj["timestamp"] = (qint64)msgSystem.msg[i]->timestamp;
        msgArr.append(mObj);
    }
    root["messages"] = msgArr;

    QJsonDocument doc(root);
    QFile file("data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
    
    notifSystem.saveToFile("notifications.txt");
}

void loadData() {
    QFile file("data.json");
    if (!file.open(QIODevice::ReadOnly)) {
        ensureDefaultAdminAccount();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    
    QJsonObject root = doc.object();
    
    if (users != nullptr) {
        for (int i=0; i<userCount; i++) delete users[i];
        delete[] users;
        users = nullptr;
    }

    nextID = root["nextID"].toInt(1);
    
    QJsonArray usersArr = root["users"].toArray();
    userCount = usersArr.size();
    if (userCount > 0) {
        users = new User*[userCount];
        for (int i = 0; i < userCount; i++) {
            QJsonObject uObj = usersArr[i].toObject();
            int uid = uObj["userID"].toInt();
            string uname = uObj["userName"].toString().toStdString();
            string upass = uObj["password"].toString().toStdString();
            string urole = uObj["role"].toString().toStdString();
            
            if (urole == "admin" || urole == "Admin") {
                users[i] = new Admin(uid, uname, upass);
            } else {
                users[i] = new NormalUser(uid, uname, upass);
            }
            users[i]->isBanned = uObj["isBanned"].toBool(false);
            users[i]->birthDate = uObj["birthDate"].toString().toStdString();
            users[i]->githubUsername = uObj["githubUsername"].toString().toStdString();
            users[i]->profileImagePath = uObj["profileImagePath"].toString().toStdString();
        }
    }
    
    auto findUser = [&](int id) -> User* {
        for (int i=0; i<userCount; i++) {
            if (users[i]->userID == id) return users[i];
        }
        return nullptr;
    };

    QJsonArray postsArr = root["posts"].toArray();
    for (int i = 0; i < postsArr.size(); i++) {
        QJsonObject pObj = postsArr[i].toObject();
        int ownerID = pObj["ownerID"].toInt();
        User* owner = findUser(ownerID);
        if (!owner) continue;
        
        Post* p = new Post();
        p->postID = pObj["postID"].toInt();
        p->content = pObj["content"].toString().toStdString();
        p->imagePath = pObj["imagePath"].toString().toStdString();
        p->timestamp = (time_t)pObj["timestamp"].toVariant().toLongLong();
        p->likeCount = pObj["likeCount"].toInt();
        
        QJsonArray likedByArr = pObj["likedBy"].toArray();
        for (int j=0; j<likedByArr.size(); j++) {
            p->likedBy.push_back(likedByArr[j].toInt());
        }
        
        QJsonArray commentsArr = pObj["comments"].toArray();
        p->commentCount = commentsArr.size();
        for (int j=0; j<p->commentCount && j<50; j++) {
            p->comments[j] = commentsArr[j].toString().toStdString();
        }
        
        Post** tempP = new Post*[owner->postCount + 1];
        for (int k = 0; k < owner->postCount; k++) tempP[k] = owner->posts[k];
        tempP[owner->postCount] = p;
        delete[] owner->posts;
        owner->posts = tempP;
        owner->postCount++;
    }

    QJsonObject rel = root["relations"].toObject();
    QJsonArray friendsArr = rel["friends"].toArray();
    for (int i=0; i<friendsArr.size(); i++) {
        QJsonArray edge = friendsArr[i].toArray();
        User* u1 = findUser(edge[0].toInt());
        User* u2 = findUser(edge[1].toInt());
        if (u1 && u2) {
            u1->resize(u1->friends, u1->friendCount);
            u1->friends[u1->friendCount++] = u2;
        }
    }
    QJsonArray reqArr = rel["requests"].toArray();
    for (int i=0; i<reqArr.size(); i++) {
        QJsonArray edge = reqArr[i].toArray();
        User* u1 = findUser(edge[0].toInt());
        User* u2 = findUser(edge[1].toInt());
        if (u1 && u2) {
            u1->resize(u1->request, u1->requestCount);
            u1->request[u1->requestCount++] = u2;
        }
    }
    QJsonArray folArr = rel["followers"].toArray();
    for (int i=0; i<folArr.size(); i++) {
        QJsonArray edge = folArr[i].toArray();
        User* u1 = findUser(edge[0].toInt());
        User* u2 = findUser(edge[1].toInt());
        if (u1 && u2) {
            u1->resize(u1->follower, u1->followerCount);
            u1->follower[u1->followerCount++] = u2;
        }
    }
    QJsonArray followingArr = rel["following"].toArray();
    for (int i=0; i<followingArr.size(); i++) {
        QJsonArray edge = followingArr[i].toArray();
        User* u1 = findUser(edge[0].toInt());
        User* u2 = findUser(edge[1].toInt());
        if (u1 && u2) {
            u1->resize(u1->following, u1->followingCount);
            u1->following[u1->followingCount++] = u2;
        }
    }

    QJsonArray msgArr = root["messages"].toArray();
    for (int i=0; i<msgArr.size(); i++) {
        QJsonObject mObj = msgArr[i].toObject();
        int s = mObj["senderID"].toInt();
        int r = mObj["receiverID"].toInt();
        string text = mObj["text"].toString().toStdString();
        time_t ts = (time_t)mObj["timestamp"].toVariant().toLongLong();
        
        msgSystem.resize();
        msgSystem.msg[msgSystem.msgCount++] = new Message(s, r, text, ts);
    }
    
    notifSystem.loadFromFile("notifications.txt");
    ensureDefaultAdminAccount();
}







