#include "user.h"
#include <fstream>
#include <sstream>
using namespace std;

// ==================== FORWARD DECLARATIONS ====================
extern MessageSystem msgSystem;
extern int nextID;

// ==================== HELPERS ====================

// Escape | and newlines inside strings so we can use | as delimiter
static string encode(const string& s) {
    string out;
    for (char c : s) {
        if (c == '|')  out += "\\p";
        else if (c == '\n') out += "\\n";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
    return out;
}

static string decode(const string& s) {
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

// Split a line by '|'
static vector<string> split(const string& line) {
    vector<string> parts;
    string cur;
    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] == '|') { parts.push_back(cur); cur.clear(); }
        else cur += line[i];
    }
    parts.push_back(cur);
    return parts;
}

// Find user index by ID (used during load)
static int findIndexByID(int id) {
    for (int i = 0; i < userCount; i++)
        if (users[i]->userID == id) return i;
    return -1;
}

// ==================== SAVE ====================

void saveData() {

    // ---------- 1. users.txt ----------
    // FORMAT per user (one line):
    // userID|role|userName|password|postCount
    // then postCount post lines:
    //   postID|likeCount|commentCount|timestamp|content|c0|c1|...
    {
        ofstream f("users.txt");
        if (!f) { cout << "[Save] Cannot open users.txt" << endl; return; }

        f << userCount << "\n";

        for (int i = 0; i < userCount; i++) {
            User* u = users[i];
            f << u->userID << "|"
                << encode(u->role) << "|"
                << encode(u->userName) << "|"
                << encode(u->password) << "|"
                << u->postCount << "\n";

            for (int j = 0; j < u->postCount; j++) {
                Post* p = u->posts[j];
                f << p->postID << "|"
                    << p->likeCount << "|"
                    << p->commentCount << "|"
                    << (long long)p->timestamp << "|"
                    << encode(p->content);
                for (int k = 0; k < p->commentCount; k++)
                    f << "|" << encode(p->comments[k]);
                f << "\n";
            }
        }
    }

    // ---------- 2. relations.txt ----------
    // Stores friend / follower / following / request lists as ID lists
    // FORMAT per user:
    // userID friendCount f0 f1 ... followingCount fo0 fo1 ... followerCount fl0 fl1 ... requestCount r0 r1 ...
    {
        ofstream f("relations.txt");
        if (!f) { cout << "[Save] Cannot open relations.txt" << endl; return; }

        for (int i = 0; i < userCount; i++) {
            User* u = users[i];
            f << u->userID;

            f << " " << u->friendCount;
            for (int j = 0; j < u->friendCount; j++) f << " " << u->friends[j]->userID;

            f << " " << u->followingCount;
            for (int j = 0; j < u->followingCount; j++) f << " " << u->following[j]->userID;

            f << " " << u->followerCount;
            for (int j = 0; j < u->followerCount; j++) f << " " << u->follower[j]->userID;

            f << " " << u->requestCount;
            for (int j = 0; j < u->requestCount; j++) f << " " << u->request[j]->userID;

            f << "\n";
        }
    }

    // ---------- 3. messages.txt ----------
    // FORMAT: senderID|receiverID|text
    {
        ofstream f("messages.txt");
        if (!f) { cout << "[Save] Cannot open messages.txt" << endl; return; }

        f << msgSystem.msgCount << "\n";
        for (int i = 0; i < msgSystem.msgCount; i++) {
            Message* m = msgSystem.msg[i];
            f << m->senderID << "|"
                << m->receiverID << "|"
                << encode(m->text) << "\n";
        }
    }

    // ---------- 4. notifications.txt ----------
    // We access private members via the public save method
    // (see NotificationSystem::saveToFile below)
    notifSystem.saveToFile("notifications.txt");

    // ---------- 5. meta.txt (nextID) ----------
    {
        ofstream f("meta.txt");
        if (!f) { cout << "[Save] Cannot open meta.txt" << endl; return; }
        f << nextID << "\n";
    }

    cout << "[Save] Data saved successfully." << endl;
}

