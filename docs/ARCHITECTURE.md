# ARCHITECTURE.md — Connectify System Architecture

> **Course:** Object Oriented Programming | BS Software Engineering  
> **Team Leader:** Ali Abdullah (25L-3022)

---

## 1. System Layer Diagram

```
┌──────────────────────────────────────────────────────────────────────┐
│                        PRESENTATION LAYER (Qt Widgets)               │
│  LoginPage  SignupPage  NewsFeedPage  ProfilePage  SearchPage         │
│  MessagePage  NotificationPage  AdminDashboard  MainWindow (router)  │
└──────────────────────────────────┬───────────────────────────────────┘
                                   │  Qt Signals / Slots
┌──────────────────────────────────▼───────────────────────────────────┐
│                        BUSINESS LOGIC LAYER (C++ Backend)            │
│  User  NormalUser  Admin  Post  Message  Group  Notification         │
│  MessageSystem  NotificationSystem  GroupSystem  ModerationSystem    │
└──────────────────────────────────┬───────────────────────────────────┘
                                   │  saveData() / loadData()
┌──────────────────────────────────▼───────────────────────────────────┐
│                        PERSISTENCE LAYER (JSON File I/O)             │
│  data.json (users, posts, relationships, messages, groups)           │
│  notifications.txt (notification records)                            │
└──────────────────────────────────────────────────────────────────────┘
```

---

## 2. Class Diagram — Core Data Model

```mermaid
classDiagram
    class Post {
        +int postID
        +string content
        +string imagePath
        +string videoPath
        +int likeCount
        +vector~int~ likedBy
        +string comments[50]
        +int commentCount
        +Post* next
        +time_t timestamp
        +toggleLike(int userID) bool
        +hasLiked(int userID) bool
        +addComment(string c) void
        +display() void
    }

    class User {
        +int userID
        +string password
        +string userName
        +string role
        +bool isBanned
        +string birthDate
        +string githubUsername
        +string email
        +string profileImagePath
        +User** friends
        +int friendCount
        +User** request
        +int requestCount
        +User** follower
        +int followerCount
        +User** following
        +int followingCount
        +Post** posts
        +int postCount
        +sendRequest(User* u) bool
        +follow(User* to) void
        +acceptRequest(User* u) void
        +rejectRequest(User* u) void
        +createPost(Post* p) void
        +deletePost(int postID) bool
        +resize(User**& u, int count) void
        +resizePosts(Post**& p, int count) void
    }

    class NormalUser {
        +NormalUser()
        +NormalUser(int id, string username, string pass, string em)
    }

    class Admin {
        +Admin()
        +Admin(int id, string username, string pass, string em)
    }

    class Message {
        +int senderID
        +int receiverID
        +string text
        +time_t timestamp
    }

    class Group {
        +int groupID
        +string name
        +string iconPath
        +int creatorID
        +vector~int~ memberIDs
    }

    class Notification {
        +int targetUserID
        +string message
        +time_t timestamp
        +bool seen
        +int kind
        +int relatedUserID
    }

    class UserReport {
        +int reporterID
        +int reportedID
        +string reason
        +time_t timestamp
    }

    class BanAppeal {
        +int userID
        +string message
        +time_t timestamp
    }

    User <|-- NormalUser : inherits
    User <|-- Admin : inherits
    User "1" o-- "0..*" Post : owns (Post**)
    User "1" o-- "0..*" User : friends / followers (User**)
    Message --> User : senderID / receiverID
    Group --> User : creatorID + memberIDs
    Notification --> User : targetUserID
    UserReport --> User : reporterID / reportedID
    BanAppeal --> User : userID
```

---

## 3. Class Diagram — System Managers

