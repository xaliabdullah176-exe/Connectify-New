[README.md](https://github.com/user-attachments/files/27322582/README.md)
<div align="center">

# 🔗 Connectify
### A C++ Social Media Management System

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus)
![Qt](https://img.shields.io/badge/Qt-6.11-green?style=for-the-badge&logo=qt)
![CMake](https://img.shields.io/badge/CMake-3.16+-red?style=for-the-badge&logo=cmake)
![License](https://img.shields.io/badge/License-Academic-purple?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)

> *"Connect with the world — built from scratch with pure C++ and OOP principles."*

**BS Software Engineering — 2nd Semester | FAST NUCES Lahore**
**Course: Object Oriented Programming | Section: BSE-2B**

</div>

---

## 📌 Table of Contents

- [About The Project](#-about-the-project)
- [Features](#-features)
- [OOP Architecture](#-oop-architecture)
- [Tech Stack](#-tech-stack)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Screenshots](#-screenshots)
- [Team](#-team)

---

## 🚀 About The Project

**Connectify** is a fully functional, GUI-based social networking desktop application built entirely in **C++ with Qt Widgets**. Developed as a semester project for OOP at FAST NUCES Lahore, it simulates core features of a modern social media platform — from user authentication and news feeds to private messaging and admin moderation.

The project demonstrates a professional-grade command of **Object-Oriented Programming** principles through a clean, modular architecture — with a dark purple-themed UI that rivals real-world applications.

---

## ✨ Features

### 👤 User Management
- Secure **Signup & Login** with credential validation
- Role-Based Access Control — `User` and `Admin` perspectives
- Account deletion with full data cleanup

### 📰 News Feed
- Dynamic feed showing posts from followed users
- Sorted by timestamp — latest first
- Create, like, and comment on posts

### 📝 Post System
- **Text Posts** and **Image Posts** (Polymorphism in action)
- Like counter with optimistic UI updates
- Comment system with notification triggers

### 👥 Social Graph
- Send, accept, and reject friend requests
- Follow / Unfollow system
- Friends-only messaging restriction

### 💬 Private Messaging
- One-to-one messaging between connected users
- Inbox view per user
- Friends-only access control

### 🔍 Search & Discovery
- Search users by keyword/username
- View user profiles
- Send friend requests directly from search

### 🔔 Notification Engine
- Real-time alerts for likes, comments, and friend requests
- Per-user notification history

### 🛡️ Admin Dashboard
- View all registered users and their stats
- Delete inappropriate posts or ban users
- Platform-wide statistics overview

### 💾 Data Persistence
- File-based storage using `fstream`
- All data saved on exit, loaded on startup
- Separate files: `users.txt`, `posts.txt`, `messages.txt`, `relations.txt`

---

## 🏗️ OOP Architecture

Connectify is built on four pillars of Object-Oriented Programming:

### Encapsulation
```cpp
class User {
private:
    string password;     // Never exposed publicly
    string email;
public:
    string getPassword() { return password; }  // Controlled access
};
```

### Inheritance
```
Person (Abstract Base)
├── User       → Regular platform user
│   └── NormalUser
└── Admin      → Elevated permissions

Post (Abstract Base)
├── TextPost   → Text-only content
└── ImagePost  → Media content
```

### Abstraction
```cpp
class Post {
public:
    virtual void display() = 0;  // Pure virtual — forces implementation
};
```

### Polymorphism
```cpp
// Same function call — different behavior
TextPost*  tp = new TextPost(id, content);
ImagePost* ip = new ImagePost(id, content, imagePath);

tp->display();  // Shows text
ip->display();  // Shows image + caption
```

---

## 🛠️ Tech Stack

| Technology | Purpose |
|---|---|
| **C++ 17** | Core application logic |
| **Qt 6.11 Widgets** | GUI framework — pure code, no QML |
| **CMake 3.16+** | Build system |
| **fstream** | File-based data persistence |
| **Git + GitHub** | Version control & collaboration |
| **Qt Creator** | IDE |

---

## 📁 Project Structure

```
ConnectifyUI/
│
├── backend/                    ← Pure C++ business logic
│   ├── user.h                  ← Core: User, Post, Admin, NormalUser classes
│   ├── user.cpp                ← User methods, signup, login, global functions
│   ├── post.cpp                ← Post creation, news feed logic
│   ├── message.cpp             ← MessageSystem implementation
│   ├── notification.cpp        ← NotificationSystem implementation
│   └── search.cpp              ← Search users & posts
│
├── LoginPage.h / .cpp          ← Login screen
├── signuppage.h / .cpp         ← Registration screen
├── newsfeedpage.h / .cpp       ← Main feed with create post
├── profilepage.h / .cpp        ← User profile view
├── searchpage.h / .cpp         ← User & post search
├── messagepage.h / .cpp        ← Private messaging
├── admindashboard.h / .cpp     ← Admin control panel
│
├── mainwindow.h / .cpp         ← QStackedWidget — navigation hub
├── style.qss                   ← Global dark purple theme
├── CMakeLists.txt              ← Build configuration
└── main.cpp                    ← App entry point
```

---

## 🚀 Getting Started

### Prerequisites

- Qt 6.11+ installed ([Download](https://www.qt.io/download-open-source))
- CMake 3.16+
- MinGW 13.1+ (included with Qt)
- Git

### Installation

```bash
# 1. Clone the repository
git clone https://github.com/xaliabdullah176-exe/Connectify-New.git
cd Connectify-New

# 2. Configure with CMake
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64"

# 3. Build
cmake --build build

# 4. Deploy Qt DLLs
"C:/Qt/6.11.0/mingw_64/bin/windeployqt.exe" ./build/ConnectifyUI.exe

# 5. Run
./build/ConnectifyUI.exe
```

### Default Admin Credentials
```
Username: admin
Password: admin123
```

---

## 🎨 UI Theme

Connectify uses a custom dark purple theme across all pages:

| Element | Color |
|---|---|
| Background | `#0d0d1a` |
| Card Background | `#12122a` |
| Primary Accent | `#7c3aed` |
| Light Accent | `#a78bfa` |
| Border | `#1e1e3a` |
| Text | `#ffffff` |

---

## 🧭 Navigation Flow

```
App Start → loadData()
    │
    ▼
LoginPage ←─────────────────────────────┐
    │ loginClicked()                     │
    ▼                                    │
[login() returns index]                  │
    │                                    │
    ├─ role == "admin" → AdminDashboard  │
    │                                    │
    └─ role == "user"  → NewsFeedPage    │
                             │           │
             ┌───────────────┼───────────┤
             ▼               ▼           │
        ProfilePage    MessagePage       │
             │               │           │
             └──→ SearchPage ─┘          │
                                         │
    SignupPage ───→ auto-login ──→ NewsFeedPage
```

---

## 👨‍💻 Team

**Project Team 05 — BSE-2B | FAST NUCES Lahore**

| Role | Name | Roll No | Contribution |
|---|---|---|---|
| **Team Lead** | Ali Abdullah | 25L-3022 | Frontend (Login, Signup, NewsFeed), Integration, GitHub |
| Member | Farzam Zeeshan | 25L-3049 | Backend Logic, Admin Features |
| Member | Mustafa Amir | 25L-3107 | News Feed Logic, Notifications |
| Member | M. Arslan | 25L-3080 | Frontend (Profile, Search, Messages, Admin Dashboard) |
| Member | Saad Amin | 25L-3045 | File Handling, Data Persistence |
| Member | Muhammad Khubaib | 25L-3026 | Social Graph, Friend System |

**Submitted To:** Mam Hina Iqbal & Syed Saad Ali

---

## 📚 Key Learning Outcomes

Through this project, our team gained hands-on experience with:

- Designing scalable **class hierarchies** with abstract base classes
- Implementing **dynamic memory management** with `new`/`delete`
- Building **event-driven GUIs** with Qt Signals & Slots
- Managing **team collaboration** with Git branching strategies
- Writing **modular, maintainable C++ code** in a real-world context

---

<div align="center">

**Built with 💜 by Team 05 — FAST NUCES Lahore**

*"From zero to social media platform — one commit at a time."*

</div>
