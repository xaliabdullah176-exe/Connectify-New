#include <iostream>
#include "user.h"
#include <algorithm>
#include <ctime>
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
int    nextGroupID = 1;
MessageSystem msgSystem;
GroupSystem groupSystem;
ModerationSystem modSystem;
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

bool User::sendRequest(User* u) {
    if (u == nullptr) { cout << "User not found." << endl; return false; }
    if (u == this) { cout << "Cannot send request to yourself." << endl; return false; }

    for (int i = 0; i < friendCount; i++)
        if (friends[i] == u) { cout << "Already friends." << endl; return false; }

    for (int i = 0; i < requestCount; i++)
        if (request[i] == u) { cout << "They already sent you a request — accept it instead." << endl; return false; }

    for (int i = 0; i < u->requestCount; i++)
        if (u->request[i] == this) { cout << "Request already sent." << endl; return false; }

    u->resize(u->request, u->requestCount);
    u->request[u->requestCount++] = this;
    cout << "Request sent successfully." << endl;
    return true;
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

bool emailTaken(string e, int exceptUserID) {
    if (e.empty()) return false;
    for (int i = 0; i < userCount; i++) {
        if (exceptUserID >= 0 && users[i]->userID == exceptUserID) continue;
        if (users[i]->email == e) return true;
    }
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

void signup(int id, string usern, string pass, string rol, string email) {
    if (userNameExist(usern)) { cout << "Username already exists." << endl; return; }
    if (!email.empty() && emailTaken(email, -1)) {
        cout << "Email already in use." << endl;
        return;
    }

    string roleLower = rol;
    for (char& c : roleLower) c = tolower(c);

    if (roleLower == "admin" && adminExist()) { cout << "Admin already exists." << endl; return; }

    resizeUsers();

    if (roleLower == "admin")
        users[userCount] = new Admin(id, usern, pass, email);
    else
        users[userCount] = new NormalUser(id, usern, pass, email);

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
    groupSystem.removeUserFromAllGroups(targetID);
    modSystem.reports.erase(
        std::remove_if(modSystem.reports.begin(), modSystem.reports.end(),
            [targetID](const UserReport& r) {
                return r.reporterID == targetID || r.reportedID == targetID;
            }),
        modSystem.reports.end());
    modSystem.pendingAppeals.erase(
        std::remove_if(modSystem.pendingAppeals.begin(), modSystem.pendingAppeals.end(),
            [targetID](const BanAppeal& a) { return a.userID == targetID; }),
        modSystem.pendingAppeals.end());
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

// ==================== GROUP SYSTEM ====================
GroupSystem::GroupSystem() {
    groups = new Group*[1];
    groupCount = 0;
}

GroupSystem::~GroupSystem() {
    for (int i = 0; i < groupCount; i++)
        delete groups[i];
    delete[] groups;
}

void GroupSystem::resizeGroups() {
    Group** temp = new Group*[groupCount + 1];
    for (int i = 0; i < groupCount; i++)
        temp[i] = groups[i];
    delete[] groups;
    groups = temp;
}

bool GroupSystem::isMember(Group* g, int userID) {
    if (!g) return false;
    for (int id : g->memberIDs)
        if (id == userID) return true;
    return false;
}

Group* GroupSystem::findGroup(int groupID) {
    for (int i = 0; i < groupCount; i++)
        if (groups[i]->groupID == groupID)
            return groups[i];
    return nullptr;
}

Group* GroupSystem::createGroup(User* creator, const string& name, const string& iconPath,
                                const vector<int>& friendUserIDs) {
    if (!creator || name.empty()) return nullptr;

    vector<int> members;
    members.push_back(creator->userID);

    for (int fid : friendUserIDs) {
        if (fid == creator->userID) continue;
        bool isFriend = false;
        for (int i = 0; i < creator->friendCount; i++)
            if (creator->friends[i]->userID == fid) { isFriend = true; break; }
        if (!isFriend) continue;
        bool dup = false;
        for (int m : members)
            if (m == fid) { dup = true; break; }
        if (!dup) members.push_back(fid);
    }

    if (members.size() < 2) return nullptr;

    resizeGroups();
    Group* g = new Group();
    g->groupID   = nextGroupID++;
    g->name      = name;
    g->iconPath  = iconPath;
    g->creatorID = creator->userID;
    g->memberIDs = members;
    groups[groupCount++] = g;
    return g;
}

bool GroupSystem::setGroupName(int groupID, User* actor, const string& newName) {
    if (newName.empty()) return false;
    Group* g = findGroup(groupID);
    if (!g || !actor || !isMember(g, actor->userID)) return false;
    g->name = newName;
    return true;
}

bool GroupSystem::addMembersToGroup(int groupID, User* actor, const vector<int>& friendUserIDs) {
    Group* g = findGroup(groupID);
    if (!g || !actor || !isMember(g, actor->userID)) return false;

    bool any = false;
    for (int fid : friendUserIDs) {
        if (fid == actor->userID) continue;
        if (isMember(g, fid)) continue;

        bool isFriend = false;
        for (int i = 0; i < actor->friendCount; i++)
            if (actor->friends[i]->userID == fid) { isFriend = true; break; }
        if (!isFriend) continue;

        g->memberIDs.push_back(fid);
        any = true;
    }
    return any;
}

bool GroupSystem::removeMember(int groupID, User* actor, int memberUserID) {
    Group* g = findGroup(groupID);
    if (!g || !actor || actor->userID != g->creatorID) return false;
    if (memberUserID == actor->userID) return false;
    auto& v = g->memberIDs;
    auto it = std::find(v.begin(), v.end(), memberUserID);
    if (it == v.end()) return false;
    v.erase(it);
    return true;
}

int getFirstAdminUserID() {
    for (int i = 0; i < userCount; i++)
        if (users[i] && (users[i]->role == "admin" || users[i]->role == "Admin"))
            return users[i]->userID;
    return -1;
}

bool ModerationSystem::alreadyReportedPair(int reporterID, int reportedID) const {
    for (const auto& r : reports)
        if (r.reporterID == reporterID && r.reportedID == reportedID) return true;
    return false;
}

bool ModerationSystem::addReport(User* reporter, int reportedUserID) {
    if (!reporter || reportedUserID == reporter->userID) return false;
    if (alreadyReportedPair(reporter->userID, reportedUserID)) return false;
    UserReport ur;
    ur.reporterID = reporter->userID;
    ur.reportedID = reportedUserID;
    ur.timestamp = std::time(nullptr);
    reports.push_back(ur);
    return true;
}

void ModerationSystem::clearReportsAgainst(int reportedUserID) {
    reports.erase(
        std::remove_if(reports.begin(), reports.end(),
            [reportedUserID](const UserReport& r) { return r.reportedID == reportedUserID; }),
        reports.end());
}

int ModerationSystem::reportCountAgainst(int reportedUserID) const {
    int c = 0;
    for (const auto& r : reports)
        if (r.reportedID == reportedUserID) c++;
    return c;
}

bool ModerationSystem::hasPendingAppeal(int userID) const {
    for (const auto& a : pendingAppeals)
        if (a.userID == userID) return true;
    return false;
}

void ModerationSystem::addAppeal(int userID, const string& msg) {
    if (msg.empty()) return;
    if (hasPendingAppeal(userID)) return;
    BanAppeal a;
    a.userID = userID;
    a.message = msg;
    a.timestamp = std::time(nullptr);
    pendingAppeals.push_back(a);
}

void ModerationSystem::removeAppealsFor(int userID) {
    pendingAppeals.erase(
        std::remove_if(pendingAppeals.begin(), pendingAppeals.end(),
            [userID](const BanAppeal& a) { return a.userID == userID; }),
        pendingAppeals.end());
}

void GroupSystem::removeUserFromAllGroups(int userID) {
    int i = 0;
    while (i < groupCount) {
        Group* g = groups[i];
        auto& v = g->memberIDs;
        v.erase(std::remove(v.begin(), v.end(), userID), v.end());
        if (v.empty()) {
            const int gid = g->groupID;
            msgSystem.removeMessagesForGroup(gid);
            delete g;
            for (int j = i; j < groupCount - 1; j++)
                groups[j] = groups[j + 1];
            groupCount--;
        } else {
            ++i;
        }
    }
}

void saveData() {
    QJsonObject root;
    root["nextID"] = nextID;
    root["nextGroupID"] = nextGroupID;
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
        uObj["email"] = QString::fromStdString(users[i]->email);
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

    QJsonArray groupsArr;
    for (int i = 0; i < groupSystem.groupCount; i++) {
        Group* g = groupSystem.groups[i];
        QJsonObject go;
        go["groupID"] = g->groupID;
        go["name"] = QString::fromStdString(g->name);
        go["iconPath"] = QString::fromStdString(g->iconPath);
        go["creatorID"] = g->creatorID;
        QJsonArray memb;
        for (int id : g->memberIDs) memb.append(id);
        go["memberIDs"] = memb;
        groupsArr.append(go);
    }
    root["groups"] = groupsArr;

    QJsonArray repArr;
    for (const auto& r : modSystem.reports) {
        QJsonObject o;
        o["reporterID"] = r.reporterID;
        o["reportedID"] = r.reportedID;
        o["timestamp"] = (qint64)r.timestamp;
        repArr.append(o);
    }
    root["userReports"] = repArr;

    QJsonArray appArr;
    for (const auto& a : modSystem.pendingAppeals) {
        QJsonObject o;
        o["userID"] = a.userID;
        o["message"] = QString::fromStdString(a.message);
        o["timestamp"] = (qint64)a.timestamp;
        appArr.append(o);
    }
    root["banAppeals"] = appArr;

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
    nextGroupID = root["nextGroupID"].toInt(1);

    for (int i = 0; i < groupSystem.groupCount; i++)
        delete groupSystem.groups[i];
    delete[] groupSystem.groups;
    groupSystem.groups = new Group*[1];
    groupSystem.groupCount = 0;

    QJsonArray groupsArr = root["groups"].toArray();
    for (int i = 0; i < groupsArr.size(); i++) {
        QJsonObject go = groupsArr[i].toObject();
        Group* g = new Group();
        g->groupID = go["groupID"].toInt();
        g->name = go["name"].toString().toStdString();
        g->iconPath = go["iconPath"].toString().toStdString();
        g->creatorID = go["creatorID"].toInt();
        QJsonArray memb = go["memberIDs"].toArray();
        for (int j = 0; j < memb.size(); j++)
            g->memberIDs.push_back(memb[j].toInt());

        Group** tempG = new Group*[groupSystem.groupCount + 1];
        for (int k = 0; k < groupSystem.groupCount; k++)
            tempG[k] = groupSystem.groups[k];
        tempG[groupSystem.groupCount] = g;
        delete[] groupSystem.groups;
        groupSystem.groups = tempG;
        groupSystem.groupCount++;
    }
    int maxGroupId = 0;
    for (int gi = 0; gi < groupSystem.groupCount; gi++)
        if (groupSystem.groups[gi]->groupID > maxGroupId)
            maxGroupId = groupSystem.groups[gi]->groupID;
    if (nextGroupID <= maxGroupId)
        nextGroupID = maxGroupId + 1;

    modSystem.reports.clear();
    modSystem.pendingAppeals.clear();
    QJsonArray repArr = root["userReports"].toArray();
    for (int ri = 0; ri < repArr.size(); ri++) {
        QJsonObject o = repArr[ri].toObject();
        UserReport ur;
        ur.reporterID = o["reporterID"].toInt();
        ur.reportedID = o["reportedID"].toInt();
        ur.timestamp = (time_t)o["timestamp"].toVariant().toLongLong();
        modSystem.reports.push_back(ur);
    }
    QJsonArray appArr = root["banAppeals"].toArray();
    for (int ai = 0; ai < appArr.size(); ai++) {
        QJsonObject o = appArr[ai].toObject();
        BanAppeal ba;
        ba.userID = o["userID"].toInt();
        ba.message = o["message"].toString().toStdString();
        ba.timestamp = (time_t)o["timestamp"].toVariant().toLongLong();
        modSystem.pendingAppeals.push_back(ba);
    }
    
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
            users[i]->email = uObj["email"].toString().toStdString();
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







