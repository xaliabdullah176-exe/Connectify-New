# FEATURES.md — Complete Feature List

CONNECT is a fully featured social media desktop application. This document lists every capability available to users and administrators.

---

## User Features (18 Actions)

### Authentication

| Feature | Description |
|---|---|
| Sign Up | Register a new account with username, password, and optional email. Role is set to `"user"` automatically. |
| Log In | Authenticate with username and password. Incorrect credentials return an error; banned accounts are blocked. |
| Delete Account | Permanently removes the user's account, all their posts, and all social relationships (friends, followers, following). |
| Log Out | Ends the session and returns to the main menu / login screen. |

### Social Graph

| Feature | Description |
|---|---|
| Send Friend Request | Send a request to another user by username. Duplicate requests are rejected. A notification is sent to the recipient. |
| Accept Friend Request | Accept a pending request — both users become mutual friends. A notification is sent to the requester. |
| Reject Friend Request | Decline a pending friend request. The request is removed and the requester is notified. |
| Follow a User | One-directional follow (like Twitter). The followed user appears in your news feed. |
| View Friend List | See all accepted mutual friends. |
| View Following List | See all users you are currently following. |
| View Pending Requests | See all incoming friend requests awaiting your decision. |

### Posts & Feed

| Feature | Description |
|---|---|
| Create Post | Write and publish a text post (with optional image path). The post is timestamped and added to your post list. |
| Show My Posts | View all posts you have created, including like counts and comments. |
| Show News Feed | View posts from all users you follow, displayed in chronological order. |
| Like / Unlike a Post | Toggle a like on any post belonging to a friend or yourself. The post owner receives a notification. |
| Comment on a Post | Add a text comment to any post belonging to a friend or yourself. The post owner receives a notification. |

### Messaging

| Feature | Description |
|---|---|
| Send Message | Send a direct message to any user by username. |
| View Inbox | See all direct messages received, grouped by conversation partner. |

### Notifications

| Feature | Description |
|---|---|
| View Notifications | See all notifications (friend requests, likes, comments). Notifications are marked as seen on view. |

### Search

| Feature | Description |
|---|---|
| Search Users | Find users by keyword (searches usernames). |
| Search Posts | Find posts by keyword (searches post content). |

---

## Admin Features (5 Actions + Dashboard)

Admin accounts have a separate dashboard and elevated privileges.

| Feature | Description |
|---|---|
| Display All Users | View a complete list of all registered users with their IDs, usernames, emails, roles, and ban status. |
| Delete a User | Permanently remove any user account by username. All their posts and social relationships are cleaned up. |
| Search Users | Same keyword search as regular users but available from the admin dashboard. |
| Search Posts | Same keyword search as regular users but available from the admin dashboard. |
| View All Posts | See every post from every user on the platform, with post IDs, content, like counts, and comments. |

### Admin Dashboard UI (Qt Frontend)

The graphical admin dashboard includes additional features beyond the console:

| Feature | Description |
|---|---|
| Stats Panel | Shows total users, total posts, banned users count, and posts pending review — updated live. |
| User Table | A scrollable, searchable table of all users with avatar initials, email, role badge, post count, and ban status. |
| User Search | Real-time filter on the user table — typing in the search box instantly shows/hides matching rows. |
| Ban / Unban User | Toggle a user's `isBanned` flag directly from the dashboard. |
| Post Moderation Grid | A two-column grid of post cards with owner name, content, image, like count, and comment count. |
| User Reports | Review reports submitted against users, with a summary and the option to take action. |
| Ban Appeals | Review ban appeal messages submitted by banned users and decide to unban or keep the ban. |
| Sidebar Navigation | Persistent sidebar with Dashboard, Users, Posts, Moderation, and News sections. |

---

## Group Chat Features

| Feature | Description |
|---|---|
| Create Group | Any user can create a group chat, set a name and icon, and invite friends by user ID. |
| Send Group Message | Send a message visible to all group members. |
| Rename Group | The group creator can change the group name. |
| Add Members | The group creator can add new friends to the group. |
| Remove Member | The group creator can remove a member from the group. |

---

## Notification Events

Notifications are automatically sent in the following situations:

| Event | Recipient |
|---|---|
| Friend request sent | The user who received the request |
| Friend request accepted | The user who sent the original request |
| Friend request rejected | The user who sent the original request |
| Post liked | The post owner |
| Comment added to post | The post owner |

---

## Data Persistence

All data survives application restarts:

| Data | Persisted In |
|---|---|
| User accounts | `data.json` → `users` array |
| Posts | `data.json` → `posts` array |
| Direct messages | `data.json` → `messages` array |
| Groups | `data.json` → `groups` array |
| Social relations (friends, followers) | `data.json` → `relations` object |
| User reports | `data.json` → `userReports` array |
| Ban appeals | `data.json` → `banAppeals` array |
| Notifications | `notifications.txt` |

---

## Default Admin Account

The application ships with a default admin account seeded in `data.json`:

| Field | Value |
|---|---|
| Username | `Jeffrey_epstien` |
| Password | `Mustafa_tech123` |
| Role | `admin` |
| userID | `1` |

This account is protected — it is recreated on startup if removed (`ensureDefaultAdminAccount()` in `user.cpp`).
