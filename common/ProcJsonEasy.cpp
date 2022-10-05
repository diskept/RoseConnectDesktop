#include "ProcJsonEasy.h"

#include <QVariant>


/**
 * @brief ProcJsonEasy::get_flagOk
 * @param jsonObj
 * @return
 */
bool ProcJsonEasy::get_flagOk(const QJsonObject &jsonObj){
    return (jsonObj.contains("flagOk") && jsonObj["flagOk"].toBool()) ? true : false;
}


/**
 * @brief ProcJsonEasy::getJsonArray
 * @param jsonObj
 * @param key
 * @return
 */
QJsonArray ProcJsonEasy::getJsonArray(const QJsonObject &jsonObj, const QString key){
    if(jsonObj.contains(key)){
        return jsonObj[key].toArray();
    }
    else{
        return QJsonArray();
    }
}

/**
 * @brief ProcJsonEasy::getJsonObject
 * @param jsonObj
 * @param key
 * @return
 */
QJsonObject ProcJsonEasy::getJsonObject(const QJsonObject &jsonObj, const QString key){
    if(jsonObj.contains(key)){
        return jsonObj[key].toObject();
    }
    else{
        return QJsonObject();
    }
}


/**
 * @brief ProcJsonEasy::getString
 * @param jsonObj
 * @param key
 * @param defValue
 * @return
 */
QString ProcJsonEasy::getString(const QJsonObject &jsonObj, const QString key, const QString defValue){
   if(jsonObj.contains(key)) {
       return jsonObj[key].toString(defValue);
   }
   else {
       return defValue;
   }
}

/**
 * @brief ProcJsonEasy::getInt
 * @param jsonObj
 * @param key
 * @param defValue
 * @return
 */
int ProcJsonEasy::getInt(const QJsonObject &jsonObj, const QString key, const int defValue){
   if(jsonObj.contains(key)) {
       return jsonObj[key].toInt(defValue);
   }
   else {
       return defValue;
   }
}

/**
 * @brief ProcJsonEasy::getInt
 * @param jsonObj
 * @param key
 * @param defValue
 * @return
 */
double ProcJsonEasy::getDouble(const QJsonObject &jsonObj, const QString key, const double defValue){
   if(jsonObj.contains(key)) {
       return jsonObj[key].toDouble(defValue);
   }
   else {
       return defValue;
   }
}



/**
 * @brief ProcJsonEasy::getBool
 * @param jsonObj
 * @param key
 * @param defValue
 * @return
 */
bool ProcJsonEasy::getBool(const QJsonObject &jsonObj, const QString key, const bool defValue){
   if(jsonObj.contains(key)) {
       return jsonObj[key].toBool(defValue);
   }
   else {
       return defValue;
   }
}


/**
 * @brief QVariant 로 반환한다.
 * @param jsonObj
 * @param key
 * @return
 */
QVariant ProcJsonEasy::getVariant(const QJsonObject &jsonObj, const QString key){
   if(jsonObj.contains(key)) {
       return jsonObj[key].toVariant();
   }
   else {
       return QVariant();
   }
}



/**
 * @brief JsonArray 내에 JsonObject 여러개로 구성되었을 때, JsonObject의 key 정보들만 추출하여 QList<QString> 으로 반환한다.
 * @param jsonArr
 * @param key
 * @return
 */
QList<QString> ProcJsonEasy::getListString_fromJsonArr(const QJsonArray& jsonArr, const QString key){\

    QList<QString> list_output;

    for(int i=0 ; i<jsonArr.count() ; i++){
        QJsonObject tmp_jsonObj = jsonArr.at(i).toObject();
        list_output.append(ProcJsonEasy::getString(tmp_jsonObj, key));
    }

    return list_output;

}


/**
 * @brief JsonArray 내에 JsonObject 여러개로 구성되었을 때, JsonObject의 key 정보들만 추출하여 QList<int>으로 반환한다.
 * @param jsonArr
 * @param key
 * @return
 */
QList<int> ProcJsonEasy::getListInt_fromJsonArr(const QJsonArray& jsonArr, const QString key){

    QList<int> list_output;

    for(int i=0 ; i<jsonArr.count() ; i++){
        QJsonObject tmp_jsonObj = jsonArr.at(i).toObject();
        list_output.append(ProcJsonEasy::getInt(tmp_jsonObj, key));
    }

    return list_output;

}


void ProcJsonEasy::clearJsonArray(QJsonArray& jsonArr){

    QJsonArray::iterator i;
    for (i = jsonArr.begin(); i != jsonArr.end(); ++i){
        jsonArr.erase(i);
    }

}


/**
 * @brief ProcJsonEasy::mergeJsonArray
 * @param jsonArr_1
 * @param jsonArr_2
 */
void ProcJsonEasy::mergeJsonArray(QJsonArray& jsonArr_org, const QJsonArray& jsonArr_target){

//    QJsonArray jsonArr_output = jsonArr_org;
    for(int i=0 ; i<jsonArr_target.size() ; i++){
        jsonArr_org.append(jsonArr_target.at(i));
    }

}


