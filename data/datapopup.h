#ifndef DATAPOPUP_H
#define DATAPOPUP_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 옵션팝업 관련 데이터 클래스 @n
 *      벅스 타이달을 제외한 옵션팝업 사용하는 대항목 필수 데이터
 */
class DataPopup : public QObject
{
    Q_OBJECT
public:
    explicit DataPopup(QObject *parent = nullptr);

    /// 앨범명
    QString getAlbum() const;
    void setAlbum(const QString &value);

    /// 오디오 PK
    QString getId() const;
    void setId(const QString &value);

    /// 앨범 PK
    int getAlbum_id() const;
    void setAlbum_id(int value);

    /// 가수명
    QString getArtist() const;
    void setArtist(const QString &value);

    /// 오디오 제목
    QString getTitle() const;
    void setTitle(const QString &value);

    /// 오디오 파일 경로 (.mp3)
    QString getData() const;
    void setData(const QString &value);

    /// 비디오 재생시간
    int getDuration() const;
    void setDuration(int value);

    /// 비디오 파일경로
    QString getFilePath() const;
    void setFilePath(const QString &value);

    /// 로즈튜브 채널명
    QString getChannelName() const { return this->channelName; };
    void setChannelName(const QString &value){ this->channelName = value; };

    /// 로즈튜브 채널아이디
    QString getChannelId() const { return this->channelId; };
    void setChannelId(const QString &value){ this->channelId = value; };

    QString getThumbnailUrl() const { return this->thumbnailUrl; };
    void setThumbnailUrl(const QString &value){ this->thumbnailUrl = value; };



    void setJsonData(const QJsonObject &p_data);
    QJsonObject getJsonData() const;
    QJsonObject getJsonDataForNetworkPlay() const;


    void setName(const QString &p_name);
    void setPath(const QString &p_path);

    QString getAlbum_art() const;
    void setAlbum_art(const QString &value);

    QString getAlbum_key() const;
    void setAlbum_key(const QString &value);

    QString getArtist_key() const;
    void setArtist_key(const QString &value);

    QString getArtist_id() const;
    void setArtist_id(const QString &value);

    QString getAlbum_mime() const;
    void setAlbum_mime(const QString &mime);

    int getFavorite_id() const;

    void setFavorite_id(int value);

private:
    // 공통
    QString id="";
    QString title="";

    // MUSIC 전용
    QString album = "";
    int album_id = 0;
    QString artist = "";
    QString data = "";
    QString album_art = "";
    QString album_key = "";
    QString artist_key = "";
    QString artist_id = "";
    QString album_mime = "";
    int samplerate = 0;
    int bitdepth = 0;


    // VIDEO 전용
    int duration=0;
    QString filePath="";

    // 라디오 전용
    QString country;
    bool favorite=false;
    bool isSelect=false;
    QString radio_thumbnail_url;

    // 로즈튜브 전용
    QString channelId = "";
    QString channelName = "";
    QString thumbnailUrl = "";


    // MUSIC NETWORK PLAY 전용(ex.친구 플레이리스트 재생)  => 사용안하는멤버변수 삭제하고 강제세팅해둠.
    QString ext="";
    QString id_loginInfo="";
    QString pw_loginInfo="";
    QString ip="";
    //bool isCloud = false;
    //bool isDir = false;
    bool isFile = false;
    //bool isNetworkAdd = false;
    bool isServer = false;
    //bool isShare = false;
    //bool isThumbNailGetting = false;
    //bool isUp = false;
    QString name="";
    QString path="";
    QString smbParamPath="";
    QString thumbnail="";

    // FAVORITE
    int favorite_id=0;

};

#endif // DATAPOPUP_H
