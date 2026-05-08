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
    n->message = msg;
    n->timestamp = std::time(nullptr);
    notifications[notifCount++] = n;
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

        // split on first THREE '|' separators: id|ts|seen|message
        size_t p1 = line.find('|');
        if (p1 == string::npos) continue;
        size_t p2 = line.find('|', p1 + 1);
        if (p2 == string::npos) continue;
        size_t p3 = line.find('|', p2 + 1);
        if (p3 == string::npos) continue;

        int    tid  = stoi(line.substr(0, p1));
        time_t ts   = (time_t)stoll(line.substr(p1 + 1, p2 - p1 - 1));
        bool   seen = stoi(line.substr(p2 + 1, p3 - p2 - 1)) != 0;
        string msg  = notif_decode(line.substr(p3 + 1));

        resize();
        Notification* n = new Notification();
        n->targetUserID = tid;
        n->timestamp    = ts;
        n->seen         = seen;
        n->message      = msg;
        notifications[notifCount++] = n;
    }
}
