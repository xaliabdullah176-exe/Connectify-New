# QT_SIGNALS_AND_SLOTS.md — Complete Signal/Slot Wiring Map

This document lists every signal and slot connection in the CONNECT application, showing exactly how pages communicate with `MainWindow` and with each other.

All connections are established in `MainWindow::connectSignals()`, which is called once from the `MainWindow` constructor after all pages are instantiated.

---

## Connection Syntax Reference

```cpp
// Named slot:
connect(sender, &SenderClass::signal, receiver, &ReceiverClass::slot);

// Lambda slot:
connect(sender, &SenderClass::signal, receiver, [this, capture]() {
    // inline logic
});
```

---

## LoginPage Signals

**File:** `frontend/pages/LoginPage.h/.cpp`

| Signal                                             | Emitted When                | Connected To                   |
| -------------------------------------------------- | --------------------------- | ------------------------------ |
| `loginClicked(QString username, QString password)` | User clicks "Log In" button | `MainWindow::onLoginClicked()` |
| `goToSignup()`                                     | User clicks "Sign Up" link  | `MainWindow::showSignup()`     |

### `MainWindow::onLoginClicked()` — what it does

```
1. Calls backend: login(username, password) → returns index or -1
2. If -1: loginPage->showError("Invalid username or password.")
3. If user is banned: shows ban dialog with appeal form
4. If valid:
   - m_loggedInIndex = index
   - loginPage->clearFields()
   - if role == "admin": showAdminDashboard()
   - else:               showNewsFeed()
```

---

## SignupPage Signals

**File:** `frontend/pages/signuppage.h/.cpp`

| Signal                                                                               | Emitted When                                                      | Connected To                    |
| ------------------------------------------------------------------------------------ | ----------------------------------------------------------------- | ------------------------------- |
| `signupClicked(QString fullName, QString username, QString email, QString password)` | User clicks "Create Account" after passing client-side validation | `MainWindow::onSignupClicked()` |
| `loginLinkClicked()`                                                                 | User clicks "Already have an account?"                            | `MainWindow::showLogin()`       |

### Client-side validation (inside `SignupPage` before emitting)

- All fields non-empty
- Username has no spaces
- Password ≥ 6 characters
- Password matches confirm password

### `MainWindow::onSignupClicked()` — what it does

```
1. Checks userNameExist() → showError if taken
2. Checks emailTaken()   → showError if taken
3. Calls signup(nextID++, username, password, "user", email)
4. Calls login() immediately → auto-login
5. m_loggedInIndex = index
6. signupPage->clearFields()
7. showNewsFeed()
```

---

## NewsFeedPage Signals

**File:** `frontend/pages/newsfeedpage.h/.cpp`

| Signal                                                  | Emitted When                     | Connected To                                                 |
| ------------------------------------------------------- | -------------------------------- | ------------------------------------------------------------ |
| `createPostClicked(QString content, QString imagePath)` | User submits a new post          | `MainWindow::onCreatePost()`                                 |
| `likeClicked(int postID)`                               | User clicks the like button      | `MainWindow::onLikePost()`                                   |
| `commentClicked(int postID)`                            | User clicks the comment button   | `MainWindow::onCommentPost()`                                |
| `deletePostClicked(int postID)`                         | User clicks delete on their post | `MainWindow::onDeletePost()`                                 |
| `logoutClicked()`                                       | User clicks logout               | `MainWindow::onLogout()`                                     |
| `goToProfile()`                                         | User clicks their avatar/name    | Lambda → sets `m_viewedUserID = -1`, calls `showProfile()`   |
| `goToMessages()`                                        | User clicks Messages nav button  | `MainWindow::showMessages()`                                 |
| `goToSearch()`                                          | User clicks Search nav button    | `MainWindow::showSearch()`                                   |
| `goToNotifications()`                                   | User clicks Notifications bell   | `MainWindow::showNotifications()`                            |
| `showOwnPostsClicked()`                                 | User selects "My Posts" feed tab | Lambda → sets `m_feedViewMode = Home`, calls `loadFeed()`    |
| `showNetworkPostsClicked()`                             | User selects "Network" feed tab  | Lambda → sets `m_feedViewMode = Network`, calls `loadFeed()` |

### `MainWindow::onCreatePost()` — what it does

```
1. Creates Post* p = new Post(nextID++, content, imagePath)
2. users[m_loggedInIndex]->createPost(p)
3. For each friend: notifSystem.addNotification(friendID, "shared a new post!")
4. saveData()
5. loadFeed() — refreshes the feed
```

### `MainWindow::onLikePost()` — what it does

```
1. Find the post across all users by postID
2. Check friendship (can only like friends' posts)
3. post->toggleLike(myUserID)
4. If liked: notifSystem.addNotification(owner, "liked your post")
5. saveData()
6. Refresh current page (feed or profile)
```

### `MainWindow::onCommentPost()` — what it does

