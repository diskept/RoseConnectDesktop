#include "datauser.h"
#include <QJsonDocument>
#include <QJsonObject>


QString key_email = "email";                ///< 사용자 이메일
QString key_access_token = "access_token";     ///< 로그인 토큰
QString key_token_type = "token_type";      ///< 로그인 토큰 타입
QString key_username = "username";          ///< 사용자명

QString key_access_token_2 = "accesskey";      ///< 로즈기기 사용자 정보 반환 KEY
QString key_username_2 = "userName";              ///< 로즈기기 사용자 정보 반환 KEY

QString key_access_token_api = "accessToken";
QString key_token_type_api = "tokenType";
QString key_refresh_token_api = "refreshToken";

/**
 * @brief 사용자 정보 데이터 클래스
 * @param parent
 */
DataUser::DataUser(QObject *parent) : QObject(parent)
{

}

DataUser::~DataUser(){

}

void DataUser::setDataUser(const QJsonObject &p_json){

    //QJsonDocument doc(p_json);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE USER DATA RCV : " << strJson;

    // 초기화
    this->email = "";
    this->username = "";

    this->access_token = "";    
    this->token_type = "Bearer";

    if(ProcJsonEasy::get_flagOk(p_json) == true){
        this->email = ProcJsonEasy::getString(p_json, key_email);

        if(p_json.contains(key_access_token_2)){
            this->access_token = ProcJsonEasy::getString(p_json, key_access_token_2);
        }
        else if(p_json.contains(key_access_token)){
            this->access_token = ProcJsonEasy::getString(p_json, key_access_token);
        }
        else if(p_json.contains(key_access_token_api)){
            this->access_token = ProcJsonEasy::getString(p_json, key_access_token_api);
        }

        if(p_json.contains(key_refresh_token_api)){
            this->refresh_token = ProcJsonEasy::getString(p_json, key_refresh_token_api);
        }

        if(p_json.contains(key_username_2)){
            this->username = ProcJsonEasy::getString(p_json, key_username_2);
        }
        else if(p_json.contains(key_username)){
            this->username = ProcJsonEasy::getString(p_json, key_username);
        }

        if(p_json.contains(key_token_type)){
            this->token_type = ProcJsonEasy::getString(p_json, key_token_type);
        }
        else if(p_json.contains(key_token_type_api)){
            this->token_type = ProcJsonEasy::getString(p_json, key_token_type_api);
        }

        if(!this->access_token.isEmpty()){
            QStringList splitToken = this->access_token.split(".");
            if(splitToken.size() > 1){
                QByteArray tokenBase64 = QByteArray::fromBase64(splitToken.at(1).toUtf8());
                QJsonDocument doc = QJsonDocument::fromJson(tokenBase64);
                QJsonObject json = doc.object();

                if(json.empty()){
                    QString tmpData = QByteArray::fromBase64(splitToken.at(1).toUtf8());
                    tmpData.replace("\\", "");
                    tmpData.replace("\"", "");
                    QStringList splitToken = tmpData.split(",");

                    if(splitToken.at(0).contains("sub")){
                        QString tmpsub = splitToken.at(0);
                        QStringList subToken = tmpsub.split(":");
                        if(subToken.count() > 1){
                            this->sub = subToken.at(1);
                        }
                    }
                    if(splitToken.at(1).contains("iat")){
                        QString tmpiat = splitToken.at(0);
                        QStringList iatToken = tmpiat.split(":");
                        if(iatToken.count() > 1){
                            this->iat = iatToken.at(1).toInt();
                        }
                    }
                    if(splitToken.at(2).contains("exp")){
                        QString tmpexp = splitToken.at(0);
                        QStringList expToken = tmpexp.split(":");
                        if(expToken.count() > 1){
                            this->iat = expToken.at(1).toInt();
                        }
                    }
                }
                else{
                    if(json.contains("sub")){
                        this->sub = json["sub"].toString("");
                    }
                    if(json.contains("iat")){
                        this->iat = json["iat"].toInt(0);
                    }
                    if(json.contains("exp")){
                        this->exp = json["exp"].toInt(0);
                    }
                    if(json.contains("friend")){
                        this->userFriend = json["friend"].toString("");
                    }
                    if(json.contains("rosetubeRecentPlaylist")){
                        this->rosetubeRecentPlaylist = json["rosetubeRecentPlaylist"].toString("");
                    }
                }
            }
        }
    }

    /*this->exp = 0;
    this->userFriend = "";
    this->iat = 0;
    this->rosetubeRecentPlaylist = "";
    this->sub = "";

    if(p_json.contains(key_email)){
        email = p_json[key_email].toString("");
    }

    if(p_json.contains(key_access_token) || p_json.contains(key_access_token_2)){

        access_token = p_json[key_access_token].toString(p_json[key_access_token_2].toString());

        QStringList splitToken = access_token.split(".");
        if(splitToken.size() > 1){
            QByteArray tokenBase64 = QByteArray::fromBase64(splitToken.at(1).toUtf8());
            QJsonDocument doc = QJsonDocument::fromJson(tokenBase64);
            QJsonObject json = doc.object();

            if(json.contains("exp")){
                exp = json["exp"].toInt(0);
            }
            if(json.contains("friend")){
                userFriend = json["friend"].toString("");
            }
            if(json.contains("iat")){
                iat = json["iat"].toInt(0);
            }
            if(json.contains("rosetubeRecentPlaylist")){
                rosetubeRecentPlaylist = json["rosetubeRecentPlaylist"].toString("");
            }
            if(json.contains("sub")){
                sub = json["sub"].toString("");
            }
        }
    }
    if(p_json.contains(key_token_type)){
        token_type = p_json[key_token_type].toString("");
    }


    if(p_json.contains(key_username)){
        username = p_json[key_username].toString("");
    }
    else if(p_json.contains(key_username_2)){
        username = p_json[key_username_2].toString();
    }*/
}