// ==================== LOAD ====================

void loadData() {

    // ---------- 1. meta.txt ----------
    {
        ifstream f("meta.txt");
        if (f) f >> nextID;
    }

    // ---------- 2. users.txt ----------
    {
        ifstream f("users.txt");
        if (!f) return;  // first run, no data yet

        int count = 0;
        f >> count;
        f.ignore();

        for (int i = 0; i < count; i++) {
            string line;
            if (!getline(f, line)) break;

            vector<string> parts = split(line);
            if ((int)parts.size() < 5) continue;

            int    uid = stoi(parts[0]);
            string role = decode(parts[1]);
            string uname = decode(parts[2]);
            string pass = decode(parts[3]);
            int    postCnt = stoi(parts[4]);

            // Create correct subclass
            User* u = nullptr;
            if (role == "admin" || role == "Admin")
                u = new Admin(uid, uname, pass);
            else
                u = new NormalUser(uid, uname, pass);

            // Read posts
            for (int j = 0; j < postCnt; j++) {
                string pline;
                if (!getline(f, pline)) break;
                vector<string> pp = split(pline);
                if ((int)pp.size() < 5) continue;

                int    pid = stoi(pp[0]);
                int    likes = stoi(pp[1]);
                int    cCnt = stoi(pp[2]);
                time_t ts = (time_t)stoll(pp[3]);
                string cont = decode(pp[4]);

                Post* p = new Post(pid, cont);
                p->likeCount = likes;
                p->timestamp = ts;

                for (int k = 0; k < cCnt && (5 + k) < (int)pp.size(); k++)
                    p->addComment(decode(pp[5 + k]));

                // Add post directly (bypass resize pattern for simplicity)
                u->resize((User**&)u->posts, u->postCount);
                u->posts[u->postCount++] = p;
            }

            // Add user to global array
            resizeUsers();
            users[userCount++] = u;
        }
    }

    // ---------- 3. relations.txt ----------
    {
        ifstream f("relations.txt");
        if (f) {
            string line;
            while (getline(f, line)) {
                if (line.empty()) continue;
                istringstream ss(line);

                int uid; ss >> uid;
                int idx = findIndexByID(uid);
                if (idx == -1) continue;
                User* u = users[idx];

                auto readList = [&](User**& arr, int& cnt) {
                    int n; ss >> n;
                    for (int i = 0; i < n; i++) {
                        int tid; ss >> tid;
                        int tidx = findIndexByID(tid);
                        if (tidx == -1) continue;
                        u->resize(arr, cnt);
                        arr[cnt++] = users[tidx];
                    }
                    };

                readList(u->friends, u->friendCount);
                readList(u->following, u->followingCount);
                readList(u->follower, u->followerCount);
                readList(u->request, u->requestCount);
            }
        }
    }

    // ---------- 4. messages.txt ----------
    {
        ifstream f("messages.txt");
        if (f) {
            int cnt; f >> cnt; f.ignore();
            for (int i = 0; i < cnt; i++) {
                string line;
                if (!getline(f, line)) break;
                vector<string> parts = split(line);
                if ((int)parts.size() < 3) continue;
                int sid = stoi(parts[0]);
                int rid = stoi(parts[1]);
                string txt = decode(parts[2]);
                // Resize msg array and add
                Message** temp = new Message * [msgSystem.msgCount + 1];
                for (int j = 0; j < msgSystem.msgCount; j++) temp[j] = msgSystem.msg[j];
                delete[] msgSystem.msg;
                msgSystem.msg = temp;
                msgSystem.msg[msgSystem.msgCount++] = new Message(sid, rid, txt);
            }
        }
    }

    // ---------- 5. notifications.txt ----------
    notifSystem.loadFromFile("notifications.txt");

    if (userCount > 0)
        cout << "[Load] Data loaded successfully. (" << userCount << " users)" << endl;
}
