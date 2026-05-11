#include "user.h"
#include <fstream>
#include <ctime>
using namespace std;

NotificationSystem notifSystem;

void NotificationSystem::resize() {
    Notification** temp = new Notification * [notifCount + 1];
    for (int i = 0; i < notifCount; i++)
        temp[i] = notifications[i];
    delete[] notifications;
    notifications = temp;
}

void NotificationSystem::addNotification(int targetID, string msg) {
    resize();
    Notification* n = new Notification();
    n->targetUserID = targetID;
    n->kind = NotifNormal;
    n->relatedUserID = -1;
    n->message = msg;
    n->timestamp = std::time(nullptr);
    notifications[notifCount++] = n;
}

void NotificationSystem::addFriendRequestNotification(int targetUserID, int fromUserID, const string& message) {
    resize();
    Notification* n = new Notification();
    n->targetUserID = targetUserID;
    n->kind = NotifFriendRequest;
    n->relatedUserID = fromUserID;
    n->message = message;
    n->timestamp = std::time(nullptr);
    notifications[notifCount++] = n;
}

void NotificationSystem::removeFriendRequestNotifications(int targetUserID, int fromUserID) {
    int write = 0;
    for (int read = 0; read < notifCount; read++) {
        Notification* n = notifications[read];
        if (n->targetUserID == targetUserID && n->kind == NotifFriendRequest && n->relatedUserID == fromUserID) {
            delete n;
            continue;
        }
        notifications[write++] = n;
    }
    notifCount = write;
}

void NotificationSystem::showNotifications(int userID, string userName) {
    cout << "===== Notifications for " << userName << " =====" << endl;
    bool found = false;
    for (int i = 0; i < notifCount; i++) {
        if (notifications[i]->targetUserID == userID) {
            char buffer[64];
            std::tm* localTime = std::localtime(&notifications[i]->timestamp);
            if (localTime && std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime)) {
                cout << "[" << buffer << "] " << notifications[i]->message << endl;
            } else {
                cout << "[time unavailable] " << notifications[i]->message << endl;
            }
            found = true;
        }
    }
    if (!found) cout << "No notifications." << endl;
}

// ==================== FILE HANDLING ====================
// FORMAT per line: targetUserID|timestamp|message

static string notif_encode(const string& s) {
    string out;
    for (char c : s) {
        if (c == '|')  out += "\\p";
        else if (c == '\n') out += "\\n";
        else if (c == '\\') out += "\\\\";
        else                out += c;
    }
    return out;
}

static string notif_decode(const string& s) {
    string out;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '\\' && i + 1 < (int)s.size()) {
            char next = s[i + 1];
            if (next == 'p') { out += '|';  i++; }
            else if (next == 'n') { out += '\n'; i++; }
            else if (next == '\\') { out += '\\'; i++; }
            else out += s[i];
        }
        else {
            out += s[i];
        }
    }
    return out;
}

void NotificationSystem::saveToFile(const string& filename) {
    ofstream f(filename);
    if (!f) { cout << "[Save] Cannot open " << filename << endl; return; }

    f << notifCount << "\n";
    for (int i = 0; i < notifCount; i++) {
        f << notifications[i]->targetUserID << "|"
            << (long long)notifications[i]->timestamp << "|"
            << (notifications[i]->seen ? 1 : 0) << "|"
            << notifications[i]->kind << "|"
            << notifications[i]->relatedUserID << "|"
            << notif_encode(notifications[i]->message) << "\n";
    }
}

void NotificationSystem::loadFromFile(const string& filename) {
    ifstream f(filename);
    if (!f) return;

    int cnt; f >> cnt; f.ignore();
    for (int i = 0; i < cnt; i++) {
        string line;
        if (!getline(f, line)) break;

        // id|ts|seen|message  (legacy)
        // id|ts|seen|kind|relatedUserID|message  (current)
        size_t p1 = line.find('|');
        if (p1 == string::npos) continue;
        size_t p2 = line.find('|', p1 + 1);
        if (p2 == string::npos) continue;
        size_t p3 = line.find('|', p2 + 1);
        if (p3 == string::npos) continue;

        int    tid  = stoi(line.substr(0, p1));
        time_t ts   = (time_t)stoll(line.substr(p1 + 1, p2 - p1 - 1));
        bool   seen = stoi(line.substr(p2 + 1, p3 - p2 - 1)) != 0;

        string tail = line.substr(p3 + 1);
        int    kind = NotifNormal;
        int    rel  = -1;
        string msg;

        size_t t1 = tail.find('|');
        size_t t2 = (t1 == string::npos) ? string::npos : tail.find('|', t1 + 1);
        if (t1 != string::npos && t2 != string::npos) {
            string kStr = tail.substr(0, t1);
            string rStr = tail.substr(t1 + 1, t2 - t1 - 1);
            if (kStr == "0" || kStr == "1") {
                kind = stoi(kStr);
                rel  = stoi(rStr);
                msg  = notif_decode(tail.substr(t2 + 1));
            } else {
                msg = notif_decode(tail);
            }
        } else {
            msg = notif_decode(tail);
        }

        resize();
        Notification* n = new Notification();
        n->targetUserID = tid;
        n->timestamp    = ts;
        n->seen         = seen;
        n->kind         = kind;
        n->relatedUserID = rel;
        n->message      = msg;
        notifications[notifCount++] = n;
    }
}
