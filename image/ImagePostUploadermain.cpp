#include <QApplication>
#include <QStandardPaths>
#include <QDebug>
#include "ImagePostUploader.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MySocialApp");   // used by default storageRoot path

    ImagePostUploader uploader;

    // ── Backend config ─────────────────────────────
    uploader.setApiBaseUrl("https://api.yourapp.com/v1");
    uploader.setAuthToken("YOUR_JWT_TOKEN_HERE");
    QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    uploader.setStorageRoot(docs + "/MySocialApp/posts_storage");
    // ── (Optional) custom storage location ─────────
    // uploader.setStorageRoot("C:/MyApp/posts_storage");
    qDebug() << "Storage root:" << uploader.storageRoot();

    // ── React to events ─────────────────────────────
    QObject::connect(&uploader, &ImagePostUploader::postSavedLocally,
        [](const StoredPost& p) {
            qDebug() << "[LOCAL] Saved:" << p.localImagePath;
            // Storage layout:
            //   images/<date>_<uuid>.jpg
            //   posts/<uuid>.json    ← uploaded: false
        });

    QObject::connect(&uploader, &ImagePostUploader::postUploaded,
        [](const StoredPost& p) {
            qDebug() << "[UPLOAD] Done. Server ID:" << p.postId;
            qDebug() << "         Local copy:    " << p.localImagePath;
            // posts/<uuid>.json is now updated with:
            //   uploaded: true
            //   post_id: "<server id>"
        });

    QObject::connect(&uploader, &ImagePostUploader::uploadFailed,
        [](const QString& err) {
            qDebug() << "[UPLOAD] Failed:" << err;
            // Local copy is still safe — you can retry later
        });

    uploader.resize(500, 750);
    uploader.show();
    return app.exec();
}
