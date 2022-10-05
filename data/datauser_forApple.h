#ifndef DATAUSER_FORAPPLE_H
#define DATAUSER_FORAPPLE_H

#include <QObject>
#include <QJsonObject>


/**
 * @brief 사용자 정보 데이터 클래스(벅스)
 */
class DataUser_forApple : public QObject
{
    Q_OBJECT

public:
    const QString appleAPI_url = "https://api.music.apple.com";

public:
    explicit DataUser_forApple(QObject *parent = nullptr);

    QString get_user_token(){ return this->user_token; }
    QString get_store_front(){ return this->store_front; }

    bool isLogined() const;
    void setLogin(const QString &userToken, const QString &storefront, const bool flagSavedLoginInfo);
    void setLogout();

    bool flag_rosehome = false;
    QJsonObject rosehome_obj;

private:

    // 로그인을 위한 정보
    bool flagSavedLoginInfo = false;        ///< 로그인을 위한 정보 - Apple 사용자 정보를 저장할지의 여부. (자동로그인에서 사용)

    bool flagLogin = false;                 ///< 현재 어플에서 로그인을 했는지의 상태값

    QString user_token;
    QString store_front;
};
#endif // DATAUSER_FORAPPLE_H
