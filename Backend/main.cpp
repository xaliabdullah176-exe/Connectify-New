#include "user.h"
int main() {
    int choice;
    int loggedInIndex = -1;

    cout << "=== Social Network System ===" << endl;

    while (true) {
        if (loggedInIndex == -1) {
            cout << "\n--- Main Menu ---" << endl;
            cout << "1. Signup" << endl;
            cout << "2. Login" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                int id;
                string username, password, role;
                cout << "Enter User ID: "; cin >> id;
                cout << "Enter Username: "; cin >> username;
                cout << "Enter Password: "; cin >> password;
                cout << "Enter Role (Admin/User): "; cin >> role;
                signup(id, username, password, role);
            }
            else if (choice == 2) {
                string username, password;
                cout << "Enter Username: "; cin >> username;
                cout << "Enter Password: "; cin >> password;
                loggedInIndex = login(username, password);
                if (loggedInIndex == -1)
                    cout << "Invalid credentials" << endl;
                else
                    cout << "Logged in as: " << user[loggedInIndex].userName
                    << " (" << user[loggedInIndex].role << ")" << endl;
            }
            else if (choice == 3) {
                cout << "Goodbye!" << endl;
                break;
            }
            else {
                cout << "Invalid choice" << endl;
            }
        }
        else {
            bool isAdmin = (user[loggedInIndex].role == "Admin" ||
                user[loggedInIndex].role == "admin");

            cout << "\n--- Logged in as: " << user[loggedInIndex].userName << " ---" << endl;
            cout << "1. Send Friend Request" << endl;
            cout << "2. Accept Friend Request" << endl;
            cout << "3. Reject Request" << endl;
            cout << "4. Follow a User" << endl;
            cout << "5. View My Info" << endl;
            cout << "6. Delete My Account" << endl;
            if (isAdmin) {
                cout << "7. [Admin] Display All Users" << endl;
                cout << "8. [Admin] Delete a User" << endl;
            }
            cout << "0. Logout" << endl;
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                string targetName;
                cout << "Enter username to send request: "; cin >> targetName;
                User* target = nullptr;
                for (int i = 0; i < userCount; i++)
                    if (user[i].userName == targetName) { target = &user[i]; break; }
                user[loggedInIndex].sendRequest(target);
            }
            else if (choice == 2) {
                if (user[loggedInIndex].requestCount == 0) {
                    cout << "No pending requests" << endl;
                }
                else {
                    cout << "Pending requests:" << endl;
                    for (int i = 0; i < user[loggedInIndex].requestCount; i++)
                        cout << i + 1 << ". " << user[loggedInIndex].request[i]->userName << endl;
                    string targetName;
                    cout << "Enter username to accept: "; cin >> targetName;
                    User* target = nullptr;
                    for (int i = 0; i < userCount; i++)
                        if (user[i].userName == targetName) { target = &user[i]; break; }
                    user[loggedInIndex].acceptRequest(target);
                }
            }
            else if (choice == 3) {
                string targetName;
                cout << "Enter username to reject: "; cin >> targetName;
                User* target = nullptr;
                for (int i = 0; i < userCount; i++)
                    if (user[i].userName == targetName) { target = &user[i]; break; }
                if (target) user[loggedInIndex].rejectRequest(target);
                else cout << "User not found" << endl;
            }
            else if (choice == 4) {
                string targetName;
                cout << "Enter username to follow: "; cin >> targetName;
                User* target = nullptr;
                for (int i = 0; i < userCount; i++)
                    if (user[i].userName == targetName) { target = &user[i]; break; }
                if (target) user[loggedInIndex].follow(target);
                else cout << "User not found" << endl;
            }
            else if (choice == 5) {
                User& u = user[loggedInIndex];
                cout << "\n--- My Profile ---" << endl;
                cout << "ID        : " << u.userID << endl;
                cout << "Username  : " << u.userName << endl;
                cout << "Role      : " << u.role << endl;
                cout << "Friends   : " << u.friendCount << endl;
                for (int i = 0; i < u.friendCount; i++)
                    cout << "  - " << u.friends[i]->userName << endl;
                cout << "Followers : " << u.followerCount << endl;
                for (int i = 0; i < u.followerCount; i++)
                    cout << "  - " << u.follower[i]->userName << endl;
                cout << "Following : " << u.followingCount << endl;
                for (int i = 0; i < u.followingCount; i++)
                    cout << "  - " << u.following[i]->userName << endl;
                cout << "Pending Requests: " << u.requestCount << endl;
                for (int i = 0; i < u.requestCount; i++)
                    cout << "  - " << u.request[i]->userName << endl;
            }
            else if (choice == 6) {
                deleteAccount(loggedInIndex);
                loggedInIndex = -1;
                cout << "You have been logged out." << endl;
            }
            else if (choice == 7 && isAdmin) {
                display(loggedInIndex);
            }
            else if (choice == 8 && isAdmin) {
                string targetName;
                cout << "Enter username to delete: "; cin >> targetName;
                adminDelete(loggedInIndex, targetName);
                // Re-find admin index after array shift
                loggedInIndex = -1;
                for (int i = 0; i < userCount; i++) {
                    if (user[i].role == "Admin" || user[i].role == "admin") {
                        loggedInIndex = i;
                        break;
                    }
                }
            }
            else if (choice == 0) {
                loggedInIndex = -1;
                cout << "Logged out." << endl;
            }
            else {
                cout << "Invalid choice" << endl;
            }
        }
    }

    delete[] user;
   return 0;
}