```
1. Show QInputDialog to get comment text
2. Find post by postID
3. post->addComment("username: text")
4. notifSystem.addNotification(owner, "commented on your post")
5. saveData()
6. Refresh current page
```

---

## ProfilePage Signals

**File:** `frontend/pages/profilepage.h/.cpp`

| Signal                                  | Emitted When                                 | Connected To                                                  |
| --------------------------------------- | -------------------------------------------- | ------------------------------------------------------------- |
| `likeClicked(int postID)`               | Like button on a profile post                | `MainWindow::onLikePost()`                                    |
| `commentClicked(int postID)`            | Comment button on a profile post             | `MainWindow::onCommentPost()`                                 |
| `deletePostClicked(int postID)`         | Delete button on a post                      | `MainWindow::onDeletePost()`                                  |
| `deleteAccountClicked()`                | User clicks "Delete My Account"              | `MainWindow::onDeleteMyAccount()`                             |
| `editProfileClicked()`                  | User clicks "Edit Profile"                   | Lambda — shows `QDialog` for editing                          |
| `reportUserClicked(int reportedUserID)` | User clicks "Report" on another profile      | Lambda — calls `modSystem.addReport()`                        |
| `createPostClicked()`                   | User clicks "Create Post" from their profile | Lambda → `showNewsFeed()`                                     |
| `backClicked()`                         | User clicks "← Back"                         | Lambda — context-sensitive: returns to feed, search, or admin |
| `viewAllFriendsClicked()`               | User clicks "View All" in friends sidebar    | `MainWindow::showSearch()`                                    |

### Back navigation logic

```
backClicked lambda:
    if (logged in as admin AND viewing another user's profile):
        m_viewedUserID = -1
        showAdminDashboard()
    else if (viewing another user's profile from search):
        m_viewedUserID = -1
        showSearch()
    else:
        showNewsFeed()
```

### Edit Profile dialog — fields

| Field           | Widget                            | Validation                                   |
| --------------- | --------------------------------- | -------------------------------------------- |
| Username        | `QLineEdit`                       | Non-empty, case-insensitive uniqueness check |
| Email           | `QLineEdit`                       | Must contain `@`, must not be taken          |
| Birthday        | `QDateEdit` (with calendar popup) | Any valid date                               |
| GitHub username | `QLineEdit`                       | No validation                                |
| Profile photo   | `QFileDialog` (image picker)      | File must exist                              |

---

## SearchPage Signals

**File:** `frontend/pages/searchpage.h/.cpp`

| Signal                            | Emitted When                  | Connected To                                                       |
| --------------------------------- | ----------------------------- | ------------------------------------------------------------------ |
| `backClicked()`                   | Back button clicked           | `MainWindow::showNewsFeed()`                                       |
| `logoutClicked()`                 | Logout button                 | `MainWindow::onLogout()`                                           |
| `sendFriendRequest(int targetID)` | "Add Friend" on a user card   | Lambda — calls `me->sendRequest(target)` + notification            |
| `followUser(int targetID)`        | "Follow" on a user card       | Lambda — calls `me->follow(target)`                                |
| `viewProfile(int targetID)`       | "View Profile" on a user card | Lambda — sets `m_viewedUserID`, calls `showProfile()`              |
| `sendMessageTo(int targetID)`     | "Message" on a user card      | Lambda — calls `messagePage->loadConversation()`, `showMessages()` |

---

## MessagePage Signals

**File:** `frontend/pages/messagepage.h/.cpp`

| Signal                                                 | Emitted When                           | Connected To                                                             |
| ------------------------------------------------------ | -------------------------------------- | ------------------------------------------------------------------------ |
| `backClicked()`                                        | Back button                            | `MainWindow::showNewsFeed()`                                             |
| `logoutClicked()`                                      | Logout button                          | `MainWindow::onLogout()`                                                 |
| `contactSelected(int userID)`                          | User clicks a conversation in the list | Lambda — loads conversation history, calls `messagePage->loadMessages()` |
| `dmMessageSendRequested(int receiverID, QString text)` | Send button in DM chat                 | Lambda — `msgSystem.sendMessage()`, `saveData()`                         |
| `groupMessageSendRequested(int groupID, QString text)` | Send button in group chat              | Lambda — `msgSystem.sendGroupMessage()`, `saveData()`                    |
| `createGroupClicked()`                                 | "+ Create group" button                | Lambda — shows `QDialog` with friend checkboxes                          |
| `renameGroupClicked(int groupID)`                      | "Edit name" button in group chat       | Lambda — `QInputDialog::getText()`, `groupSystem.setGroupName()`         |
| `addMembersToGroupClicked(int groupID)`                | "Add members" button                   | Lambda — shows `QDialog` with friend checkboxes                          |
| `manageGroupMembersClicked(int groupID)`               | "Manage" button (creator only)         | Lambda — shows `QDialog` with remove buttons                             |

### DM message send flow

