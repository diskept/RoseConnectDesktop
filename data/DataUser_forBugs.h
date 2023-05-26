#ifndef DATAUSER_FORBUGS_H
#define DATAUSER_FORBUGS_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 사용자 정보 데이터 클래스(벅스)
 */
class DataUser_forBugs : public QObject
{
    Q_OBJECT

public:
    explicit DataUser_forBugs(QObject *parent = nullptr);

    void set_bugs_login_authCode(const QString p_bugs_login_authCode);
    bool isLogined() const;

    bool flag_login_wait = false;

    QString get_login_authCode(){ return this->bugs_login_authCode; }

    void set_loginState(const QString p_accessToken, const QString p_refreshToken);
    void set_logoutState();
    void savePageData(QJsonObject p_data){this->page_data = p_data;};//c230421
    QJsonObject getPageData(){ return this->page_data;};//c230421

    QString get_access_token(){ return this->access_token; }
    QString get_refresh_token(){ return this->refresh_token; }

    QString getNickName(){ return this->nickName; }
    QString getProductName(){ return this->product_name; }
    //void setNickName(QString p_nickName){ this->nickName = p_nickName; }
    //void setProductName(QString p_productName){ this->product_name = p_productName; }

    bool dlg_isShow() const;
    void dlg_set_state(const bool flag);

    bool dlg_isSignal() const;
    void dlg_set_signal(const bool flag);

    bool getAdult_yn(){ return this->adult_yn; }
    QString getProduct_display_end_dt(){ return this->product_display_end_dt; }
    bool getStreaming_yn(){ return this->streaming_yn; }
    bool getFlac_premium_yn(){ return this->flac_premium_yn; }
    void setNickName(QString p_nickName){ this->nickName = p_nickName; }
    void setProductName(QString p_productName){ this->product_name = p_productName; }
    void setAdult_yn(bool p_adult_yn){ this->adult_yn = p_adult_yn; }
    void setProduct_display_end_dt(QString p_product_display_end_dt){ this->product_display_end_dt = p_product_display_end_dt; }
    void setStreaming_yn(bool p_streaming_yn){ this->streaming_yn = p_streaming_yn; }
    void setFlac_premium_yn(bool p_flac_premium_yn){ this->flac_premium_yn = p_flac_premium_yn; }

    bool flag_rosehome = false;
    QJsonObject rosehome_obj;

private:
    bool flagLogin = false;                 ///< 현재 로그인을 했는지의 상태값
    QJsonObject page_data;//c230421
    QString bugs_login_authCode;              ///< Authorization Code
    QString access_token;
    QString refresh_token;
//    QString token_type;

    QString nickName;
    QString product_name;
    bool adult_yn;
    QString product_display_end_dt;
    bool streaming_yn;
    bool flac_premium_yn;

    bool dlg_enable = false;                ///< musicvideo, artists dialog 상태값
    bool dlg_signal_check = false;          ///< musicvideo, artists signal check

};

#endif // DATAUSER_FORBUGS_H
