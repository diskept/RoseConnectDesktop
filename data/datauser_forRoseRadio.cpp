#include "data/datauser_forRoseRadio.h"


#include "common/networkhttp.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"


DataUser_forRoseRadio::DataUser_forRoseRadio(QObject *parent) : QObject(parent){

    this->list_genreInfo = new QList<roseRadio::RoseRadioGenreInfo>();
    this->list_regionInfo = new QList<roseRadio::RoseRadioRegionInfo>();
    this->list_languageInfo = new QList<roseRadio::RoseRadioLanguageInfo>();
}


DataUser_forRoseRadio::~DataUser_forRoseRadio(){

    this->deleteLater();
}


void DataUser_forRoseRadio::setGenreInfo(const QJsonArray &jsonArr){

    for(int i = 0; i < jsonArr.count(); i++){
        QJsonObject tmp_json = jsonArr.at(i).toObject();

        roseRadio::RoseRadioGenreInfo tmp_data;
        tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
        tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");

        tmp_data.name = ProcJsonEasy::getString(tmp_json, "name");
        tmp_data.imageUrl = ProcJsonEasy::getString(tmp_json, "imageUrl");

        this->list_genreInfo->append(tmp_data);
    }
}


void DataUser_forRoseRadio::setRegionInfo(const QJsonArray &jsonArr){

    for(int i = 0; i < jsonArr.count(); i++){
        QJsonObject tmp_json = jsonArr.at(i).toObject();

        roseRadio::RoseRadioRegionInfo tmp_data;
        tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
        tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");

        tmp_data.name = ProcJsonEasy::getString(tmp_json, "name");

        this->list_regionInfo->append(tmp_data);
    }
}


void DataUser_forRoseRadio::setLanguageInfo(const QJsonArray &jsonArr){

    for(int i = 0; i < jsonArr.count(); i++){
        QJsonObject tmp_json = jsonArr.at(i).toObject();

        roseRadio::RoseRadioLanguageInfo tmp_data;
        tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
        tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");

        tmp_data.name = ProcJsonEasy::getString(tmp_json, "name");

        this->list_languageInfo->append(tmp_data);
    }
}


QList<roseRadio::RoseRadioGenreInfo>* DataUser_forRoseRadio::getGenreInfo(){

    return this->list_genreInfo;
}


QList<roseRadio::RoseRadioRegionInfo>* DataUser_forRoseRadio::getRegionInfo(){

    return this->list_regionInfo;
}


QList<roseRadio::RoseRadioLanguageInfo>* DataUser_forRoseRadio::getLanguageInfo(){

    return this->list_languageInfo;
}
