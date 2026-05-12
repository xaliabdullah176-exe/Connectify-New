# QT_FUNDAMENTALS.md — How Qt Works in CONNECT

This document explains the core Qt concepts used throughout the project: the object model, signals & slots, widget hierarchy, and the event system.

---

## 1. What Qt Adds to C++

Qt extends C++ with a **meta-object system** that enables:

- **Signals and slots** — a type-safe event/callback mechanism (no raw function pointers)
- **Object ownership** — parent widgets automatically delete their children
- **Dynamic properties** — runtime key-value data on any `QObject`
- **Qt Designer UI** — `.ui` files describe widget layout in XML

Every Qt class that uses signals or slots must include the `Q_OBJECT` macro in its class body. This triggers the **Meta-Object Compiler (MOC)** to generate extra C++ code for signal/slot wiring.

```cpp
class LoginPage : public QWidget {
    Q_OBJECT       // ← required for signals and slots
public:
    explicit LoginPage(QWidget* parent = nullptr);
signals:
    void loginClicked(const QString& username, const QString& password);
};
```

---

## 2. Signals and Slots — The Core Communication Pattern

In traditional C++, objects call each other's methods directly. Qt replaces this with **signals and slots**:

- A **signal** is emitted by a widget when something happens (button click, text change).
- A **slot** is a function that runs in response to a signal.
- They are connected with `QObject::connect()`.

```
[Widget A] --emits signal--> connect() --calls slot--> [Widget B / MainWindow]
```

### Why this matters for the project

The strict architecture rule — _UI cannot talk to backend directly_ — is enforced through signals and slots. A page widget never calls `login()` or `signup()` itself. It only emits a signal. The `MainWindow` receives the signal and calls the backend.

```cpp
// LoginPage emits — it doesn't know who handles it:
emit loginClicked(username, password);

// MainWindow connects — it bridges UI and backend:
connect(loginPage, &LoginPage::loginClicked,
        this,      &MainWindow::onLoginClicked);

// MainWindow slot — calls the backend:
void MainWindow::onLoginClicked(const QString &username, const QString &password) {
    int index = login(username.toStdString(), password.toStdString());
    // ...
}
```

### connect() syntax — the modern form

All connections in this project use the **pointer-to-member-function** form (Qt5+):

```cpp
connect(sender,  &SenderClass::signalName,
        receiver, &ReceiverClass::slotName);
```

This is checked at **compile time** — if signal and slot signatures don't match, the build fails. The old `SIGNAL()` / `SLOT()` string-based macros are never used here.

### Lambda slots

Many connections use C++ lambdas instead of named slots, especially for inline logic:

```cpp
connect(banBtn, &QPushButton::clicked, this, [this, userID]() {
    // capture 'this' and 'userID' from the enclosing scope
    users[i]->isBanned = true;
    saveData();
    showAdminDashboard();
});
```

**Lambda capture rules used in this project:**

- `[this]` — captures the `MainWindow` pointer so slots can call its methods
- `[this, userID]` — captures both `this` and a local variable `userID` by value
- `[&]` — used inside modal dialogs to capture everything by reference (safe because the lambda outlives the variables)

---

## 3. The Widget Hierarchy and Parent Ownership

Qt widgets form a **parent-child tree**. When a parent widget is destroyed, it automatically destroys all its children. This means:

- You almost never call `delete` on a widget explicitly.
- Passing a parent pointer to a widget's constructor registers it as a child.

```cpp
// LoginPage constructor:
LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {
    setupUI();
}

// Inside setupUI(), every widget gets 'this' as parent — or is added to a layout:
QLabel* title = new QLabel("Welcome back", this);    // parent = this
QPushButton* btn = new QPushButton("Login");          // no parent yet
layout->addWidget(btn);   // layout's parent widget takes ownership
```

### Layout ownership

When you add a widget to a layout, and that layout is set on a parent widget, the widget's parent becomes the layout's parent widget. This is how the entire tree is managed.

