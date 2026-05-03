#include "ImagePostUploader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QImageReader>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMimeDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QStandardPaths>
#include <QDebug>

// ═══════════════════════════════════════════════════════════════
//  Construction
// ═══════════════════════════════════════════════════════════════
ImagePostUploader::ImagePostUploader(QWidget* parent)
    : QWidget(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // Default storage root: e.g. C:/Users/<user>/AppData/Local/<AppName>/
    m_storageRoot = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation) + "/posts_storage";

    setupUi();
    applyStyles();

    connect(m_networkManager, &QNetworkAccessManager::finished,
        this, &ImagePostUploader::onUploadFinished);
}

// ═══════════════════════════════════════════════════════════════
//  Public setters
// ═══════════════════════════════════════════════════════════════
void ImagePostUploader::setApiBaseUrl(const QString& url) { m_apiBaseUrl = url; }
void ImagePostUploader::setAuthToken(const QString& token) { m_authToken = token; }
void ImagePostUploader::setStorageRoot(const QString& dirPath) { m_storageRoot = dirPath; }

// ═══════════════════════════════════════════════════════════════
//  UI setup
// ═══════════════════════════════════════════════════════════════
void ImagePostUploader::setupUi()
{
    setWindowTitle("New Post");
    setMinimumWidth(480);

    m_imagePreviewLabel = new QLabel(this);
    m_imagePreviewLabel->setObjectName("previewLabel");
    m_imagePreviewLabel->setAlignment(Qt::AlignCenter);
    m_imagePreviewLabel->setMinimumSize(440, 280);
    m_imagePreviewLabel->setText("No image selected\nClick \"Choose Photo\" to get started");
    m_imagePreviewLabel->setWordWrap(true);

    m_fileInfoLabel = new QLabel(this);
    m_fileInfoLabel->setObjectName("fileInfoLabel");
    m_fileInfoLabel->setAlignment(Qt::AlignCenter);

    m_selectImageBtn = new QPushButton("📷  Choose Photo", this);
    m_selectImageBtn->setObjectName("selectBtn");

    m_clearBtn = new QPushButton("✕  Clear", this);
    m_clearBtn->setObjectName("clearBtn");
    m_clearBtn->setEnabled(false);

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(m_selectImageBtn);
    btnRow->addWidget(m_clearBtn);

    auto* captionLbl = new QLabel("Caption", this);
    captionLbl->setObjectName("fieldLabel");
    m_captionEdit = new QTextEdit(this);
    m_captionEdit->setObjectName("captionEdit");
    m_captionEdit->setPlaceholderText("Write a caption…");
    m_captionEdit->setMaximumHeight(90);

    auto* tagsLbl = new QLabel("Tags (comma-separated)", this);
    tagsLbl->setObjectName("fieldLabel");
    m_tagsEdit = new QLineEdit(this);
    m_tagsEdit->setObjectName("tagsEdit");
    m_tagsEdit->setPlaceholderText("#travel, #photo, #nature");

    auto* visLbl = new QLabel("Audience", this);
    visLbl->setObjectName("fieldLabel");
    m_visibilityCombo = new QComboBox(this);
    m_visibilityCombo->setObjectName("visibilityCombo");
    m_visibilityCombo->addItems({ "🌍  Public", "👥  Friends", "🔒  Only Me" });

    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("progressBar");
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    m_uploadBtn = new QPushButton("Share Post", this);
    m_uploadBtn->setObjectName("uploadBtn");
    m_uploadBtn->setEnabled(false);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);
    root->addWidget(m_imagePreviewLabel);
    root->addWidget(m_fileInfoLabel);
    root->addLayout(btnRow);
    root->addSpacing(8);
    root->addWidget(captionLbl);
    root->addWidget(m_captionEdit);
    root->addWidget(tagsLbl);
    root->addWidget(m_tagsEdit);
    root->addWidget(visLbl);
    root->addWidget(m_visibilityCombo);
    root->addSpacing(8);
    root->addWidget(m_progressBar);
    root->addWidget(m_statusLabel);
    root->addWidget(m_uploadBtn);

    connect(m_selectImageBtn, &QPushButton::clicked, this, &ImagePostUploader::onSelectImageClicked);
    connect(m_uploadBtn, &QPushButton::clicked, this, &ImagePostUploader::onUploadClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &ImagePostUploader::onClearClicked);
}