```mermaid
classDiagram
    class MessageSystem {
        +Message** msg
        +int msgCount
        +resize() void
        +sendMessage(User* from, User* to, string text) bool
        +sendGroupMessage(User* from, int groupID, string text) bool
        +removeMessagesForGroup(int groupID) void
        +viewInbox(User* u) void
    }

    class NotificationSystem {
        -Notification** notifications
        -int notifCount
        -resize() void
        +addNotification(int targetID, string msg) void
        +addFriendRequestNotification(int target, int from, string msg) void
        +removeFriendRequestNotifications(int target, int from) void
        +countUnseen(int userID) int
        +markAllSeen(int userID) void
        +getCount() int
        +getAt(int i) Notification*
        +saveToFile(string filename) void
        +loadFromFile(string filename) void
    }

    class GroupSystem {
        +Group** groups
        +int groupCount
        +resizeGroups() void
        +createGroup(User* creator, string name, string icon, vector~int~ friendIDs) Group*
        +findGroup(int groupID) Group*
        +setGroupName(int groupID, User* actor, string newName) bool
        +addMembersToGroup(int groupID, User* actor, vector~int~ ids) bool
        +removeMember(int groupID, User* actor, int memberUserID) bool
        +isMember(Group* g, int userID) bool
        +removeUserFromAllGroups(int userID) void
    }

    class ModerationSystem {
        +vector~UserReport~ reports
        +vector~BanAppeal~ pendingAppeals
        +addReport(User* reporter, int reportedUserID, string reason) bool
        +clearReportsAgainst(int reportedUserID) void
        +reportCountAgainst(int reportedUserID) int
        +alreadyReportedPair(int reporterID, int reportedID) bool
        +addAppeal(int userID, string msg) void
        +hasPendingAppeal(int userID) bool
        +removeAppealsFor(int userID) void
    }

    MessageSystem --> Message : manages
    NotificationSystem --> Notification : manages
    GroupSystem --> Group : manages
    ModerationSystem --> UserReport : contains
    ModerationSystem --> BanAppeal : contains
```

---

## 4. Class Diagram — Frontend (Qt Pages)

```mermaid
classDiagram
    class MainWindow {
        -QStackedWidget* stack
        -LoginPage* loginPage
        -SignupPage* signupPage
        -NewsFeedPage* feedPage
        -ProfilePage* profilePage
        -SearchPage* searchPage
        -MessagePage* messagePage
        -AdminDashboard* adminPage
        -NotificationPage* notifPage
        -int currentUserID
        +showLogin() void
        +showSignup() void
        +showFeed() void
        +showProfile(int userID) void
        +showSearch() void
        +showMessages() void
        +showAdmin() void
        +showNotifications() void
        +onCreatePost(string content, string imagePath, string videoPath) void
        +~MainWindow() void
    }

    class LoginPage {
        -QLineEdit* usernameInput
        -QLineEdit* passwordInput
        +loginClicked signal
        +signupClicked signal
        +applyStyles() void
    }

    class SignupPage {
        -QLineEdit* usernameInput
        -QLineEdit* passwordInput
        -QLineEdit* emailInput
        +signupClicked signal
        +loginClicked signal
    }

    class NewsFeedPage {
        -QVBoxLayout* feedLayout
        -QString selectedImagePath
        -QString selectedVideoPath
        -QMediaPlayer* mediaPlayer
        -QVideoWidget* videoWidget
        +loadFeed(int userID) void
        +onPostBtnClicked() void
        +onSelectMediaClicked() void
        +postCreated signal
    }

    class ProfilePage {
        +loadProfile(int viewerID, int targetID) void
        +followClicked signal
        +sendRequestClicked signal
    }

    class SearchPage {
        -QList~SearchUser~ allUsers
        +loadCurrentUser(int userID, ...) void
        +addUser(...) void
        +clearResults() void
        +viewProfileClicked signal
        +addFriendClicked signal
    }

    class MessagePage {
        +loadConversations(int userID) void
        +loadGroupConversations(int userID) void
        +onSendClicked() void
    }

    class AdminDashboard {
        +loadDashboard(int adminID) void
        +onBanUser() void
        +onDeletePost() void
        +onViewReports() void
    }

    class NotificationPage {
        +loadNotifications(int userID) void
        +onAcceptRequest(int fromUserID) void
        +onRejectRequest(int fromUserID) void
    }

    MainWindow --> LoginPage : stacks
    MainWindow --> SignupPage : stacks
    MainWindow --> NewsFeedPage : stacks
    MainWindow --> ProfilePage : stacks
    MainWindow --> SearchPage : stacks
    MainWindow --> MessagePage : stacks
    MainWindow --> AdminDashboard : stacks
    MainWindow --> NotificationPage : stacks
```

---

## 5. Data Flow — Login Sequence

