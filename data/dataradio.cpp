#include "dataradio.h"

/**
 * @brief DataRadio::DataRadio : 라디오 채널 리스트 데이터 클래스
 * @details API 라디오-채널 리스트의 키값을 변수로 사용
 * @param parent QObject
 */
DataRadio::DataRadio(QObject *parent)
    : QObject(parent)
{

}

DataRadio::~DataRadio(){

}

void DataRadio::setData(const QJsonObject &p_json){

    const QString key_radio_key = "radio_key";
    const QString key_radio_nm = "radio_nm";
    const QString key_radio_url = "radio_url";
    const QString key_radio_thumbnail_url = "radio_thumbnail_url";
    const QString key_radio_cate = "radio_cate";
    const QString key_radio_type = "radio_type";
    const QString key_country = "country";

    if(p_json.contains(key_radio_key)){
        radio_key = p_json[key_radio_key].toString("");
    }
    if(p_json.contains(key_radio_nm)){
        radio_nm = p_json[key_radio_nm].toString("");
    }
    if(p_json.contains(key_radio_url)){
        radio_url = p_json[key_radio_url].toString("");
    }
    if(p_json.contains(key_radio_thumbnail_url)){
        radio_thumbnail_url = p_json[key_radio_thumbnail_url].toString("");
    }
    if(p_json.contains(key_radio_cate)){
        radio_cate = p_json[key_radio_cate].toString("");
    }
    if(p_json.contains(key_radio_type)){
        radio_type = p_json[key_radio_type].toString("");
    }
    if(p_json.contains(key_country)){
        country = p_json[key_country].toString("");
    }
}

void DataRadio::setDataForFavor(const QJsonObject &p_json){

    const QString key_id = "_id";
    const QString key_country = "country";
    const QString key_favorite = "favorite";
    const QString key_isSelect = "isSelect";
    const QString key_radio_nm = "radio_nm";
    const QString key_radio_thumbnail_url = "radio_thumbnail_url";
    const QString key_radio_url = "radio_url";

    if(p_json.contains(key_id)){
        _id = p_json[key_id].toString();
    }
    if(p_json.contains(key_country)){
        country = p_json[key_country].toString("");
    }
    if(p_json.contains(key_favorite)){
        favorite = p_json[key_favorite].toInt(0);
    }
    if(p_json.contains(key_isSelect)){
        isSelect = p_json[key_isSelect].toBool(false);
    }
    if(p_json.contains(key_radio_nm)){
        radio_nm = p_json[key_radio_nm].toString("");
    }
    if(p_json.contains(key_radio_thumbnail_url)){
        radio_thumbnail_url = p_json[key_radio_thumbnail_url].toString("");
    }
    if(p_json.contains(key_radio_url)){
        radio_url = p_json[key_radio_url].toString("");
    }

}
/**
 * @brief DataRadio::getDataJsonForOptionPopup
 * @return
 */
QJsonObject DataRadio::getDataJsonForOptionPopup(){

    QJsonObject tmp_dataObject;
    tmp_dataObject.insert("_id", this->getId());
    tmp_dataObject.insert("country", this->getCountry());
    tmp_dataObject.insert("favorite", this->getFavorite());
    tmp_dataObject.insert("isSelect", this->getIsSelect());
    tmp_dataObject.insert("radio_nm", this->getRadio_nm());
    tmp_dataObject.insert("radio_thumbnail_url", this->getRadio_thumbnail_url());
    tmp_dataObject.insert("radio_url", this->getRadio_url());
    return  tmp_dataObject;
}

/**
 * @brief DataRadio::getDataJsonForUpdateFavor : 좋아요 업데이트 용 Json
 * @details API 문서에는 라디오(Favor) 모든 키 값 전달하라고 되어 있으나,
 * favorite, radio_nm, radio_url만 있으면 가능
 * @return
 */
QJsonObject DataRadio::getDataJsonForUpdateFavor(){
    QJsonObject tmp_dataObject;
    tmp_dataObject.insert("favorite", favorite>0 ? 0 : 1);
    tmp_dataObject.insert("radio_nm", radio_nm);
    tmp_dataObject.insert("radio_url", radio_url);
    return tmp_dataObject;
}

QString DataRadio::getRadio_key() const
{
    return radio_key;
}

QString DataRadio::getRadio_nm() const
{
    return radio_nm;
}

QString DataRadio::getRadio_url() const
{
    return radio_url;
}

QString DataRadio::getRadio_thumbnail_url() const
{
    return radio_thumbnail_url;
}

QString DataRadio::getRadio_cate() const
{
    return radio_cate;
}

QString DataRadio::getRadio_type() const
{
    return radio_type;
}

QString DataRadio::getId() const
{
    return _id;
}

QString DataRadio::getCountry() const
{
    return country;
}

int DataRadio::getFavorite() const
{
    return favorite;
}

bool DataRadio::getIsSelect() const
{
    return isSelect;
}

void DataRadio::setFavorite(const int &p_favorite){
    favorite = p_favorite;
}