void ImagePostUploader::applyStyles()
{
    setStyleSheet(R"(
        ImagePostUploader { background: #0f0f0f; }

        QLabel#previewLabel {
            background: #1a1a1a; border: 2px dashed #333;
            border-radius: 12px; color: #555;
            font-size: 14px; padding: 20px;
        }
        QLabel#fileInfoLabel { color: #555; font-size: 12px; }
        QLabel#fieldLabel {
            color: #888; font-size: 11px; font-weight: bold;
            letter-spacing: 0.06em;
        }
        QPushButton#selectBtn {
            background: #1e1e1e; color: #e0e0e0;
            border: 1px solid #333; border-radius: 8px;
            padding: 10px 18px; font-size: 14px;
        }
        QPushButton#selectBtn:hover { background: #2a2a2a; border-color: #555; }

        QPushButton#clearBtn {
            background: transparent; color: #555;
            border: 1px solid #2a2a2a; border-radius: 8px;
            padding: 10px 18px; font-size: 14px;
        }
        QPushButton#clearBtn:hover  { color: #f87171; border-color: #f87171; }
        QPushButton#clearBtn:disabled { color: #2e2e2e; border-color: #1e1e1e; }

        QTextEdit#captionEdit, QLineEdit#tagsEdit {
            background: #1a1a1a; color: #e0e0e0;
            border: 1px solid #2e2e2e; border-radius: 8px;
            padding: 10px; font-size: 14px;
        }
        QTextEdit#captionEdit:focus, QLineEdit#tagsEdit:focus { border-color: #555; }

        QComboBox#visibilityCombo {
            background: #1a1a1a; color: #e0e0e0;
            border: 1px solid #2e2e2e; border-radius: 8px;
            padding: 10px; font-size: 14px;
        }
        QComboBox#visibilityCombo::drop-down { border: none; width: 28px; }
        QComboBox#visibilityCombo QAbstractItemView {
            background: #1e1e1e; color: #e0e0e0;
            selection-background-color: #333; border: 1px solid #333;
        }
        QPushButton#uploadBtn {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #7c3aed, stop:1 #a855f7);
            color: white; border: none; border-radius: 10px;
            padding: 14px; font-size: 15px; font-weight: bold;
        }
        QPushButton#uploadBtn:hover {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #6d28d9, stop:1 #9333ea);
        }
        QPushButton#uploadBtn:disabled { background: #2a2a2a; color: #555; }

        QProgressBar#progressBar {
            background: #1a1a1a; border: 1px solid #2e2e2e;
            border-radius: 6px; color: #e0e0e0;
            text-align: center; font-size: 12px; height: 22px;
        }
        QProgressBar#progressBar::chunk {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #7c3aed, stop:1 #a855f7);
            border-radius: 6px;
        }
        QLabel#statusLabel { color: #888; font-size: 13px; }
    )");
}

// ═══════════════════════════════════════════════════════════════
//  Slots
// ═══════════════════════════════════════════════════════════════
void ImagePostUploader::onSelectImageClicked()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Select Image", QString(),
        "Images (*.png *.jpg *.jpeg *.gif *.webp *.bmp *.tiff)"
    );
    if (!path.isEmpty())
        setPreviewImage(path);
}

void ImagePostUploader::onUploadClicked()
{
    if (m_selectedFilePath.isEmpty()) {
        showStatus("Please select an image first.", true);
        return;
    }
    if (m_apiBaseUrl.isEmpty()) {
        showStatus("API URL not configured.", true);
        return;
    }

    const QString caption = m_captionEdit->toPlainText().trimmed();
    const QString tags = m_tagsEdit->text().trimmed();
    const QString visibility = (m_visibilityCombo->currentIndex() == 0) ? "public"
        : (m_visibilityCombo->currentIndex() == 1) ? "friends"
        : "private";
    // ── 1. Save image + metadata locally ────────
    if (!ensureStorageDirs()) {
        showStatus("Cannot create local storage folder.", true);
        return;
    }

    m_pendingPost = saveLocally(m_selectedFilePath, caption, tags, visibility);
    if (m_pendingPost.localImagePath.isEmpty()) {
        showStatus("Failed to save image locally.", true);
        return;
    }

    emit postSavedLocally(m_pendingPost);
    showStatus("Image saved locally. Uploading…");

    // ── 2. Read saved copy for upload ────────────
    QFile file(m_pendingPost.localImagePath);
    if (!file.open(QIODevice::ReadOnly)) {
        showStatus("Cannot read saved image.", true);
        return;
    }
    const QByteArray imageData = file.readAll();
    file.close();

    const QFileInfo fi(m_pendingPost.localImagePath);

    // ── 3. Build & send multipart request ────────
    QByteArray boundary;
    const QByteArray body = buildMultipartBody(
        imageData, fi.fileName(), caption, visibility, boundary);

    QNetworkRequest request(QUrl(m_apiBaseUrl + "/posts/image"));
    request.setRawHeader("Authorization", ("Bearer " + m_authToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "multipart/form-data; boundary=" + boundary);
    request.setHeader(QNetworkRequest::ContentLengthHeader, body.size());
    if (!tags.isEmpty())
        request.setRawHeader("X-Post-Tags", tags.toUtf8());

    setUiEnabled(false);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);

    m_activeReply = m_networkManager->post(request, body);
    connect(m_activeReply, &QNetworkReply::uploadProgress,
        this, &ImagePostUploader::onUploadProgress);
}

