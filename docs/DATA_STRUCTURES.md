# DATA_STRUCTURES.md — Custom Data Structures in CONNECT

One of the defining constraints of this project is that **no STL containers are used**. No `std::vector`, `std::list`, `std::map`, `std::sort`, or `std::find`. Every collection is implemented by hand using raw pointers and manual memory management.

This document explains every custom data structure in the project.

---

## 1. Dynamic Pointer Arrays (User & Post Collections)

The most pervasive structure in the project is a **manually resized raw pointer array**.

### Pattern

```cpp
// Declaration
User** friends;
int friendCount;

// Resize function
void User::resize(User**& u, int count) {
    User** temp = new User*[count + 2];   // allocate one extra slot
    for (int i = 0; i < count; i++)
        temp[i] = u[i];                   // copy existing pointers
    delete[] u;                           // free old array
    u = temp;                             // point to new array
}

// Usage — adding a friend
resize(friends, friendCount);
friends[friendCount++] = newFriend;
```

### Where Used

| Array | Element Type | Owner Class | Purpose |
|---|---|---|---|
| `friends` | `User*` | `User` | Accepted friends |
| `request` | `User*` | `User` | Pending friend requests |
| `follower` | `User*` | `User` | Users who follow this user |
| `following` | `User*` | `User` | Users this user follows |
| `posts` | `Post*` | `User` | Posts created by this user |
| `users` | `User*` | Global (`user.cpp`) | All registered users |
| `msg` | `Message*` | `MessageSystem` | All direct messages |
| `notifications` | `Notification*` | `NotificationSystem` | All notifications |
| `groups` | `Group*` | `GroupSystem` | All group chats |

### Key Properties

- Arrays start with a capacity of 1 (`new User*[1]`)
- On each insert, `resize()` allocates `count + 2` slots (room for one more)
- The old array is `delete[]`'d after copying
- This is essentially a hand-written `std::vector<T*>` with O(n) resize

---

## 2. Singly-Linked List — `Post` (via `next` pointer)

Each `Post` object contains a raw `next` pointer, making posts a singly-linked list when chained.

```cpp
class Post {
public:
    int postID;
    string content;
    Post* next;      // ← linked list pointer
    // ...
};
```

### Usage

Posts are stored in a linked list owned by the `User` class via the `posts` pointer array. When traversing a user's posts in chronological order, the `next` pointer enables sequential access without an index.

### Operations

- **Insert:** Append new post to the end of the chain
- **Delete:** Linear scan to find the node with matching `postID`, unlink it, `delete` it
- **Traverse:** Walk `post->next` until `nullptr`

---

## 3. `MessageSystem` — Dynamic Array of Messages

```cpp
class MessageSystem {
public:
    Message** msg;     // dynamic array of Message pointers
    int msgCount;

    void resize();
    bool sendMessage(User* from, User* to, string text);
    bool sendGroupMessage(User* from, int groupID, const string& text);
    void removeMessagesForGroup(int groupID);
    void viewInbox(User* u);
};
```

The `MessageSystem` manages all messages (both direct and group) in a single flat dynamic array. Filtering by sender/receiver is done at query time with a linear scan.

**Conversation lookup:** To retrieve a conversation between two users, the entire `msg[]` array is scanned and messages where `(senderID == A && receiverID == B) || (senderID == B && receiverID == A)` are collected.

---

## 4. `NotificationSystem` — Dynamic Array with Typed Records

```cpp
class NotificationSystem {
    Notification** notifications;   // dynamic heap array
    int notifCount;

    void resize();   // private — doubles capacity on demand
public:
    // ...
};
```

The `Notification` struct carries a `kind` field (an enum) to distinguish normal notifications from friend-request notifications:

```cpp
enum NotificationKind : int {
    NotifNormal        = 0,
    NotifFriendRequest = 1
};

class Notification {
public:
    int targetUserID;
    string message;
    time_t timestamp;
    bool seen;
    int kind;           // NotificationKind
    int relatedUserID;  // e.g. friend-request sender
};
```

