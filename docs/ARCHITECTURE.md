# ARCHITECTURE.md — Connectify System Architecture

## Overview

Connectify is built on a **strict three-layer architecture**. No layer may depend on a layer above it. This design separates domain logic, business logic, and presentation into independent, testable units.

```
┌────────────────────────────────────────────────────┐
│                  UI Layer (Qt6)                    │
│  MainWindow · 8 Pages · 2 Widgets · Session        │
│  LoginPage · SignupPage · NewsFeedPage ·           │
│  ProfilePage · MessagePage · SearchPage ·          │
│  NotificationPage · AdminDashboard                 │
├────────────────────────────────────────────────────┤
│           Manager / Business Logic Layer           │
│  AuthManager · FileManager · NewsFeed ·            │
│  FriendGraph · MessageSystem ·                     │
│  NotificationSystem · SearchEngine · GroupSystem   │
├────────────────────────────────────────────────────┤
│               Model Layer (Pure C++)               │
│  User · NormalUser · Admin · Post · Message ·      │
│  Notification · Group · Custom Data Structures     │
└────────────────────────────────────────────────────┘
```

**Core Rule:** Models know nothing about managers. Managers know nothing about Qt. The UI knows about both but never bypasses the manager layer.

---

## Layer 1 — Model Layer

All domain classes live here. They have **zero Qt dependency** and **zero STL containers**.

### Class Hierarchy

```
User  (base class — virtual destructor)
├── NormalUser     role = "user"
└── Admin          role = "admin"

Post  (standalone — linked list node)
└── (TextPost / ImagePost variants planned for v2)

Message            (value object)
Notification       (value object with seen/unseen state)
Group              (group chat entity)
```

### Key Model: `User`

The `User` class is the heart of the application. It holds:

| Field | Type | Purpose |
|---|---|---|
| `userID` | `int` | Unique identifier |
| `userName` | `string` | Display name |
| `password` | `string` | Stored credential |
| `role` | `string` | `"user"` or `"admin"` |
| `email` | `string` | Contact / login |
| `isBanned` | `bool` | Admin-controlled ban flag |
| `birthDate` | `string` | Profile information |
| `githubUsername` | `string` | Profile link |
| `profileImagePath` | `string` | Avatar image path |
| `friends` | `User**` | Dynamic array of friend pointers |
| `request` | `User**` | Pending friend requests |
| `follower` | `User**` | Users following this user |
| `following` | `User**` | Users this user follows |
| `posts` | `Post**` | Dynamic array of post pointers |

The `User` class implements:
- Custom copy constructor (deep copy of pointer arrays)
- Copy assignment operator (`operator=`)
- Virtual destructor (proper cleanup of all dynamic arrays)
- `resize()` and `resizePosts()` for dynamic array growth

---

## Layer 2 — Manager Layer

Managers implement business rules. They are independent of Qt and interact with models only.

### `NotificationSystem`

Manages a dynamic array of `Notification*` objects. Provides:
- `addNotification(targetID, message)` — append a new notification
- `addFriendRequestNotification(targetID, fromID, message)` — typed notification
- `removeFriendRequestNotifications(targetID, fromID)` — cleanup on rejection
- `showNotifications(userID, userName)` — console display
- `countUnseen(userID)` — badge count for UI
- `markAllSeen(userID)` — mark all as read
- `saveToFile()` / `loadFromFile()` — persistence

### `MessageSystem`

Manages a dynamic array of `Message*` objects with full resize logic:
- `sendMessage(from, to, text)` — create and store message
- `sendGroupMessage(from, groupID, text)` — group chat support
- `viewInbox(user)` — console display of conversations
- `removeMessagesForGroup(groupID)` — group cleanup

### `GroupSystem`