```cpp
QVBoxLayout* vl = new QVBoxLayout(this);    // 'this' owns the layout
vl->addWidget(titleLabel);                  // titleLabel's parent = 'this'
vl->addWidget(loginBtn);                    // loginBtn's parent = 'this'
```

---

## 4. QStackedWidget — The Navigation Engine

The entire navigation between pages is managed by a single `QStackedWidget` in `MainWindow`. This widget holds all pages stacked on top of each other and shows only one at a time.

```
QMainWindow
└── QStackedWidget (stack)
    ├── LoginPage       ← index 0
    ├── SignupPage      ← index 1
    ├── NewsFeedPage    ← index 2
    ├── ProfilePage     ← index 3
    ├── SearchPage      ← index 4
    ├── MessagePage     ← index 5
    ├── NotificationPage← index 6
    └── AdminDashboard  ← index 7
```

Navigation is done by switching the active widget (not the index number, to avoid fragility):

```cpp
void MainWindow::showLogin() {
    stack->setCurrentWidget(loginPage);
}

void MainWindow::showNewsFeed() {
    loadFeed();
    stack->setCurrentWidget(feedPage);
}

void MainWindow::showAdminDashboard() {
    // populate admin data first, then switch
    adminPage->clearUsers();
    for (int i = 0; i < userCount; i++)
        adminPage->addUserRow(...);
    stack->setCurrentWidget(adminPage);
}
```

No page is ever destroyed and recreated — they all live in the stack for the lifetime of the app. Before switching to a page, data is always refreshed (e.g. `loadFeed()`, `adminPage->clearUsers()` + re-populate).

---

## 5. The `setupUI()` Pattern

Every page follows the same construction pattern:

```cpp
ClassName::ClassName(QWidget* parent) : QWidget(parent) {
    setupUI();     // build the widget tree and layouts
    applyStyles(); // apply the QSS stylesheet
}
```

`setupUI()` is always a private method that:

1. Creates all sub-widgets
2. Arranges them in layouts
3. Connects internal signals (widget → widget within the same page)

`applyStyles()` calls `setStyleSheet()` with the QSS string.

External signals (page → MainWindow) are **never connected inside the page**. The page only emits them. MainWindow connects them in its `connectSignals()` method.

---

## 6. Layout System

Qt uses **layout managers** to position widgets. No pixel positions are used in this project — everything is relative.

### Layouts used in CONNECT

| Layout Class     | Usage                                                   |
| ---------------- | ------------------------------------------------------- |
| `QVBoxLayout`    | Stack widgets vertically (most pages)                   |
| `QHBoxLayout`    | Place widgets side by side (toolbars, rows, input bars) |
| `QGridLayout`    | Admin moderation post grid (2 columns)                  |
| `QFormLayout`    | Edit Profile dialog (label + input pairs)               |
| `QStackedWidget` | Page switcher in MainWindow                             |

### Layout nesting example — MessagePage input bar

```cpp
// Root layout: horizontal split (left panel | right panel)
QHBoxLayout* root = new QHBoxLayout(this);

// Left panel: vertical stack
QVBoxLayout* leftVl = new QVBoxLayout(leftPanel);
leftVl->addWidget(leftTop);      // top bar
leftVl->addWidget(listHeader);   // Messages title + search
leftVl->addWidget(listScroll);   // scrollable chat list

// Input bar (bottom of right panel): horizontal
QHBoxLayout* ibHl = new QHBoxLayout(inputBar);
ibHl->addWidget(messageInput);   // text field (stretches)
ibHl->addWidget(sendBtn);        // fixed-width button
```

### Stretch

`addStretch()` pushes widgets to one side of a layout:

```cpp
// Push everything to the left, nothing on the right:
layout->addWidget(logo);
layout->addStretch();   // fills the remaining space

// Center a widget:
outerLayout->addStretch();
outerLayout->addWidget(card);   // centered card
outerLayout->addStretch();
```

---

