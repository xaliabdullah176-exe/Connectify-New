# CLASS_REFERENCE.md — Complete Class & API Reference

This document lists every class in the CONNECT project with its public interface, file location, and purpose.

---

## Backend Classes (`backend/include/user.h`)

---

### `Post`

A single social media post. Acts as a linked list node via the `next` pointer.

**File:** `backend/include/user.h`, `backend/src/post.cpp`

**Fields**

| Field | Type | Description |
|---|---|---|
| `postID` | `int` | Unique identifier |
| `content` | `string` | Text body |
| `imagePath` | `string` | Optional image path |
| `likeCount` | `int` | Cached like count |
| `likedBy` | `vector<int>` | User IDs who liked the post |
| `comments[50]` | `string[]` | Fixed-size comment array |
| `commentCount` | `int` | Number of comments stored |
| `next` | `Post*` | Next post in linked list |
| `timestamp` | `time_t` | Unix time of creation |

**Methods**

| Method | Returns | Description |
|---|---|---|
| `Post()` | — | Default constructor |
| `Post(int id, string c, string imgPath="")` | — | Parameterised constructor |
| `toggleLike(int userID)` | `bool` | Like if not liked, unlike if liked. Returns new state. |
| `hasLiked(int userID)` | `bool` | Check if a user has liked this post |
| `addComment(string c)` | `void` | Append comment (max 50) |
| `display()` | `void` | Print post to console |

---

### `User`

Base class for all user accounts. Contains the full social graph.

**File:** `backend/include/user.h`, `backend/src/user.cpp`

**Fields**

| Field | Type | Description |
|---|---|---|
| `userID` | `int` | Unique identifier |
| `userName` | `string` | Display name |
| `password` | `string` | Login credential |
| `role` | `string` | `"user"` or `"admin"` |
| `isBanned` | `bool` | Ban flag |
| `birthDate` | `string` | Profile info |
| `githubUsername` | `string` | GitHub link |
| `email` | `string` | Email address |
| `profileImagePath` | `string` | Avatar path |
| `friends` | `User**` | Dynamic array of friend pointers |
| `friendCount` | `int` | Number of friends |
| `request` | `User**` | Pending incoming requests |
| `requestCount` | `int` | Number of pending requests |
| `follower` | `User**` | Users following this user |
| `followerCount` | `int` | Follower count |
| `following` | `User**` | Users this user follows |
| `followingCount` | `int` | Following count |
| `posts` | `Post**` | Dynamic array of posts |
| `postCount` | `int` | Number of posts |

**Methods**

| Method | Returns | Description |
|---|---|---|
| `User()` | — | Default constructor — allocates all pointer arrays |
| `virtual ~User()` | — | Destructor — frees all arrays and posts |
| `User(const User&)` | — | Copy constructor — deep copies arrays |
| `operator=(const User&)` | `User&` | Copy assignment with self-assignment guard |
| `resize(User**&, int)` | `void` | Grow a `User**` array by one slot |
| `resizePosts(Post**&, int)` | `void` | Grow a `Post**` array by one slot |
| `sendRequest(User* u)` | `bool` | Send a friend request to user `u` |
| `follow(User* to)` | `void` | Follow another user |
| `acceptRequest(User* u)` | `void` | Accept request from `u` — makes both friends |
| `rejectRequest(User* u)` | `void` | Reject and remove request from `u` |
| `createPost(Post* p)` | `void` | Add a post to this user's post list |
| `deletePost(int postID)` | `bool` | Remove and delete a post by ID |
| `showPosts()` | `void` | Print all posts to console |
| `showNewsFeed()` | `void` | Print posts from followed users |

---

### `NormalUser : public User`

A standard user account.

**File:** `backend/include/user.h`

| Constructor | Description |
|---|---|
| `NormalUser()` | Sets `role = "user"` |
| `NormalUser(int id, string username, string pass, string em="")` | Full parameterised constructor |

---

### `Admin : public User`

