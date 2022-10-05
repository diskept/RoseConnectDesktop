#ifndef DATAUSER_FORQOBUZ_H
#define DATAUSER_FORQOBUZ_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 사용자정보 데이터 클래스(타이달)
 */
class DataUser_forQOBUZ : public QObject
{
    Q_OBJECT
public:
    explicit DataUser_forQOBUZ(QObject *parent = nullptr);

    QString getUserAuthToken() const;
    QString getUserId() const;


    bool dlg_isShow() const;
    void dlg_set_state(const bool flag);

    bool isLogined() const;
    void setLogin(const QString &user_auth_token, const QString &user_name, const QString &user_Id, const bool flagSavedLoginInfo);
    void setLogout();

    QString getUsername() const;
    void setUsername(const QString &value);

    QString getUserstreaming() const;
    void setUserstreaming(const QString &value);

    bool getHiRes();
    bool getLossless();
    void setStream_state(const bool hires, const bool lossless);

    bool flag_login_wait = false;

    bool flag_main_filter = false;
    bool flag_explore_filter = false;
    bool flag_search_filter = false;

    int enable_main_filter[30];
    int enable_explore_filter[30];
    int enable_search_filter[30];

    bool flag_rosehome = false;
    QJsonObject rosehome_obj;

private :

    // 로그인을 위한 정보
    bool flagSavedLoginInfo = false;    ///< 로그인을 위한 정보 - qobuz 사용자 정보를 저장할지의 여부. (자동로그인에서 사용)

    QString user_name = "";             ///< 로그인을 위한 정보 - qobuz 사용자 이메일
    QString user_Id = "";               ///< 로그인 성공 데이터 저장정보
    QString user_auth_token = "";       ///< 로그인 성공 데이터 저장정보
    QString user_straming = "";

    bool flagLogin = false;             ///< 현재 어플에서 로그인을 했는지의 상태값

    bool dlg_enable = false;

    bool flagHires = false;
    bool flagLossless = false;

};

#endif // DATAUSER_FORQOBUZ_H