## 7. QScrollArea — Scrollable Content

Pages with variable-length content (feed, messages, admin user list) use `QScrollArea`:

```cpp
QScrollArea* listScroll = new QScrollArea();
listScroll->setWidgetResizable(true);           // inner widget resizes with scroll area
listScroll->setFrameShape(QFrame::NoFrame);     // no border
listScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

QWidget* listInner = new QWidget();
chatListLayout = new QVBoxLayout(listInner);
chatListLayout->addStretch();

listScroll->setWidget(listInner);   // set the scrollable content
```

Content is added dynamically by inserting widgets into the inner layout:

```cpp
// Insert before the stretch at the bottom:
chatListLayout->insertWidget(chatListLayout->count() - 1, newChatItem);
```

---

## 8. QPainter — Custom Drawing (Circle Avatars)

Profile images and avatars are cropped into circles using `QPainter` and `QPainterPath`. This is done in a helper function used across multiple pages:

```cpp
static QPixmap makeCirclePixmap(const QString& path, int size) {
    QPixmap px(path);
    if (px.isNull() || size <= 0) return QPixmap();

    // 1. Scale to fill the square
    QPixmap scaled = px.scaled(size, size,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation);

    // 2. Create a transparent target pixmap
    QPixmap circle(size, size);
    circle.fill(Qt::transparent);

    // 3. Paint with a circular clip path
    QPainter painter(&circle);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath clip;
    clip.addEllipse(0, 0, size, size);
    painter.setClipPath(clip);           // clip to circle
    painter.drawPixmap(0, 0, scaled);    // draw inside the clip

    return circle;
}
```

This function is used wherever an avatar is displayed:

- `AdminDashboard::makeUserRow()` — user table avatars (30px)
- `ProfilePage::loadProfile()` — main avatar (88px)
- `MessagePage::openConversation()` — chat header avatar (38px)

---

## 9. QDialog — Modal Pop-ups

Several interactions use modal dialogs rather than page switches:

### Edit Profile (`QDialog` + `QFormLayout`)

```cpp
QDialog dlg(this);
dlg.setWindowTitle("Edit About");
QFormLayout* form = new QFormLayout(&dlg);

// Add input rows
form->addRow("Username", usernameEdit);
form->addRow("Email", emailEdit);
form->addRow("Birthday", birthEdit);
form->addRow("GitHub", githubEdit);

// Ok / Cancel buttons
QDialogButtonBox* buttons = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
form->addRow(buttons);

if (dlg.exec() != QDialog::Accepted) return;   // blocks until user clicks OK or Cancel
// read inputs and apply changes
```

### Ban Confirmation (`QMessageBox`)

```cpp
const auto reply = QMessageBox::question(
    this,
    "Confirm Ban",
    "Ban user \"" + targetName + "\"?",
    QMessageBox::Yes | QMessageBox::No,
    QMessageBox::No           // default button
);
if (reply != QMessageBox::Yes) return;
```

### Banned Account Appeal (`QDialog` + `QTextEdit`)

When a banned user tries to log in, a custom dialog is built inline with a `QTextEdit` for the appeal message:

```cpp
QDialog banDlg(this);
banDlg.resize(420, 280);
auto* v = new QVBoxLayout(&banDlg);
v->addWidget(new QLabel("Your account has been banned..."));
auto* te = new QTextEdit(&banDlg);
te->setPlaceholderText("Write 1–2 lines for the admin…");
auto* submit = new QPushButton("Submit appeal");
connect(submit, &QPushButton::clicked, &banDlg, [&]() {
    // validate and submit
    modSystem.addAppeal(bu->userID, te->toPlainText().toStdString());
    banDlg.accept();
});
v->addWidget(te);
v->addWidget(submit);
banDlg.exec();
```

### Add Members Dialog (`QDialog` + `QScrollArea` + `QCheckBox`)

