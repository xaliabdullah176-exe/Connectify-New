#include "user.h"
#include <ctime>
void MessageSystem::resize() {
    Message** temp = new Message * [msgCount + 1];
    for (int i = 0; i < msgCount; i++)
        temp[i] = msg[i];
    delete[] msg;
    msg = temp;
}
bool MessageSystem::sendMessage(User* from, User* to, string text) {
    if (!from || !to || from == to) {
        cout << "invalid users" << endl;
        return false;
    }

    // check connection friends only
    bool connected = false;
    for (int i = 0; i < from->friendCount; i++) {
        if (from->friends[i] == to) {
            connected = true;
            break;
        }
    }
    if (!connected) {
        cout << "only connected users can message" << endl;
        return false;
    }
    resize();
    msg[msgCount++] = new Message(from->userID, to->userID, text, std::time(nullptr));
    cout << "message sent" << endl;
    return true;
}
void MessageSystem::viewInbox(User* u) {
    if (u == nullptr) return;
    cout << "===== Inbox for " << u->userName << " =====" << endl;
    bool found = false;
    for (int i = 0; i < msgCount; i++) {
        if (msg[i]->receiverID == u->userID) {
            cout << "From ID " << msg[i]->senderID << ": " << msg[i]->text << endl;
            found = true;
        }
    }
    if (!found) cout << "No messages." << endl;
}