An administrator account with elevated privileges.

**File:** `backend/include/user.h`

| Constructor | Description |
|---|---|
| `Admin()` | Sets `role = "admin"` |
| `Admin(int id, string username, string pass, string em="")` | Full parameterised constructor |

---

### `Message`

A value object representing one direct or group message.

**File:** `backend/include/user.h`

| Field | Type | Description |
|---|---|---|
| `senderID` | `int` | Sender's user ID |
| `receiverID` | `int` | Receiver's user ID (negative = group ID) |
| `text` | `string` | Message content |
| `timestamp` | `time_t` | Send time |

| Constructor | Description |
|---|---|
| `Message(int s, int r, string t, time_t ts=0)` | Full parameterised constructor |

---

### `MessageSystem`

Manages all direct and group messages in a dynamic array.

**File:** `backend/include/user.h`, `backend/src/message.cpp`

| Field | Type | Description |
|---|---|---|
| `msg` | `Message**` | Dynamic array of message pointers |
| `msgCount` | `int` | Total message count |

| Method | Returns | Description |
|---|---|---|
| `MessageSystem()` | — | Allocates initial `msg` array |
| `~MessageSystem()` | — | Deletes all messages and the array |
| `resize()` | `void` | Grow the message array by one slot |
| `sendMessage(User*, User*, string)` | `bool` | Create and store a direct message |
| `sendGroupMessage(User*, int groupID, string)` | `bool` | Create and store a group message |
| `removeMessagesForGroup(int groupID)` | `void` | Delete all messages for a group |
| `viewInbox(User*)` | `void` | Print all messages for a user |

---

### `Notification`

A single notification record with type information.

**File:** `backend/include/user.h`

| Field | Type | Description |
|---|---|---|
| `targetUserID` | `int` | Who receives this notification |
| `message` | `string` | Notification text |
| `timestamp` | `time_t` | Creation time |
| `seen` | `bool` | Whether the user has seen it |
| `kind` | `int` | `NotifNormal (0)` or `NotifFriendRequest (1)` |
| `relatedUserID` | `int` | For friend requests: the sender's user ID |

---

### `NotificationSystem`

Manages all notifications for all users.

**File:** `backend/include/user.h`, `backend/src/notification.cpp`

| Method | Returns | Description |
|---|---|---|
| `NotificationSystem()` | — | Constructor |
| `~NotificationSystem()` | — | Frees all notifications |
| `addNotification(int targetID, string msg)` | `void` | Create a normal notification |
| `addFriendRequestNotification(int targetID, int fromID, string msg)` | `void` | Create a typed friend-request notification |
| `removeFriendRequestNotifications(int targetID, int fromID)` | `void` | Remove request notification on reject/accept |
| `showNotifications(int userID, string userName)` | `void` | Print to console |
| `getCount()` | `int` | Total notification count |
| `getAt(int i)` | `Notification*` | Get notification by index |
| `countUnseen(int userID)` | `int` | Count unseen notifications for a user |
| `markAllSeen(int userID)` | `void` | Mark all of a user's notifications as seen |
| `saveToFile(string filename)` | `void` | Persist to file |
| `loadFromFile(string filename)` | `void` | Load from file |

---

### `Group`

A group chat entity.

**File:** `backend/include/user.h`

| Field | Type | Description |
|---|---|---|
| `groupID` | `int` | Unique identifier |
| `name` | `string` | Display name |
| `iconPath` | `string` | Optional icon image path |
| `creatorID` | `int` | User ID of the creator |
| `memberIDs` | `vector<int>` | Member user IDs |

---

### `GroupSystem`

Manages all group chats.

**File:** `backend/include/user.h`, `backend/src/`

