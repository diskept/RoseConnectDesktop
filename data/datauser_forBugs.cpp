#include "datauser_forBugs.h"
#include <QWidget>


DataUser_forBugs::DataUser_forBugs(QObject *parent) : QObject(parent)
{
    this->flagLogin = false;
}


/**
 * @brief set - bugs_authCode
 * @param p_bugs_authCode
 */
void DataUser_forBugs::set_bugs_login_authCode(const QString p_bugs_login_authCode){
    this->bugs_login_authCode = p_bugs_login_authCode;
    this->flagLogin = !this->bugs_login_authCode.isEmpty();
}


bool DataUser_forBugs::isLogined() const {
    return flagLogin;
}


/**
 * @brief BugsTokenInfo 정보로 멤버변수 설정
 * @param tokenInfo
 */
void DataUser_forBugs::set_loginState(const QString p_accessToken, const QString p_refreshToken){
    if(p_accessToken.isEmpty() || p_refreshToken.isEmpty()){
        this->flagLogin = false;
    }
    else{
        this->flagLogin = true;
        this->access_token = p_accessToken;
        this->refresh_token = p_refreshToken;
    }
}



/**
 * @brief DataUser_forBugs::set_logoutState
 */
void DataUser_forBugs::set_logoutState(){
    this->flagLogin = false;
    this->access_token = "";
    this->refresh_token = "";
    this->nickName = "";
    this->product_name = "";
}


bool DataUser_forBugs::dlg_isShow() const{
    return this->dlg_enable;
}

void DataUser_forBugs::dlg_set_state(const bool flag){
    this->dlg_enable = flag;
}

bool DataUser_forBugs::dlg_isSignal() const{
    return this->dlg_signal_check;
}

void DataUser_forBugs::dlg_set_signal(const bool flag){
    this->dlg_signal_check = flag;
}
