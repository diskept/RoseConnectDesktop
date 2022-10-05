#ifndef MEDIAPLAYERTHREAD_H
#define MEDIAPLAYERTHREAD_H

#include <QThread>
#include <QMediaPlayer>
#include <QPixmap>
#include <QEventLoop>

/**
 * @brief 미디어 플레이어 관리 Thread @n
 *      외부 라이브러리 없이 QT로 미디어 파일 MIME 정보 GET
 *
 * @note 성능 문제로 추후 OPEN SOURCE 로 변경 필요
 */
class MediaPlayerThread : public QThread
{
    Q_OBJECT
public:
    explicit MediaPlayerThread(const QString &p_filePath, const int &p_index, QObject *parent = nullptr);
    ~MediaPlayerThread();
    void stop();

signals:
    void signal_loadedMedia(const QJsonObject &p_fileInfo, const QPixmap &pixmap);
    void signal_done();

protected:
    void run() override;

private:
    QMediaPlayer *player;
    QString filePath;
    int index;
    bool flagDone = false;

public slots:
     void slot_loadedMeidaFile(QMediaPlayer::MediaStatus);
};

#endif // MEDIAPLAYERTHREAD_H
