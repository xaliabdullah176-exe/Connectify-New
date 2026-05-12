# DATA_FORMAT.md — Persistence Layer

CONNECT uses two files for data persistence: `data.json` for all structured data, and `notifications.txt` for notification records.

---

## `data.json` — Main Data Store

All application data is read from this file on startup (`loadData()`) and written back on shutdown (`saveData()`). The file is created automatically if it does not exist.

### Top-Level Schema

```json
{
    "users":       [ ... ],
    "posts":       [ ... ],
    "messages":    [ ... ],
    "groups":      [ ... ],
    "relations": {
        "friends":   [ ... ],
        "followers": [ ... ],
        "following": [ ... ],
        "requests":  [ ... ]
    },
    "banAppeals":    [ ... ],
    "userReports":   [ ... ],
    "nextID":        3,
    "nextGroupID":   1,
    "userCount":     2
}
```

---

### `users` Array

Each element represents one registered user account.

```json
{
    "userID":           1,
    "userName":         "Alice",
    "password":         "SecurePass123",
    "role":             "user",
    "email":            "alice@example.com",
    "birthDate":        "2000-01-15",
    "githubUsername":   "alice-dev",
    "profileImagePath": "C:/Users/Alice/Pictures/avatar.png",
    "isBanned":         false
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `userID` | int | Yes | Unique auto-incremented identifier |
| `userName` | string | Yes | Display name and login identifier |
| `password` | string | Yes | Plain-text credential (stored as-is in v1) |
| `role` | string | Yes | `"user"` or `"admin"` |
| `email` | string | No | Optional contact email |
| `birthDate` | string | No | Optional profile info |
| `githubUsername` | string | No | Optional GitHub link |
| `profileImagePath` | string | No | Absolute path to avatar image |
| `isBanned` | bool | Yes | Admin-controlled ban flag |

---

### `posts` Array

Each element represents one post created by a user.

```json
{
    "postID":    101,
    "ownerID":   1,
    "content":   "Hello everyone, this is my first post!",
    "imagePath": "",
    "timestamp": 1748700000,
    "likeCount": 3,
    "likedBy":   [2, 4, 7],
    "comments":  ["Bob: Great post!", "Carol: Welcome!"]
}
```

| Field | Type | Description |
|---|---|---|
| `postID` | int | Unique post identifier |
| `ownerID` | int | `userID` of the post creator |
| `content` | string | Text body of the post |
| `imagePath` | string | Path to attached image (empty if text-only) |
| `timestamp` | long | Unix epoch (seconds since 1970-01-01) |
| `likeCount` | int | Cached count (authoritative source is `likedBy.length`) |
| `likedBy` | int[] | Array of userIDs who liked this post |
| `comments` | string[] | Array of comment strings (format: `"username: text"`) |

---

### `messages` Array

Each element is a direct message between two users.

```json
{
    "senderID":   2,
    "receiverID": 1,
    "text":       "Hey, want to connect?",
    "timestamp":  1748700120
}
```

Group messages use the same structure but `receiverID` is a negative group ID (e.g. `-1` for groupID 1) to distinguish them from direct messages.

---

### `groups` Array

Each element is a group chat.

```json
{
    "groupID":   1,
    "name":      "Study Group",
    "iconPath":  "",
    "creatorID": 1,
    "memberIDs": [1, 2, 3]
}
```

---

### `relations` Object

Stores the social graph as arrays of `[fromID, toID]` pairs.

```json
"relations": {
    "friends":   [[1, 2], [1, 3]],
    "followers":  [[2, 1], [3, 1]],
    "following":  [[1, 2], [1, 3]],
    "requests":   [[4, 1]]
}
```

| Key | Meaning |
|---|---|
| `friends` | Bidirectional — user A and user B are mutual friends |
| `followers` | userID B follows userID A |
| `following` | userID A follows userID B |
| `requests` | userID A has sent a pending request to userID B |

On load, these pairs are resolved to actual `User*` pointers and wired into each user's `friends[]`, `follower[]`, `following[]`, and `request[]` arrays.

---

### `banAppeals` Array

Submitted by banned users requesting re-activation.

```json
{
    "userID":  3,
    "userName": "BadActor",
    "message":  "I promise to follow the rules from now on."
}
```

---

### `userReports` Array

Submitted by users reporting others for misconduct.

```json
{
    "reportedUserID": 3,
    "reporterID":     1,
    "reason":         "Spam and harassment"
}
```

---

### Counters

| Field | Type | Description |
|---|---|---|
| `nextID` | int | Next available userID / postID (auto-incremented) |
| `nextGroupID` | int | Next available groupID |
| `userCount` | int | Total registered users |

---

## `notifications.txt` — Notification Records

Managed by `NotificationSystem::saveToFile()` / `loadFromFile()`. The format is internal to the notification system implementation. On first run, the file is created automatically.

The `notifications.txt` file that ships with the project contains just:
```
0
```
(zero notifications in the initial empty state)

---

## Load / Save Functions

| Function | Location | What It Does |
|---|---|---|
| `loadData()` | `backend/src/user.cpp` | Read `data.json` → populate all in-memory structures |
| `saveData()` | `backend/src/user.cpp` | Serialise all in-memory state → write `data.json` |
| `notifSystem.loadFromFile()` | `backend/src/notification.cpp` | Read `notifications.txt` |
| `notifSystem.saveToFile()` | `backend/src/notification.cpp` | Write `notifications.txt` |
| `ensureDefaultAdminAccount()` | `backend/src/user.cpp` | Recreate the default admin if not found in loaded data |

---

## Reset / Fresh Start

To reset all data and start fresh:

1. Delete `data.json`
2. Delete `notifications.txt`
3. Relaunch the application

The default admin account will be recreated automatically.