Manages group chats:
- `createGroup(creator, name, iconPath, friendIDs)` — create a new group
- `findGroup(groupID)` — lookup
- `setGroupName(groupID, actor, newName)` — rename (creator only)
- `addMembersToGroup(groupID, actor, friendIDs)` — extend group
- `removeMember(groupID, actor, memberID)` — remove member

### `SearchEngine` (stateless)

- `searchUsers(keyword)` — linear scan of all usernames
- `searchPosts(keyword)` — linear scan of all post contents

---

## Layer 3 — UI Layer

Built entirely with **Qt6 Widgets**. No business logic lives here — all actions go through the manager layer.

### Navigation Model

A single `QStackedWidget` in `MainWindow` holds all pages. Navigation is done by switching the active widget index.

```
MainWindow (QMainWindow)
└── QStackedWidget (stack)
    ├── LoginPage
    ├── SignupPage
    ├── NewsFeedPage
    ├── ProfilePage
    ├── SearchPage
    ├── MessagePage
    ├── NotificationPage
    └── AdminDashboard
```

### Page Descriptions

| Page | File | Responsibility |
|---|---|---|
| `LoginPage` | `frontend/pages/LoginPage` | Username + password form, login signal |
| `SignupPage` | `frontend/pages/SignupPage` | Registration form with validation |
| `NewsFeedPage` | `frontend/pages/newsfeedpage` | Post feed, compose post, like, comment |
| `ProfilePage` | `frontend/pages/profilepage` | View/edit user profile, posts list |
| `SearchPage` | `frontend/pages/searchpage` | Search users and posts by keyword |
| `MessagePage` | `frontend/pages/messagepage` | Inbox, conversations, group chats |
| `NotificationPage` | `frontend/pages/notificationpage` | Notification list, mark-all-read |
| `AdminDashboard` | `frontend/pages/admindashboard` | Stats panel, user table, post moderation, reports, appeals |

### Integration Header

`frontend/integration/mainwindow_integration.h` serves as the **wiring document** for all inter-page signals and slots. It declares:
- All page pointers as members
- All navigation slots (`showLogin()`, `showSignup()`, `showNewsFeed()`, etc.)
- All action slots (`onLoginClicked()`, `onCreatePost()`, `onLikePost()`, etc.)
- Extern references to global backend state (`users`, `userCount`, `nextID`, `msgSystem`, `notifSystem`)

---

## Data Flow — Login Example

```
User types credentials in LoginPage
        │
        ▼ (signal: loginClicked(username, password))
MainWindow::onLoginClicked()
        │
        ▼ calls backend
login(username, password)          ← backend/src/user.cpp
        │
        ▼ returns index
m_loggedInIndex = result
        │
        ▼
if role == "admin" → showAdminDashboard()
else               → showNewsFeed()
        │
        ▼
feedPage->refresh(users[m_loggedInIndex])
```

---

## Data Persistence Flow

```
App Startup → loadData()          [reads data.json → populates users[], posts, relations]
                                  [reads notifications.txt → populates notifSystem]

App Shutdown → saveData()         [serializes all in-memory state → writes data.json]
                                  [saves notifSystem → writes notifications.txt]
```

All data is stored in:
- `data.json` — users, posts, messages, groups, relations, reports, appeals
- `notifications.txt` — notification records

---

## Build System

The project uses **CMake 3.23+** with three build presets:

| Preset | Generator | Output Directory |
|---|---|---|
| `qt-debug` | Ninja | `build-qt-debug/` |
| `qt-mingw-debug` | MinGW Makefiles | `build-mingw-debug/` |
| `msvc-debug` | Visual Studio 18 2026 | `build-msvc-debug/` |

The CMakeLists.txt uses `GLOB_RECURSE` to automatically pick up all `.cpp` files — no manual registration needed when adding new source files.

Include directories exposed to all targets:
- `backend/include` — for `user.h` (also passed to MOC)
- `backend` — for backend source includes
- `frontend/pages` — for page headers
- `frontend/integration` — for the integration header

Linked libraries: `Qt6::Widgets`
