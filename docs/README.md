# CONNECT — Social Media Platform
### Evaluation Guide

> **Project by:** Quantum Logics (PVT) Limited  
> **License:** MIT (2026)  
> **Build System:** CMake + Qt6 + MinGW / MSVC  
> **Language:** C++17

---

## What is CONNECT?

**CONNECT** (also called *Connectify* / *ConnectifyUI*) is a fully functional desktop social media platform built in C++ with a Qt6 GUI. It was engineered from scratch as an academic project that demonstrates real-world application of Object-Oriented Programming principles, custom data structures, and a layered software architecture — all without using STL containers.

The application ships with both a console-mode backend and a polished graphical frontend, proving the backend logic is completely independent of the UI layer.

---

## Table of Contents — Evaluation Guide

| # | Document | What It Covers |
|---|---|---|
| 1 | [README.md](README.md) | Project overview (this file) |
| 2 | [ARCHITECTURE.md](ARCHITECTURE.md) | Three-layer architecture, class diagram, data flow |
| 3 | [OOP_CONCEPTS.md](OOP_CONCEPTS.md) | All OOP principles demonstrated with code examples |
| 4 | [DATA_STRUCTURES.md](DATA_STRUCTURES.md) | Every custom data structure explained |
| 5 | [FEATURES.md](FEATURES.md) | Full feature list — user & admin capabilities |
| 6 | [BUILD_AND_RUN.md](BUILD_AND_RUN.md) | Step-by-step setup and build instructions |
| 7 | [DATA_FORMAT.md](DATA_FORMAT.md) | Persistence layer — how data is saved and loaded |
| 8 | [CLASS_REFERENCE.md](CLASS_REFERENCE.md) | Every class and its public API |

---

## Quick Project Stats

| Metric | Value |
|---|---|
| Total source files | 48 |
| Language | C++17 |
| GUI Framework | Qt 6.5+ |
| Build system | CMake 3.23+ with Ninja |
| Data persistence | JSON (`data.json`) + text (`notifications.txt`) |
| Custom data structures | 9 distinct hand-written structures |
| OOP principles applied | Inheritance, Encapsulation, Polymorphism, Abstraction |
| UI pages | 8 (Login, Signup, NewsFeed, Profile, Messages, Search, Notifications, Admin) |
| Backend operations | 18 user actions + 5 admin actions |

---

## Team & Repository

- **Organization:** Quantum Logics (PVT) Limited
- **Root directory:** `C:\CONNECT`
- **Main executable:** `ConnectifyUI.exe`
- **Quick launch:** `.\run.ps1` (PowerShell script — builds and launches in one step)

---

## At a Glance — What Makes This Project Stand Out

1. **Zero STL containers** — every list, array, and collection is hand-written using raw pointers and manual memory management, proving mastery of low-level C++.

2. **Clean three-layer architecture** — Model → Manager → UI, with a strict no-upward-dependency rule enforced across all files.

3. **Dual interface** — the same backend logic powers both a console menu system (`backend/main.cpp`) and a full Qt6 graphical interface (`frontend/`), demonstrating true separation of concerns.

4. **Complete social graph** — friends, followers, following, friend requests, and notifications are all managed in memory with custom pointer-based graphs.

5. **Admin system** — a separate admin role with its own dashboard for user management, ban/unban, post moderation, report review, and appeal handling.
