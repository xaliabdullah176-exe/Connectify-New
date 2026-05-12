# OOP_CONCEPTS.md — Object-Oriented Programming in CONNECT

This document demonstrates every OOP principle applied in the project, with direct references to the actual source code.

---

## 1. Classes and Objects

The project defines **7 major classes** in `backend/include/user.h`:

- `Post` — represents a social media post
- `User` — base class for all user accounts
- `NormalUser` — inherits from `User` (role = "user")
- `Admin` — inherits from `User` (role = "admin")
- `Message` — a direct message between two users
- `MessageSystem` — manages all messages
- `Notification` — a single notification record
- `NotificationSystem` — manages all notifications
- `Group` — a group chat entity
- `GroupSystem` — manages all groups

Each class encapsulates its data and exposes a clean public interface.

---

## 2. Encapsulation

Data and behaviour are bundled together inside each class. Access is controlled through public methods.

### Example — `Post` class

```cpp
class Post {
public:
    int postID;
    string content;
    string imagePath;
    int likeCount;
    std::vector<int> likedBy;   // exception: uses vector for like tracking
    string comments[50];
    int commentCount;
    Post* next;
    time_t timestamp;

    // Behaviour bundled with data:
    bool toggleLike(int userID);   // toggles like and updates likeCount
    bool hasLiked(int userID);     // check without exposing the likedBy list logic
    void addComment(string c);     // bounds-checked comment append
    void display();                // self-displays to console
};
```

**Key encapsulation detail:** `toggleLike()` hides the entire like/unlike logic. The caller never manipulates `likedBy` directly — they just call `toggleLike(userID)` and get a bool back indicating the new state.

### Example — `NotificationSystem`

```cpp
class NotificationSystem {
    Notification** notifications;   // private — internal storage hidden
    int notifCount;
    void resize();                  // private — implementation detail

public:
    void addNotification(int targetID, string msg);
    void markAllSeen(int userID);
    int countUnseen(int userID) const;
    Notification* getAt(int i) const;
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);
};
```

The internal `notifications` array and `resize()` are private. External code can only use the clean public interface.

---

## 3. Inheritance

The project uses a two-level inheritance hierarchy for users.

```
User  (base class)
├── NormalUser
└── Admin
```

### Base class — `User`

```cpp
class User {
public:
    int userID;
    string userName;
    string password;
    string role;
    bool isBanned;
    // ... all common user data

    User** friends;
    int friendCount;
    Post** posts;
    int postCount;

    virtual ~User();              // virtual destructor — essential for inheritance

    void sendRequest(User* u);
    void follow(User* to);
    void acceptRequest(User* u);
    void rejectRequest(User* u);
    void createPost(Post* p);
    bool deletePost(int postID);
    void showPosts();
    void showNewsFeed();
};
```

### Derived class — `NormalUser`

```cpp
class NormalUser : public User {
public:
    NormalUser() { role = "user"; }

    NormalUser(int id, string username, string pass, string em = "") {
        userID    = id;
        userName  = username;
        password  = pass;
        role      = "user";    // automatically set — caller doesn't need to know
        email     = em;
    }
};
```

### Derived class — `Admin`

```cpp
class Admin : public User {
public:
    Admin() { role = "admin"; }

    Admin(int id, string username, string pass, string em = "") {
        userID   = id;
        userName = username;
        password = pass;
        role     = "admin";   // automatically set
        email    = em;
    }
};
```

**What inheritance gives us here:**
- All social features (friends, posts, follow) are defined once in `User` and reused by both roles.
- `NormalUser` and `Admin` differ only in their `role` field — they are stored together in the same `User**` array and distinguished at runtime by checking `users[i]->role`.

---

## 4. Polymorphism

### Runtime polymorphism via virtual destructor

The `User` base class declares:

```cpp
virtual ~User() {
    delete[] friends;
    delete[] request;
    delete[] follower;
    delete[] following;
    for (int i = 0; i < postCount; i++)
        delete posts[i];
    delete[] posts;
}
```

This means when you `delete` a `User*` that actually points to a `NormalUser` or `Admin`, the correct destructor chain is called — preventing memory leaks when working with the polymorphic `User**` array.

### Runtime role dispatch

Throughout the codebase, polymorphism is used to branch behaviour based on role:

```cpp
// In backend/main.cpp — after login:
if (users[loggedInIndex]->role == "admin")
    adminDashboard();
else
    userDashboard();

// In frontend/mainwindow:
if (users[m_loggedInIndex]->role == "admin")
    showAdminDashboard();
else
    showNewsFeed();
```

The same `User*` pointer is used — its behaviour differs based on the actual object it points to.

### Function overloading (compile-time polymorphism)

The `Post` class provides two constructors:

```cpp
// Default constructor
Post() {
    postID = 0; content = ""; likeCount = 0; /* ... */
}

// Parameterised constructor
Post(int id, string c, string imgPath = "") {
    postID = id; content = c; imagePath = imgPath; /* ... */
}
```

The `User` class provides overloaded constructors as well as a copy constructor and `operator=`.

---

## 5. Abstraction

Abstraction means exposing only what is necessary and hiding implementation details.

### Example — `MessageSystem::sendMessage()`

```cpp
bool MessageSystem::sendMessage(User* from, User* to, string text);
```

The caller just provides sender, receiver, and text. The internal logic of:
- checking if the receiver exists
- resizing the internal message array
- creating a `Message` object on the heap
- storing it in the array

...is all hidden inside `sendMessage()`. The caller sees a simple boolean result.

### Example — `User::resize()` / `User::resizePosts()`

```cpp
void resize(User**& u, int count);       // grows a User** array by one slot
void resizePosts(Post**& p, int count);  // grows a Post** array by one slot
```

All the raw memory reallocation logic is encapsulated in these utility methods. Every place in the code that needs to grow an array just calls `resize()` rather than duplicating `new`/`delete[]` logic.

---

## 6. Constructor Types

| Constructor Type | Class | Example |
|---|---|---|
| Default constructor | `Post`, `User`, `Message`, `Notification`, `Group` | `Post()` — initialises all fields to defaults |
| Parameterised constructor | `Post`, `NormalUser`, `Admin`, `Message` | `Post(int id, string c, string imgPath)` |
| Copy constructor | `User` | Deep copies all pointer arrays |
| Destructor | `User`, `MessageSystem`, `NotificationSystem`, `GroupSystem` | Properly frees all heap-allocated members |
| Virtual destructor | `User` | Ensures correct cleanup through base class pointer |

### Copy Constructor (deep copy example)

```cpp
User(const User& other) {
    userID       = other.userID;
    userName     = other.userName;
    // ... copy all primitive fields ...
    friendCount  = other.friendCount;

    // DEEP COPY — allocate new arrays and copy pointer values
    friends = new User*[friendCount + 1];
    for (int i = 0; i < friendCount; i++)
        friends[i] = other.friends[i];

    posts = new Post*[postCount + 1];
    for (int i = 0; i < postCount; i++)
        posts[i] = other.posts[i];
}
```

### Copy Assignment Operator

```cpp
User& operator=(const User& other) {
    if (this != &other) {           // self-assignment guard
        delete[] friends;           // free old memory first
        delete[] posts;
        // ... copy all fields ...
        friends = new User*[friendCount + 1];
        // ... deep copy arrays ...
    }
    return *this;
}
```

---

## 7. Operator Overloading

The `User` class overloads `operator=` to support safe copy assignment with proper memory cleanup and deep copy of all dynamic arrays.

---

## 8. Access Specifiers

| Class | Private | Public |
|---|---|---|
| `NotificationSystem` | `notifications`, `notifCount`, `resize()` | All user-facing methods |
| `User` | (all public for performance) | All fields and methods |
| `Post` | (all public for performance) | All fields and methods |
| `Group` | (all public) | `groupID`, `name`, `memberIDs`, etc. |

The project follows a pragmatic approach: fields that are only ever modified through class methods are kept public for simplicity in this academic context, while classes that manage shared state (`NotificationSystem`) enforce proper access control.

---

## 9. Static / Global Scope

The project uses `extern` declarations to share global state across compilation units:

```cpp
// Declared in user.h — shared across all .cpp files:
extern User** users;
extern int userCount;
extern NotificationSystem notifSystem;
```

This is a deliberate architectural choice: the backend global state is accessible to both the console `main.cpp` and the Qt `mainwindow` without duplication.

---

## Summary Table

| OOP Concept | Where Applied | How |
|---|---|---|
| Classes | Throughout | `Post`, `User`, `NormalUser`, `Admin`, `Message`, `MessageSystem`, `Notification`, `NotificationSystem`, `Group`, `GroupSystem` |
| Encapsulation | `NotificationSystem`, `MessageSystem`, `Post` | Private data + public interface |
| Inheritance | `NormalUser : User`, `Admin : User` | Role-based specialisation |
| Polymorphism | `User*` array, login dispatch | Virtual destructor + runtime role check |
| Abstraction | `sendMessage()`, `resize()`, `toggleLike()` | Complex logic hidden behind simple calls |
| Default constructor | All classes | Zero-initialise fields safely |
| Parameterised constructor | `Post`, `NormalUser`, `Admin`, `Message` | Convenient object creation |
| Copy constructor | `User` | Deep copy of pointer arrays |
| Destructor | `User`, `MessageSystem`, `NotificationSystem` | Proper heap cleanup |
| Virtual destructor | `User` | Safe delete through base pointer |
| Operator overloading | `User::operator=` | Safe copy assignment |