```cpp
QDialog dlg(this);
dlg.resize(400, 420);
auto* scroll = new QScrollArea(&dlg);
auto* inner = new QWidget;
auto* innerLay = new QVBoxLayout(inner);

// Populate checkboxes from friend list
for (int fi = 0; fi < me->friendCount; fi++) {
    auto* cb = new QCheckBox(
        QString::fromStdString(me->friends[fi]->userName), inner);
    innerLay->addWidget(cb);
    boxes.push_back({cb, me->friends[fi]->userID});
}
scroll->setWidget(inner);

if (dlg.exec() == QDialog::Accepted) {
    // collect checked friends
    for (const auto& pr : boxes)
        if (pr.first->isChecked())
            selected.push_back(pr.second);
}
```

---

## 10. QTimer — Deferred Execution

`QTimer::singleShot()` is used to defer the scroll-to-bottom action until after the layout updates:

```cpp
void MessagePage::scrollToBottom() {
    QTimer::singleShot(50, this, [this]() {
        messagesScrollArea->verticalScrollBar()->setValue(
            messagesScrollArea->verticalScrollBar()->maximum()
        );
    });
}
```

Without the timer, the scroll would happen before Qt finishes laying out the new message bubble, resulting in the scroll stopping before the last message is visible.

---

## 11. String Conversion — `std::string` ↔ `QString`

The backend uses `std::string`. Qt uses `QString`. All conversions happen at the UI layer:

```cpp
// std::string → QString (for display)
QString name = QString::fromStdString(users[i]->userName);
QString::fromStdString(users[m_loggedInIndex]->email)

// QString → std::string (for backend calls)
login(username.toStdString(), password.toStdString())
me->userName = newName.toStdString();
me->email    = emailEdit->text().trimmed().toStdString();
```

**Rule:** never include `QString` in backend header files. The conversion always happens in the frontend.

---

## 12. Qt Object Name and Property System

Every widget that needs to be styled via QSS gets an **object name**:

```cpp
QPushButton* btn = new QPushButton("Ban");
btn->setObjectName("btnBan");   // used in QSS as #btnBan { ... }
```

**Dynamic properties** are used for the real-time admin search filter:

```cpp
// Store searchable text as a property on each row widget:
row->setProperty(
    "adminSearchText",
    name + " " + QString::number(userID) + " " + email + " " + role
);

// Later, filter by reading the property:
const QString blob = w->property("adminSearchText").toString().toLower();
w->setVisible(blob.contains(searchQuery));
```

Properties are also used to toggle like button state:

```cpp
likeBtn->setProperty("isLiked", isLikedByMe);
// QSS can then style it:
// QPushButton[isLiked="true"] { color: red; }
```

---

## 13. QFileDialog — Image Picker

Profile photo selection uses `QFileDialog::getOpenFileName()`:

```cpp
const QString path = QFileDialog::getOpenFileName(
    this,                              // parent window
    "Select Profile Photo",            // dialog title
    "",                                // starting directory
    "Images (*.png *.jpg *.jpeg *.gif *.bmp *.webp)"  // file filter
);
if (path.isEmpty()) return;    // user cancelled
selectedPhotoPath = path;
photoLabel->setText(path);
```

---

## 14. The MOC — Meta-Object Compiler

Qt requires an extra build step: the **Meta-Object Compiler (moc)** processes every `.h` file that contains `Q_OBJECT` and generates a `moc_ClassName.cpp` file with the signal/slot dispatch table.

CMake handles this automatically with:

```cmake
set(CMAKE_AUTOMOC ON)
```

The `CMakeLists.txt` also explicitly passes the backend include directory to MOC:

```cmake
set_target_properties(ConnectifyUI PROPERTIES
    AUTOMOC_MOC_OPTIONS
        "-I${CMAKE_SOURCE_DIR}/backend/include;-I${CMAKE_SOURCE_DIR}/backend"
)
```

This is necessary because Qt's MOC tool needs to resolve `#include "user.h"` inside the frontend headers, which would otherwise fail since MOC runs from a different working directory than the compiler.
