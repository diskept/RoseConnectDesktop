#ifndef DATAVIDEOALBUM_H
#define DATAVIDEOALBUM_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 비디오 앨범 데이터 클래스
 * @details API 비디오앨범 키값을 변수로 사용
 */
class DataVideoAlbum : public QObject
{
    Q_OBJECT
public:
    explicit DataVideoAlbum(QObject *parent = nullptr);
    explicit DataVideoAlbum(QJsonObject jsonData, QObject *parent = nullptr);
    virtual ~DataVideoAlbum();

    void setData(const QJsonObject &p_json);

    QString getAlbum() const;

    int getAlbumCnt() const;

    QString getId() const;

    int getVideoDateaAdd() const;

    int getVideoDuration() const;

    QString getVideoFilePath() const;

    int getVideoSize() const;

    QString getVideoTitle() const;

    void setCoverImg(const QString &path);

    QString getVideoCoverImg() const;

private:
    QString id;
    QString album;
    QString videoTitle;
    QString videoFilePath;
    QString videoCoverImg;
    int albumCnt;
    int videoDateaAdd;
    int videoDuration;
    int videoSize;
};

#endif // DATAVIDEOALBUM_H
