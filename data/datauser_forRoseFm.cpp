#include "datauser_forRoseFm.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"
#include "rosefm/rosefm_struct.h"

#include <QWidget>


DataUser_forRoseFM::DataUser_forRoseFM(QObject *parent) : QObject(parent){

    this->list_countryInfo = new QList<roseFM::RosefmCountryInfo>();
    this->list_channelInfo = new QList<roseFM::RosefmChannelInfo>();
}

DataUser_forRoseFM::~DataUser_forRoseFM(){

}

void DataUser_forRoseFM::setCountryInfo(const QJsonObject &jsonObj){
    /*QJsonDocument doc(jsonObj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << strJson;*/

    if(jsonObj.contains("countries")){
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj, "countries");

        for(int i = 0; i < jsonArr_item.count(); i++){
            QJsonObject tmp_json = jsonArr_item.at(i).toObject();

            roseFM::RosefmCountryInfo tmp_data;
            tmp_data.code = ProcJsonEasy::getString(tmp_json, "code");
            tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
            tmp_data.name = ProcJsonEasy::getString(tmp_json, "name");
            tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");

            QJsonObject tmp_fre = ProcJsonEasy::getJsonObject(tmp_json, "frequency");
            tmp_data.fre_max = ProcJsonEasy::getDouble(tmp_fre, "max");
            tmp_data.fre_min = ProcJsonEasy::getDouble(tmp_fre, "min");

            list_countryInfo->append(tmp_data);
        }
    }

    //QList<roseFM::RosefmCountryInfo> testData;
    //getCountryInfo(testData);
}

void DataUser_forRoseFM::setChannelInfo(const QJsonObject &jsonObj){
    /*QJsonDocument doc(jsonObj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << strJson;*/

    if(jsonObj.contains("radioChannels")){
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj, "radioChannels");

        for(int i = 0; i < jsonArr_item.count(); i++){
            QJsonObject tmp_json = jsonArr_item.at(i).toObject();

            roseFM::RosefmChannelInfo tmp_data;
            tmp_data.type = ProcJsonEasy::getString(tmp_json, "channelType");
            tmp_data.fileId = ProcJsonEasy::getInt(tmp_json, "fileId");
            tmp_data.frequency = ProcJsonEasy::getString(tmp_json, "frequency");
            tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
            tmp_data.imgUrl = ProcJsonEasy::getString(tmp_json, "imageUrl");
            tmp_data.key = ProcJsonEasy::getString(tmp_json, "key");
            tmp_data.retryUrl = ProcJsonEasy::getString(tmp_json, "retryUrl");
            tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");
            tmp_data.title = ProcJsonEasy::getString(tmp_json, "title");
            tmp_data.url = ProcJsonEasy::getString(tmp_json, "url");

            list_channelInfo->append(tmp_data);
        }
    }

    //QList<roseFM::RosefmChannelInfo> testData;
    //getChannelInfo(testData);
}

void DataUser_forRoseFM::getCountryInfo(QList<roseFM::RosefmCountryInfo> &countryInfo){

    for(int i = 0; i < list_countryInfo->count(); i++){
        countryInfo.append(list_countryInfo->at(i));
    }
}

void DataUser_forRoseFM::getChannelInfo(QList<roseFM::RosefmChannelInfo> &channelInfo){

    for(int i = 0; i < list_channelInfo->count(); i++){
        channelInfo.append(list_channelInfo->at(i));
    }
}