void DataUser::setUserInfo(const QJsonObject &p_json){

    //QJsonDocument doc(p_json);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE USER INFO RCV : " << strJson;

    if(ProcJsonEasy::get_flagOk(p_json) == true){
        this->bio = ProcJsonEasy::getString(p_json, "bio");
        this->no = ProcJsonEasy::getInt(p_json, "no");
        this->rank = ProcJsonEasy::getInt(p_json, "rank");
        this->privacy = ProcJsonEasy::getInt(p_json, "privacy");
    }
}


void DataUser::setShazamInfo(const QJsonObject &p_json){

    if(ProcJsonEasy::getString(p_json, "code") == "200"){

        QJsonObject externalToken = ProcJsonEasy::getJsonObject(p_json, "externalToken");

        this->access_token_shazam = ProcJsonEasy::getString(externalToken, "token");
        this->expired_shazam = ProcJsonEasy::getString(externalToken, "expired");
    }
}


QString DataUser::getEmail() const
{
    return email;
}

QString DataUser::getAccess_token() const
{
    return access_token;
}

QString DataUser::getRefresh_token() const
{
    return refresh_token;
}

QString DataUser::getToken_type() const
{
    return token_type;
}

QString DataUser::getUsername() const
{
    return username;
}

QString DataUser::getUserBIO() const
{
    return bio;
}

int DataUser::getUserNo() const
{
    return no;
}

int DataUser::getUserPrivacy() const
{
    return privacy;
}

int DataUser::getUserRank() const
{
    return rank;
}

int DataUser::getExp() const
{
    return exp;
}

int DataUser::getIat() const
{
    return iat;
}

QString DataUser::getUserFriend() const
{
    return userFriend;
}

QString DataUser::getRosetubeRecentPlaylist() const
{
    return rosetubeRecentPlaylist;
}

QString DataUser::getSub() const
{
    return sub;
}


void DataUser::setDeviceChange(bool flag){
    this->device_change = flag;
}

bool DataUser::isChanged(){

    return this->device_change;
}

QString DataUser::getShazam_token() const
{
    return this->access_token_shazam;
}

/**
 * @brief DataUser::isValid : 사용자 정보 유효성 검사
 * @return if true, 유효
 * @note 로즈기기에서 사용자 정보 반환 시, username 누락되는 경우 확인
 */
bool DataUser::isValid(){
    if(access_token.isEmpty() || email.isEmpty() || username.isEmpty()){
        return false;
    }

    return true;
}
