#include "user.h"

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
    n->timestamp = time(0);
    notifications[notifCount++] = n;
}

void NotificationSystem::showNotifications(int userID, string userName) {
    cout << "===== Notifications for " << userName << " =====" << endl;
    bool found = false;
    for (int i = 0; i < notifCount; i++) {
        if (notifications[i]->targetUserID == userID) {
            char buffer[26];
            ctime_s(buffer, sizeof(buffer), &notifications[i]->timestamp);
            cout << "[" << buffer << "] " << notifications[i]->message << endl;
            found = true;
        }
    }
    if (!found) cout << "No notifications." << endl;
}