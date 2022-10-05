#include "data/datauser_forApple.h"
#include <QWidget>


DataUser_forApple::DataUser_forApple(QObject *parent) : QObject(parent)
{
    this->setLogout();
}


bool DataUser_forApple::isLogined() const {
    return this->flagLogin;
}


void DataUser_forApple::setLogin(const QString &userToken, const QString &storefront, const bool flagSavedLoginInfo){

    this->user_token = userToken;
    this->store_front = storefront;
    this->flagSavedLoginInfo = flagSavedLoginInfo;
    this->flagLogin = true;
}


void DataUser_forApple::setLogout(){

    this->user_token = "";
    this->store_front = "";
    this->flagLogin = false;
}
