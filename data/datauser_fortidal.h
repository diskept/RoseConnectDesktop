#ifndef DATAUSER_FORTIDAL_H
#define DATAUSER_FORTIDAL_H

#include "tidal/tidal_struct.h"

#include <QObject>
#include <QJsonObject>

/**
 * @brief 사용자정보 데이터 클래스(타이달)
 */
class DataUser_forTIDAL : public QObject
{
    Q_OBJECT
public:
    explicit DataUser_forTIDAL(QObject *parent = nullptr);

    QString getSessionId() const;
    QString getCountryCode() const;
    int getUserId() const;
    QString getUsername() const;
    int getHeighestSound() const;
    int getSoundQuality() const;


    bool dlg_isShow() const;
    void dlg_set_state(const bool flag);

    //bool isLogined() const;
    void setLogin(const QString &sessionId, const QString &countryCode, const int &userId, const QString &userName, const bool flagSavedLoginInfo);
    void setLogout();
    void setMain(const bool flagMainOK);


    void set_tidal_login_authCode(const QString p_tidal_login_authCode);
    bool isLogined() const;
    bool isMainOK() const;
    bool flag_login_wait = false;

    QString get_login_authCode(){ return this->tidal_login_authCode; }

    void set_loginState(const QString p_accessToken, const QString p_refreshToken, const QString p_countryCode, const QString p_seesionId, const int p_userId, const QString p_userName);
    void set_soundState(const int p_highestSound, const int p_soundQuality);

    QString get_access_token(){ return this->access_token; }
    QString get_refresh_token(){ return this->refresh_token; }

    void set_logoutState();

    void savePageData(QJsonObject p_data){this->page_data = p_data;};//c230421
    QJsonObject getPageData(){ return this->page_data;};//c230421

    QString getNickName(){ return this->nickName; }
    QString getProductName(){ return this->product_name; }
    void setNickName(QString p_nickName){ this->nickName = p_nickName; }
    void setProductName(QString p_productName){ this->product_name = p_productName; }

    bool flag_rosehome = false;
    QJsonObject rosehome_obj;

    QList<tidal::AlbumItemData> list_visualData;

private :

    QJsonObject page_data;//c230421

    // 로그인을 위한 정보
    bool flagSavedLoginInfo = false;    ///< 로그인을 위한 정보 - 타이달 사용자 정보를 저장할지의 여부. (자동로그인에서 사용)
    QString username = "";              ///< 로그인을 위한 정보 - 타이달 사용자 이메일

    bool flagLogin = false;         ///< 현재 어플에서 로그인을 했는지의 상태값
    bool flagMain = false;

    QString sessionId = "";         ///< 로그인 성공 데이터 저장정보
    QString countryCode = "";     ///< 로그인 성공 데이터 저장정보
    int userId = 0;            ///< 로그인 성공 데이터 저장정보

    int userHighestSound = 0;
    int userSoundQuality = 0;

    //bool flagLogin = false;                 ///< 현재 로그인을 했는지의 상태값

    QString tidal_login_authCode;              ///< Authorization Code
    QString access_token;
    QString refresh_token;
//    QString token_type;

    QString nickName;
    QString product_name;

    bool dlg_enable = false;

};

#endif // DATAUSER_FORTIDAL_H
