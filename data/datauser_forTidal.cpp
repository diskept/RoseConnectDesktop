#include "data/datauser_forTidal.h"
#include <QWidget>


DataUser_forTIDAL::DataUser_forTIDAL(QObject *parent) : QObject(parent)
{
    this->flagLogin = false;
}


/**
 * @brief set - bugs_authCode
 * @param p_bugs_authCode
 */
void DataUser_forTIDAL::set_tidal_login_authCode(const QString p_tidal_login_authCode){
    this->tidal_login_authCode = p_tidal_login_authCode;
    this->flagLogin = !this->tidal_login_authCode.isEmpty();
}


bool DataUser_forTIDAL::isLogined() const {
    return this->flagLogin;
}

bool DataUser_forTIDAL::isMainOK() const {
    return flagMain;
}


/**
 * @brief TidalTokenInfo 정보로 멤버변수 설정
 * @param tokenInfo
 */
void DataUser_forTIDAL::set_loginState(const QString p_accessToken, const QString p_refreshToken, const QString p_countryCode, const QString p_seesionId, const int p_userId, const QString p_userName){
    if(p_accessToken.isEmpty()){
        this->flagLogin = false;
    }
    else{
        this->flagLogin = true;
        this->access_token = p_accessToken;
        this->refresh_token = p_refreshToken;
        this->countryCode = p_countryCode;
        this->sessionId = p_seesionId;
        this->userId = p_userId;
        this->username = p_userName;
    }
}


/**
 * @brief DataUser_forBugs::set_logoutState
 */
void DataUser_forTIDAL::set_logoutState(){
    this->flagLogin = false;
    this->access_token = "";
    this->refresh_token = "";
    this->countryCode = "US";
    this->sessionId = "";
    this->username = "";
    this->flag_login_wait = false;
}


void DataUser_forTIDAL::set_soundState(const int p_highestSound, const int p_soundQuality){
    this->userHighestSound = p_highestSound;
    this->userSoundQuality = p_soundQuality;
}



/*DataUser_forTIDAL::DataUser_forTIDAL(QObject *parent) : QObject(parent)
{
    this->setLogout();      // for init
}*/

QString DataUser_forTIDAL::getSessionId() const
{
    return sessionId;
}

QString DataUser_forTIDAL::getCountryCode() const
{
    return countryCode;
}

int DataUser_forTIDAL::getUserId() const
{
    return userId;
}

QString DataUser_forTIDAL::getUsername() const
{
    return username;
}
int DataUser_forTIDAL::getHeighestSound() const
{
    return userHighestSound;
}

int DataUser_forTIDAL::getSoundQuality() const
{
    return userSoundQuality;
}

/*bool DataUser_forTIDAL::isLogined() const {
    return flagLogin;
}*/

/**
 * @brief 타이달 서비스 로그인이 된 상태를 저장한다.
 * @param sessionId
 * @param countryCode
 * @param userId
 * @param flagSavedLoginInfo
 */
void DataUser_forTIDAL::setLogin(const QString &sessionId, const QString &countryCode, const int &userId, const QString &userName, const bool flagSavedLoginInfo){
    this->flagLogin = true;
    this->sessionId = sessionId;
    this->countryCode = countryCode;
    this->userId = userId;    
    this->username = userName;
    this->flagSavedLoginInfo = flagSavedLoginInfo;
}

/**
 * @brief 타이달 서비스 로그아웃 상태를 저장한다.
 */
void DataUser_forTIDAL::setLogout(){
    this->flagLogin = false;
    this->sessionId = "";
    this->countryCode = "US";           // default countryCode
    this->userId = 0;
    this->flagSavedLoginInfo = false;
    this->flagMain = false;
    this->flag_login_wait = false;
}

void DataUser_forTIDAL::setMain(const bool flagMainOK){
 this->flagMain = flagMainOK;
}

/*void DataUser_forTIDAL::setUsername(const QString &value)
{
    username = value;
}*/


bool DataUser_forTIDAL::dlg_isShow() const{
    return this->dlg_enable;
}

void DataUser_forTIDAL::dlg_set_state(const bool flag){
    this->dlg_enable = flag;
}