```
User types credentials
        │
        ▼
LoginPage::onLoginClicked()
        │ emit loginClicked(username, password)
        ▼
MainWindow::onLoginClicked(username, password)
        │ calls login(username, password)  [user.cpp]
        ▼
int login(string u, string pass)
        │ iterates users[], compares userName + password
        │ returns userID  (or -1 if fail)
        ▼
MainWindow stores currentUserID
        │ calls showFeed()
        ▼
NewsFeedPage::loadFeed(currentUserID)
        │ iterates users[i]->following for the current user
        │ renders FeedPostCard for each Post*
        ▼
UI displays news feed
```

---

## 6. Data Flow — Post Creation Sequence

```
User types content, optionally selects image/video
        │
        ▼
NewsFeedPage::onPostBtnClicked()
        │ validates: content OR media must be non-empty
        │ emit postCreated(content, imagePath, videoPath)
        ▼
MainWindow::onCreatePost(content, imagePath, videoPath)
        │ finds User* me = users[currentUserID]
        │ creates Post* p = new Post(nextPostID++, content, imagePath, videoPath)
        │ calls me->createPost(p)
        ▼
User::createPost(Post* p)
        │ calls resizePosts(posts, postCount)
        │ posts[postCount++] = p
        ▼
MainWindow::saveData()  (called on exit)
        │ serializes all users, posts, relationships to data.json
        ▼
data.json updated on disk
```

---

## 7. Data Flow — Friend Request Sequence

```
User A clicks "+ Add Friend" on User B's profile
        │
        ▼
SearchPage / ProfilePage emits addFriendClicked(userBID)
        │
        ▼
MainWindow::onAddFriend(userBID)
        │ finds User* a = users[currentUserID]
        │ finds User* b = findUser(userBID)
        │ calls a->sendRequest(b)
        ▼
User::sendRequest(User* u)
        │ resizes request array of target user u
        │ u->request[u->requestCount++] = this
        │ calls notifSystem.addFriendRequestNotification(u->userID, this->userID, ...)
        ▼
User B logs in → NotificationPage::loadNotifications()
        │ shows pending friend request
        │ User B clicks Accept
        ▼
MainWindow::onAcceptFriendRequest(userAID)
        │ finds both users
        │ calls b->acceptRequest(a)
        ▼
User::acceptRequest(User* u)
        │ adds each to other's friends[] array
        │ removes from request[] array
        │ calls notifSystem.removeFriendRequestNotifications(...)
```

---

## 8. Memory Management Architecture

```
Global Heap
├── User** users[userCount]
│     ├── users[0] → NormalUser { Post** posts[postCount] }
│     │                              ├── posts[0] → Post { comments[50] }
│     │                              └── posts[n] → Post
│     ├── users[1] → Admin { ... }
│     └── users[n] → NormalUser { ... }
│
├── MessageSystem msgSystem
│     └── Message** msg[msgCount]
│
├── NotificationSystem notifSystem
│     └── Notification** notifications[notifCount]
│
└── GroupSystem groupSystem
      └── Group** groups[groupCount]

Cleanup on exit: MainWindow::~MainWindow()
  ├── saveData()          → flush all data to data.json
  └── freeAllData()       → delete all users[i]; delete[] users;
                            (MessageSystem, NotificationSystem, GroupSystem
                             self-clean via their own destructors)
```

---

## 9. OOP Concepts Map

| Concept | Location | Example |
|---|---|---|
| **Classes & Objects** | All backend files | `Post`, `User`, `Message`, `Group` |
| **Inheritance** | `user.h:175-209` | `NormalUser : public User`, `Admin : public User` |
| **Encapsulation** | All classes | `password`, `isBanned` accessed only via class methods |
| **Polymorphism** | `user.h:140` | `virtual ~User()` — vtable dispatch on delete |
| **Abstraction** | `Post`, `User` | Uniform `toggleLike()`, `addComment()` hide implementation |
| **Dynamic Memory** | `user.h:133-137` | `friends = new User*[1]` with manual `resize()` |
| **Rule of Three** | `user.h:152-153` | Copy ctor + assignment `= delete` |
| **Destructors** | `user.h:140-149` | Cascading `delete` of owned Post objects |
| **File I/O** | `user.cpp` | `saveData()` / `loadData()` via `QJsonDocument` |
| **GUI / Event-Driven** | All frontend files | Qt Signals & Slots replacing callbacks |
