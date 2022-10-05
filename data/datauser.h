#ifndef DATAUSER_H
#define DATAUSER_H

#include <QDebug>
#include <QObject>
#include <QJsonObject>
#include <common/ProcJsonEasy.h>


/**
 * @brief 사용자 정보 데이터 클래스
 */
class DataUser : public QObject
{
    Q_OBJECT
public:
    explicit DataUser(QObject *parent = nullptr);
    ~DataUser();

    void setDataUser(const QJsonObject &p_json);
    void setUserInfo(const QJsonObject &p_json);
    void setShazamInfo(const QJsonObject&);

    QString getEmail() const;

    QString getAccess_token() const;

    QString getRefresh_token() const;

    QString getToken_type() const;

    QString getUsername() const;

    QString getUserBIO() const;

    int getUserNo() const;

    int getUserPrivacy() const;

    int getUserRank() const;

    QString getShazam_token() const;

    void setUserName(QString p_userName){ username = p_userName; }
    void setUserBIO(QString p_userBIO){ bio = p_userBIO; }

    int getExp() const;
    int getIat() const;
    QString getUserFriend() const;
    QString getRosetubeRecentPlaylist() const;
    QString getSub() const;

    void setDeviceChange(bool flag);
    bool isChanged();

    bool isValid();    

    bool flag_rosehome = false;
    QJsonObject rosehome_obj;

    bool flag_music = false;
    QJsonObject music_obj;

    bool flag_roseradio = false;
    QJsonObject roseradio_obj;

    bool flag_rosetube = false;
    QJsonObject rosetube_obj;

    bool flag_home_filter = false;
    int enable_home_filter[30];

    bool flag_favorite_filter = false;
    int enable_favorite_filter[30];
    int enable_favRTrack_filter = 0;
    int enable_favRAlbum_filter = 0;
    int enable_favRPlaylist_filter = 0;

    int enable_favorite_order[10];
    int enable_favRTrack_order = 0;
    int enable_favRAlbum_order = 0;
    int enable_favRPlaylist_order = 0;

    bool flag_favMusic_order = false;
    int enable_favMusic_order[10];
    int enable_favMTrack_order = 0;
    int enable_favMAlbum_order = 0;
    int enable_favMPlaylist_order = 0;

private:
    QString email;
    QString username;
    QString bio;

    QString access_token;
    QString refresh_token;
    QString token_type;

    QString access_token_api;
    QString refresh_token_api;
    QString token_type_api;

    QString access_token_shazam;
    QString expired_shazam;

    int no;
    int rank;
    int privacy;

    bool device_change = false;

    // acess_token to base64
    int exp;
    int iat;
    QString userFriend;
    QString rosetubeRecentPlaylist;
    QString sub;

};

#endif // DATAUSER_H