void ImagePostUploader::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (bytesTotal > 0)
        m_progressBar->setValue(static_cast<int>(bytesSent * 100 / bytesTotal));
}

void ImagePostUploader::onUploadFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    m_activeReply = nullptr;
    setUiEnabled(true);

    if (reply->error() != QNetworkReply::NoError) {
        const QString err = reply->errorString();
        showStatus("Upload failed: " + err, true);
        emit uploadFailed(err);
        return;
    }

    const int statusCode = reply->attribute(
        QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

    if (statusCode == 200 || statusCode == 201) {
        // Extract server-assigned ID
        if (doc.isObject()) {
            const QJsonObject obj = doc.object();
            m_pendingPost.postId = obj.value("id").toString();
            if (m_pendingPost.postId.isEmpty())
                m_pendingPost.postId = obj.value("post_id").toString();
        }
        m_pendingPost.uploaded = true;

        // ── Persist the updated metadata ────────────
        updateMetadataAfterUpload(m_pendingPost);

        m_progressBar->setValue(100);
        showStatus("✓ Post shared and saved locally!");
        emit postUploaded(m_pendingPost);
    }
    else {
        QString serverMsg;
        if (doc.isObject())
            serverMsg = doc.object().value("message").toString();
        if (serverMsg.isEmpty())
            serverMsg = QString("HTTP %1").arg(statusCode);

        showStatus("Server error: " + serverMsg, true);
        emit uploadFailed(serverMsg);
    }
}

void ImagePostUploader::onClearClicked()
{
    m_selectedFilePath.clear();
    m_previewPixmap = QPixmap();
    m_pendingPost = StoredPost{};
    m_imagePreviewLabel->setPixmap(QPixmap());
    m_imagePreviewLabel->setText("No image selected\nClick \"Choose Photo\" to get started");
    m_fileInfoLabel->setText("");
    m_captionEdit->clear();
    m_tagsEdit->clear();
    m_progressBar->setVisible(false);
    m_progressBar->setValue(0);
    m_statusLabel->setText("");
    m_uploadBtn->setEnabled(false);
    m_clearBtn->setEnabled(false);
}

// ═══════════════════════════════════════════════════════════════
//  Local storage helpers
// ═══════════════════════════════════════════════════════════════

bool ImagePostUploader::ensureStorageDirs()
{
    const QDir root(m_storageRoot);
    return root.mkpath("images") && root.mkpath("posts");
}

StoredPost ImagePostUploader::saveLocally(const QString& srcPath,
    const QString& caption,
    const QString& tags,
    const QString& visibility)
{
    StoredPost post;
    post.localId = newUuid();
    post.caption = caption;
    post.tags = tags;
    post.visibility = visibility;
    post.createdAt = QDateTime::currentDateTime();
    post.uploaded = false;

    // Build destination filename:  2025-05-03_<uuid>.jpg
    const QFileInfo srcInfo(srcPath);
    const QString datePart = post.createdAt.toString("yyyy-MM-dd");
    const QString destName = datePart + "_" + post.localId + "." + srcInfo.suffix().toLower();
    const QString destPath = m_storageRoot + "/images/" + destName;

    // Copy image
    if (!QFile::copy(srcPath, destPath)) {
        qWarning() << "ImagePostUploader: failed to copy image to" << destPath;
        return {};          // empty localImagePath signals failure
    }

    post.localImagePath = destPath;

    // Write initial metadata
    if (!writeMetadata(post)) {
        qWarning() << "ImagePostUploader: failed to write metadata for" << post.localId;
        // Non-fatal — the image is saved; we just log the warning
    }

    return post;
}

