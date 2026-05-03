#pragma once

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QByteArray>
#include <QDateTime>

class QLabel;
class QPushButton;
class QTextEdit;
class QProgressBar;
class QLineEdit;
class QComboBox;

// ─────────────────────────────────────────────────────────────
//  StoredPost  — metadata written alongside the saved image
// ─────────────────────────────────────────────────────────────
struct StoredPost {
    QString   localId;        // client-side UUID (always present)
    QString   postId;         // server-assigned ID (filled after upload)
    QString   localImagePath; // absolute path to the stored image copy
    QString   caption;
    QString   tags;
    QString   visibility;
    QDateTime createdAt;
    bool      uploaded{ false };// true once server confirmed
};

// ─────────────────────────────────────────────────────────────
//  ImagePostUploader
//  Composes, locally stores, and uploads an image post.
//
//  Storage layout (under storageRoot):
//    <storageRoot>/
//      images/   ← image copies  (YYYY-MM-DD_<uuid>.<ext>)
//      posts/    ← JSON metadata (<uuid>.json)
// ─────────────────────────────────────────────────────────────
class ImagePostUploader : public QWidget
{
    Q_OBJECT

public:
    explicit ImagePostUploader(QWidget* parent = nullptr);
    ~ImagePostUploader() override = default;

    void setApiBaseUrl(const QString& url);
    void setAuthToken(const QString& token);

    // Override default storage root (QStandardPaths::AppDataLocation)
    void    setStorageRoot(const QString& dirPath);
    QString storageRoot() const { return m_storageRoot; }

signals:
    void postSavedLocally(const StoredPost& post);  // image stored, before upload
    void postUploaded(const StoredPost& post);       // server confirmed, post.postId set
    void uploadFailed(const QString& error);

private slots:
    void onSelectImageClicked();
    void onUploadClicked();
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onUploadFinished(QNetworkReply* reply);
    void onClearClicked();

private:
    // UI
    void setupUi();
    void applyStyles();
    void setPreviewImage(const QString& filePath);
    void setUiEnabled(bool enabled);
    void showStatus(const QString& msg, bool isError = false);

    // Local storage
    bool        ensureStorageDirs();
    StoredPost  saveLocally(const QString& srcPath,
        const QString& caption,
        const QString& tags,
        const QString& visibility);
    bool        writeMetadata(const StoredPost& post);
    bool        updateMetadataAfterUpload(const StoredPost& post);
    QString     newUuid() const;

    // Network
    QByteArray  buildMultipartBody(const QByteArray& imageData,
        const QString& fileName,
        const QString& caption,
        const QString& visibility,
        QByteArray& boundaryOut);

    // State
    QString    m_apiBaseUrl;
    QString    m_authToken;
    QString    m_storageRoot;
    QString    m_selectedFilePath;
    QPixmap    m_previewPixmap;
    StoredPost m_pendingPost;         // set by saveLocally() before upload

    QNetworkAccessManager* m_networkManager{ nullptr };
    QNetworkReply* m_activeReply{ nullptr };

    // Widgets
    QLabel* m_imagePreviewLabel{ nullptr };
    QPushButton* m_selectImageBtn{ nullptr };
    QTextEdit* m_captionEdit{ nullptr };
    QLineEdit* m_tagsEdit{ nullptr };
    QComboBox* m_visibilityCombo{ nullptr };
    QPushButton* m_uploadBtn{ nullptr };
    QPushButton* m_clearBtn{ nullptr };
    QProgressBar* m_progressBar{ nullptr };
    QLabel* m_statusLabel{ nullptr };
    QLabel* m_fileInfoLabel{ nullptr };
};
