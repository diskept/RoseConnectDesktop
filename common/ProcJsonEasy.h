#ifndef PROCJSONEASY_H
#define PROCJSONEASY_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

/**
 * @brief 공통적으로 체크하고 반환하는 부분에 대해서, 사용 편리를 위해서 만든 클래스.@n
 *      JsonObject에서 데이터를 가져올 때, contains 체크 및 default value 세팅 가능. @n
 *      static 함수들로만 구성.
 */
class ProcJsonEasy : QObject
{
public:
    static bool get_flagOk(const QJsonObject &jsonObj);
    static QJsonArray getJsonArray(const QJsonObject &jsonObj, const QString key);
    static QJsonObject getJsonObject(const QJsonObject &jsonObj, const QString key);
    static QString getString(const QJsonObject &jsonObj, const QString key, const QString defValue="");
    static int getInt(const QJsonObject &jsonObj, const QString key, const int defValue=0);
    static double getDouble(const QJsonObject &jsonObj, const QString key, const double defValue=0);
    static bool getBool(const QJsonObject &jsonObj, const QString key, const bool defValue=false);
    static QVariant getVariant(const QJsonObject &jsonObj, const QString key);

    static QList<QString> getListString_fromJsonArr(const QJsonArray& jsonArr, const QString key);
    static QList<int> getListInt_fromJsonArr(const QJsonArray& jsonArr, const QString key);

    static void clearJsonArray(QJsonArray& jsonArr);
    static void mergeJsonArray(QJsonArray& jsonArr_1, const QJsonArray& jsonArr_2);

};

#endif // PROCJSONEASY_H
