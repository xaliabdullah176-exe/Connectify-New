#include "user.h"
int strLen(string s) {
    int count = 0;
    while (s[count] != '\0') count++;
    return count;
}

bool containsKeyword(string text, string keyword) {
    int tLen = strLen(text);
    int kLen = strLen(keyword);

    if (kLen > tLen) return false;

    for (int i = 0; i <= tLen - kLen; i++) {
        bool match = true;

        for (int j = 0; j < kLen; j++) {
            if (text[i + j] != keyword[j]) {
                match = false;
                break;
            }
        }

        if (match) return true;
    }

    return false;
}

string toLower(string s) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i] + 32;  
    }
    return s;
}
void searchUsers(string keyword) {
    string keyLower = toLower(keyword);
    bool found = false;

    cout << "\n========== USER SEARCH RESULTS ==========\n";

    for (int i = 0; i < userCount; i++) {
        string nameLower = toLower(users[i]->userName);

        if (containsKeyword(nameLower, keyLower)) {
            cout << "User ID  : " << users[i]->userID << endl;
            cout << "Username : " << users[i]->userName << endl;
            cout << "Role     : " << users[i]->role << endl;
            cout << "Followers: " << users[i]->followerCount << endl;
            cout << "Friends  : " << users[i]->friendCount << endl;
            cout << "-----------------------------------------\n";
            found = true;
        }
    }

    if (!found)
        cout << "No users found matching: " << keyword << endl;
}

void searchPosts(string keyword) {
    string keyLower = toLower(keyword);
    bool found = false;

    cout << "\n========== POST SEARCH RESULTS ==========\n";

    for (int i = 0; i < userCount; i++) {
        User* u = users[i];

        for (int j = 0; j < u->postCount; j++) {
            string contentLower = toLower(u->posts[j]->content);

            if (containsKeyword(contentLower, keyLower)) {
                cout << "Posted by : " << u->userName << endl;
                cout << "Post ID   : " << u->posts[j]->postID << endl;
                cout << "Content   : " << u->posts[j]->content << endl;
                cout << "Likes     : " << u->posts[j]->likeCount << endl;

                if (u->posts[j]->commentCount > 0) {
                    cout << "Comments  :" << endl;
                    for (int k = 0; k < u->posts[j]->commentCount; k++)
                        cout << "  - " << u->posts[j]->comments[k] << endl;
                }

                cout << "-----------------------------------------\n";
                found = true;
            }
        }
    }

    if (!found)
        cout << "No posts found matching: " << keyword << endl;
}