| Method | Returns | Description |
|---|---|---|
| `GroupSystem()` | — | Constructor |
| `~GroupSystem()` | — | Frees all groups |
| `resizeGroups()` | `void` | Grow the group array |
| `createGroup(User*, string name, string icon, vector<int> memberIDs)` | `Group*` | Create a new group |
| `findGroup(int groupID)` | `Group*` | Lookup by ID |
| `setGroupName(int groupID, User*, string newName)` | `bool` | Rename (creator only) |
| `addMembersToGroup(int groupID, User*, vector<int> friendIDs)` | `bool` | Add members |
| `removeMember(int groupID, User*, int memberUserID)` | `bool` | Remove a member |

---

## Global Functions (`backend/src/user.cpp`)

| Function | Signature | Description |
|---|---|---|
| `userNameExist` | `bool(string n)` | Check if username is taken |
| `emailTaken` | `bool(string e, int exceptUserID=-1)` | Check if email is taken |
| `adminExist` | `bool()` | Check if any admin account exists |
| `resizeUsers` | `void()` | Grow the global `users[]` array |
| `signup` | `void(int id, string user, string pass, string role, string email)` | Create and register a new user |
| `login` | `int(string u, string pass)` | Authenticate — returns index or -1 |
| `removeUserReferences` | `void(int targetID)` | Clean up all social graph references to a user |
| `deleteAccount` | `void(int index)` | Remove a user from the global array |
| `adminDelete` | `void(int adminIndex, string u)` | Admin-initiated user deletion |
| `display` | `void(int adminIndex)` | Display all users (admin only) |
| `searchUsers` | `void(string keyword)` | Search and print matching users |
| `searchPosts` | `void(string keyword)` | Search and print matching posts |
| `loadData` | `void()` | Load all data from `data.json` and `notifications.txt` |
| `saveData` | `void()` | Save all data to `data.json` |
| `ensureDefaultAdminAccount` | `void()` | Recreate default admin if missing |

---

## Frontend Classes (Qt)

### `MainWindow : QMainWindow`

**File:** `frontend/pages/mainwindow.h/.cpp`

Root window. Contains the `QStackedWidget` and all page instances. Wires all navigation signals and action slots.

### `LoginPage : QWidget`

**File:** `frontend/pages/LoginPage.h/.cpp`

Login form. Emits signal on form submit → `MainWindow::onLoginClicked()`.

### `SignupPage : QWidget`

**File:** `frontend/pages/signuppage.h/.cpp`

Registration form. Emits signal on submit → `MainWindow::onSignupClicked()`.

### `NewsFeedPage : QWidget`

**File:** `frontend/pages/newsfeedpage.h/.cpp`

Displays the post feed for the logged-in user. Handles post creation, likes, and comments.

### `ProfilePage : QWidget`

**File:** `frontend/pages/profilepage.h/.cpp`

Displays and allows editing of a user's profile (name, bio, avatar, GitHub, birth date).

### `SearchPage : QWidget`

**File:** `frontend/pages/searchpage.h/.cpp`

Keyword search for users and posts. Displays results dynamically.

### `MessagePage : QWidget`

**File:** `frontend/pages/messagepage.h/.cpp`

Inbox view with conversation list. Chat panel shows message bubbles.

### `NotificationPage : QWidget`

**File:** `frontend/pages/notificationpage.h/.cpp`

Lists all notifications for the logged-in user. Provides mark-all-read action.

### `AdminDashboard : QWidget`

**File:** `frontend/pages/admindashboard.h/.cpp`

Full admin control panel. Key public methods:

| Method | Description |
|---|---|
| `loadAdmin(int adminID, QString adminName)` | Initialise with admin's info |
| `setStats(int users, int posts, int banned, int review)` | Update the stats panel |
| `addUserRow(int userID, ...)` | Add a row to the user table |
| `addModPost(int postID, ...)` | Add a card to the moderation grid |
| `clearUsers()` | Remove all user table rows |
| `clearModPosts()` | Remove all moderation post cards |
| `addReportRow(int reportedUserID, ...)` | Add a user report card |
| `addAppealRow(int userID, ...)` | Add a ban appeal card |
| `refreshUserSearch()` | Re-apply the search filter |