```
MessagePage::onSendClicked()
    → text = messageInput->text()
    → addMessage(text, time, isMine=true)    // optimistic UI update
    → messageInput->clear()
    → emit dmMessageSendRequested(receiverID, text)

MainWindow lambda:
    → msgSystem.sendMessage(me, target, text.toStdString())
    → saveData()
```

### Group creation dialog

```
1. Show QDialog with QScrollArea of QCheckBox items (one per friend)
2. User selects friends and enters a group name
3. On accept: groupSystem.createGroup(me, name, iconPath, selectedIDs)
4. For each member: notifSystem.addNotification(memberID, "added to group")
5. saveData()
6. messagePage->openGroupConversation(...)
7. showMessages()
```

---

## NotificationPage Signals

**File:** `frontend/pages/notificationpage.h/.cpp`

| Signal                                  | Emitted When                                           | Connected To                                                                            |
| --------------------------------------- | ------------------------------------------------------ | --------------------------------------------------------------------------------------- |
| `backClicked()`                         | Back button                                            | `MainWindow::showNewsFeed()`                                                            |
| `friendRequestAccepted(int fromUserID)` | User clicks "Accept" on a friend request notification  | Lambda — calls `me->acceptRequest(from)`, sends notification to requester, `saveData()` |
| `friendRequestDeclined(int fromUserID)` | User clicks "Decline" on a friend request notification | Lambda — calls `me->rejectRequest(from)`, removes request notifications, `saveData()`   |

### After accept/decline

```
1. notifSystem.removeFriendRequestNotifications(me->userID, fromUserID)
2. me->acceptRequest(from)  OR  me->rejectRequest(from)
3. If accepted: notifSystem.addNotification(fromUserID, "accepted your request")
4. saveData()
5. notifPage->loadNotifications(me->userID)    // refresh notification list
6. feedPage->setNotifBadge(notifSystem.countUnseen(me->userID))  // update badge
```

---

## AdminDashboard Signals

**File:** `frontend/pages/admindashboard.h/.cpp`

| Signal                                        | Parameters        | Connected To                                                                       |
| --------------------------------------------- | ----------------- | ---------------------------------------------------------------------------------- |
| `logoutClicked()`                             | —                 | `MainWindow::onLogout()`                                                           |
| `deleteUserClicked(int userID)`               | target user ID    | Lambda — `QMessageBox::question()` confirm, `adminDelete()`, `saveData()`, refresh |
| `banUserClicked(int userID)`                  | target user ID    | Lambda — confirm, set `isBanned=true`, notify, `saveData()`, refresh               |
| `unbanUserClicked(int userID)`                | target user ID    | Lambda — confirm, set `isBanned=false`, notify, `saveData()`, refresh              |
| `viewUserClicked(int userID)`                 | target user ID    | Lambda — `m_viewedUserID = userID`, `showProfile()`                                |
| `userGroupsClicked(int userID)`               | target user ID    | Lambda — collect group memberships, show `QMessageBox::information()`              |
| `deletePostClicked(int postID)`               | post ID           | Lambda — confirm, `owner->deletePost(postID)`, notify, `saveData()`, refresh       |
| `moderationBanFromReport(int reportedUserID)` | reported user ID  | Lambda — confirm, ban user, clear reports, notify, `saveData()`, refresh           |
| `moderationIgnoreReports(int reportedUserID)` | reported user ID  | Lambda — `modSystem.clearReportsAgainst()`, `saveData()`, refresh                  |
| `moderationUnbanFromAppeal(int userID)`       | appealing user ID | Lambda — unban, remove appeal, notify, `saveData()`, refresh                       |
| `moderationKeepBannedFromAppeal(int userID)`  | appealing user ID | Lambda — remove appeal, notify user (ban remains), `saveData()`, refresh           |
| `createNewsClicked()`                         | —                 | `MainWindow::onAdminCreateNews()`                                                  |
| `navDashboard()`                              | —                 | `MainWindow::showAdminDashboard()`                                                 |
| `navUsers()`                                  | —                 | `MainWindow::showAdminDashboard()`                                                 |
| `navPosts()`                                  | —                 | `MainWindow::showAdminDashboard()`                                                 |

---

## Signal Flow Summary Diagram

```
User Action                    Page Signal              MainWindow Slot         Backend Call
───────────────────────────────────────────────────────────────────────────────────────────
Type credentials + click Login → loginClicked()      → onLoginClicked()    → login()
Fill form + click Register     → signupClicked()     → onSignupClicked()   → signup() + login()
Type post + click Post         → createPostClicked() → onCreatePost()      → createPost()
Click ♥ on a post              → likeClicked(id)     → onLikePost()        → toggleLike()
Click 💬 on a post             → commentClicked(id)  → onCommentPost()     → addComment()
Click Accept on friend request → friendRequestAccepted(fromID) → lambda   → acceptRequest()
Click Ban in admin panel       → banUserClicked(id)  → lambda              → isBanned = true
Click Logout                   → logoutClicked()     → onLogout()          → saveData()
```