This allows the UI to render friend-request notifications with Accept/Reject buttons while rendering regular notifications as plain text — all from the same array.

---

## 5. `GroupSystem` — Dynamic Array of Groups

```cpp
class GroupSystem {
public:
    Group** groups;
    int groupCount;

    void resizeGroups();
    Group* createGroup(User* creator, const string& name,
                       const string& iconPath,
                       const std::vector<int>& friendUserIDs);
    Group* findGroup(int groupID);
    bool setGroupName(int groupID, User* actor, const string& newName);
    bool addMembersToGroup(int groupID, User* actor,
                           const std::vector<int>& friendUserIDs);
    bool removeMember(int groupID, User* actor, int memberUserID);
};
```

Each `Group` stores its member list as a `std::vector<int>` of user IDs (the only place `std::vector` is used in the codebase, for the member ID list).

---

## 6. Global User Table — `User** users`

```cpp
extern User** users;   // global dynamic array
extern int userCount;
```

This is the master registry of all users. It is:
- Allocated in `user.cpp` as `new User*[1]`
- Grown by `resizeUsers()` whenever a new user signs up
- Searched linearly for login (`login(username, password)`)
- Searched linearly for friend lookup (`findUserByName(name)`)

The entire social graph (friends, followers, following) is stored as pointers into this array, so a change to any `User` object is immediately visible to all other users who hold a pointer to it.

---

## 7. `Post` Comment Array — Fixed-Size Stack Array

```cpp
class Post {
public:
    string comments[50];   // fixed-size array on the heap (part of Post)
    int commentCount;

    void addComment(string c) {
        if (commentCount < 50)
            comments[commentCount++] = c;
    }
};
```

This is a deliberate trade-off: comments use a fixed 50-slot array (no dynamic resize needed for the scope of this project). The `addComment()` method enforces the bound.

---

## 8. `LikedBy` List — `std::vector<int>` inside `Post`

```cpp
class Post {
public:
    std::vector<int> likedBy;   // user IDs who liked this post
    int likeCount;

    bool toggleLike(int userID) {
        for (auto it = likedBy.begin(); it != likedBy.end(); ++it) {
            if (*it == userID) {
                likedBy.erase(it);
                likeCount--;
                return false;   // unliked
            }
        }
        likedBy.push_back(userID);
        likeCount++;
        return true;   // liked
    }
};
```

This is the second place `std::vector` is used. The `toggleLike()` logic uses iterator-based removal to handle the unlike path efficiently.

---

## 9. Persistence — JSON Object as Key-Value Store

`data.json` acts as a flat key-value store for all persistent state. The structure is:

```json
{
    "users":       [ { "userID": 1, "userName": "...", ... } ],
    "posts":       [ { "postID": 1, "ownerID": 1, ... }     ],
    "messages":    [ { "senderID": 1, "receiverID": 2, ... } ],
    "groups":      [ { "groupID": 1, "name": "...", ... }   ],
    "relations": {
        "friends":   [ [1, 2], [2, 3] ],
        "followers":  [ [2, 1] ],
        "following":  [ [1, 2] ],
        "requests":   []
    },
    "banAppeals":    [],
    "userReports":   [],
    "nextID":        3,
    "nextGroupID":   1,
    "userCount":     2
}
```

On load (`loadData()`), this JSON is parsed and all in-memory data structures are populated. On save (`saveData()`), the full in-memory state is serialised back to this file.

---

## Complexity Summary

| Operation | Structure | Time Complexity |
|---|---|---|
| Add user / post | Dynamic pointer array | O(n) resize, amortised O(1) |
| Find user by name | Linear scan of `users[]` | O(n) |
| Find user by ID | Linear scan of `users[]` | O(n) |
| Add friend | Resize + append to `friends[]` | O(n) |
| Get notifications for user | Scan `notifications[]` | O(n) |
| Search posts by keyword | Scan all posts of all users | O(u × p) |
| Toggle like | Scan `likedBy` vector | O(k) where k = like count |
| Add comment | Bounds check + array write | O(1) |
| Send message | Resize + append | O(n) |