bool ImagePostUploader::writeMetadata(const StoredPost& post)
{
    // Split tags string into a JSON array
    QJsonArray tagsArray;
    for (const QString& t : post.tags.split(',', Qt::SkipEmptyParts))
        tagsArray.append(t.trimmed());

    QJsonObject obj;
    obj["local_id"] = post.localId;
    obj["post_id"] = post.postId;          // empty until uploaded
    obj["local_image_path"] = post.localImagePath;
    obj["caption"] = post.caption;
    obj["tags"] = tagsArray;
    obj["visibility"] = post.visibility;
    obj["created_at"] = post.createdAt.toString(Qt::ISODate);
    obj["uploaded"] = post.uploaded;

    const QString metaPath = m_storageRoot + "/posts/" + post.localId + ".json";
    QFile f(metaPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return true;
}

bool ImagePostUploader::updateMetadataAfterUpload(const StoredPost& post)
{
    // Re-write the same file with uploaded=true and server post_id filled
    return writeMetadata(post);
}

QString ImagePostUploader::newUuid() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

// ═══════════════════════════════════════════════════════════════
//  UI helpers
// ═══════════════════════════════════════════════════════════════
void ImagePostUploader::setPreviewImage(const QString& filePath)
{
    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    if (!reader.canRead()) {
        showStatus("Cannot read image file.", true);
        return;
    }

    m_selectedFilePath = filePath;
    m_previewPixmap = QPixmap::fromImageReader(&reader);

    const QSize maxSize = m_imagePreviewLabel->size() - QSize(20, 20);
    m_imagePreviewLabel->setPixmap(
        m_previewPixmap.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    const QFileInfo fi(filePath);
    const qint64 bytes = fi.size();
    const QString sizeStr = bytes > 1024 * 1024
        ? QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " MB"
        : QString::number(bytes / 1024.0, 'f', 1) + " KB";

    m_fileInfoLabel->setText(QString("%1  ·  %2 × %3 px  ·  %4")
        .arg(fi.fileName())
        .arg(m_previewPixmap.width())
        .arg(m_previewPixmap.height())
        .arg(sizeStr));

    m_uploadBtn->setEnabled(true);
    m_clearBtn->setEnabled(true);
    showStatus("");
}

void ImagePostUploader::setUiEnabled(bool on)
{
    m_selectImageBtn->setEnabled(on);
    m_clearBtn->setEnabled(on && !m_selectedFilePath.isEmpty());
    m_captionEdit->setEnabled(on);
    m_tagsEdit->setEnabled(on);
    m_visibilityCombo->setEnabled(on);
    m_uploadBtn->setEnabled(on && !m_selectedFilePath.isEmpty());
}

void ImagePostUploader::showStatus(const QString& msg, bool isError)
{
    m_statusLabel->setText(msg);
    m_statusLabel->setStyleSheet(isError
        ? "color: #f87171; font-size: 13px;"
        : "color: #888;   font-size: 13px;");
}

// ═══════════════════════════════════════════════════════════════
//  Network helper — manual multipart/form-data body
// ═══════════════════════════════════════════════════════════════
QByteArray ImagePostUploader::buildMultipartBody(const QByteArray& imageData,
    const QString& fileName,
    const QString& caption,
    const QString& visibility,
    QByteArray& boundaryOut)
{
    boundaryOut = ("----QtBoundary" +
        QUuid::createUuid().toString(QUuid::WithoutBraces)
        .toLatin1().replace("-", "")).left(40);

    const QByteArray nl = "\r\n";
    const QByteArray dash = "--";
    const QString mimeType = QMimeDatabase().mimeTypeForFile(fileName).name();

    QByteArray body;

    // Image part
    body += dash + boundaryOut + nl;
    body += "Content-Disposition: form-data; name=\"image\"; filename=\""
        + fileName.toUtf8() + "\"" + nl;
    body += "Content-Type: " + mimeType.toUtf8() + nl + nl;
    body += imageData + nl;

    // Caption part
    body += dash + boundaryOut + nl;
    body += "Content-Disposition: form-data; name=\"caption\"" + nl + nl;
    body += caption.toUtf8() + nl;

    // Visibility part
    body += dash + boundaryOut + nl;
    body += "Content-Disposition: form-data; name=\"visibility\"" + nl + nl;
    body += visibility.toUtf8() + nl;

    // Closing boundary
    body += dash + boundaryOut + dash + nl;

    return body;
}
