#include "user.h"
void MessageSystem::resize() {
    Message** temp = new Message * [msgCount + 1];
    for (int i = 0; i < msgCount; i++)
        temp[i] = msg[i];
    delete[] msg;
    msg = temp;
}
void MessageSystem::sendMessage(User* from, User* to, string text) {
    if (!from || !to || from == to) {
        cout << "invalid users" << endl;
        return;
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
        return;
    }
    resize();
    msg[msgCount++] = new Message(from->userID, to->userID, text);
    cout << "message sent" << endl;
}
void MessageSystem::viewInbox(User* u) {
    bool found = false;
    for (int i = 0; i < msgCount; i++) {
        if (msg[i]->receiverID == u->userID) {
            cout << "From ID: " << msg[i]->senderID << endl;
            cout << "Message: " << msg[i]->text << endl;
            cout << "-------------------" << endl;
            found = true;
        }
    }

    if (!found)
        cout << "no messages" << endl;
}
