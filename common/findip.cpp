#include "findip.h"

/**
 * @brief The FindIP class : 내 아이피 확인 @n
 * 2개의 함수로 구성 내 아이피 확인 @sa FindIP::getMyIP(), 내 맥주소 확인 @sa FindIP::getMacAddress()
 */
FindIP::FindIP(){
}
FindIP::~FindIP(){
}

/**
 * @brief FindIP::getMyIP : IP 주소 반환
 * @return
 */
QString FindIP::getMyIP(){

    QString ip="";

    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();

    for(int i = 0; i < ipList.size(); i++){
        if(ipList.at(i) != QHostAddress::LocalHost && ipList.at(i).toIPv4Address()){
            ip = ipList.at(i).toString();
            break;
        }
    }

    return ip;
}

/**
 * @brief FindIP::getMacAddress : Mac주소 반환
 * @param ip
 * @return
 */
QString FindIP::getMacAddress(QString ip){

    QString macAddress;

    QList<QNetworkInterface> macList = QNetworkInterface::allInterfaces();

    for(int i =0; i < macList.size(); i++){

        QList<QNetworkAddressEntry> allEntries = macList.at(i).addressEntries();
        foreach (auto entry, allEntries){

            if(entry.ip().toString()==ip){
                macAddress = macList.at(i).hardwareAddress();
                break;
            }

        }

        if(!macAddress.isNull()){
            break;
        }
    }

    return macAddress;
}
