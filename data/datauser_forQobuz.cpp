#include "datauser_forQobuz.h"

DataUser_forQOBUZ::DataUser_forQOBUZ(QObject *parent) : QObject(parent)
{
    this->setLogout();      // for init
}

QString DataUser_forQOBUZ::getUserAuthToken() const
{
    return user_auth_token;
}

QString DataUser_forQOBUZ::getUserId() const
{
    return user_Id;
}

bool DataUser_forQOBUZ::isLogined() const {
    return flagLogin;
}

/**
 * @brief Qobuz 서비스 로그인이 된 상태를 저장한다.
 * @param sessionId
 * @param countryCode
 * @param userId
 * @param flagSavedLoginInfo
 */
void DataUser_forQOBUZ::setLogin(const QString &user_auth_token, const QString &user_name, const QString &user_Id, const bool flagSavedLoginInfo){
    this->flagLogin = true;
    this->user_auth_token = user_auth_token;
    this->user_name = user_name;
    this->user_Id = user_Id;
    this->flagSavedLoginInfo = flagSavedLoginInfo;
}

/**
 * @brief Qobuz 서비스 로그아웃 상태를 저장한다.
 */
void DataUser_forQOBUZ::setLogout(){
    this->flagLogin = false;
    this->user_auth_token = "";
    this->user_name = "";
    this->user_Id = "";
    this->flagSavedLoginInfo = false;

}

QString DataUser_forQOBUZ::getUsername() const
{
    return this->user_name;
}

void DataUser_forQOBUZ::setUsername(const QString &value)
{
    this->user_name = value;
}


QString DataUser_forQOBUZ::getUserstreaming() const
{
    return this->user_straming;
}

void DataUser_forQOBUZ::setUserstreaming(const QString &value)
{
    this->user_straming = value;
}

bool DataUser_forQOBUZ::getHiRes(){
    return this->flagHires;
}

bool DataUser_forQOBUZ::getLossless(){
    return this->flagLossless;
}

void DataUser_forQOBUZ::setStream_state(const bool hires, const bool lossless){
    this->flagHires = hires;
    this->flagLossless = lossless;
}

bool DataUser_forQOBUZ::dlg_isShow() const{
    return this->dlg_enable;
}

void DataUser_forQOBUZ::dlg_set_state(const bool flag){
    this->dlg_enable = flag;
}
