#ifndef PROC_UPLOADTHUMBNAIL_H
#define PROC_UPLOADTHUMBNAIL_H

#include <QFile>
#include <QObject>
#include <QPixmap>

#include <common/filedownloader.h>

/**
 * @brief 썸네일 업로드 Proc @n
 *      Favor, 플레이리스트 담기시 썸네일 업로드 필요성 확인
 */
class Proc_UploadThumbnail : public QObject
{
    Q_OBJECT
public:
    explicit Proc_UploadThumbnail(QObject *parent = nullptr);
    ~Proc_UploadThumbnail();
    void uploadThumbnails(const QString &p_thumbPath, const QStringList &listUrl);
    void uploadThumbnails(const QPixmap &pixmap, const QStringList &listUrl);
    void uploadThumnail(const QString &p_thumbPath, const QString &p_url);
    void uploadThumnail(const QPixmap &pixmap, const QString &p_url);

signals :
    void signal_uploadedThumbnail();

private slots:
    void slot_networkManagerFinished();
    void slot_replyFinished();
    void slot_loadImage();

private :
    void setInit();
    void saveAlbumImgToFile(const QString &p_url);

    FileDownloader *fileDownloader;
    QFile *fileUpload;
    QPixmap pixmapThumb;
};

#endif // PROC_UPLOADTHUMBNAIL_H
