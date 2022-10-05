#ifndef DATADEVICE_H
#define DATADEVICE_H

#include <QObject>
#include <QJsonObject>
#include <QVariantList>

/**
 * @brief 로즈 기기 데이터 클래스
 * @details API 장치연결의 키값을 변수로 사용
 */
class DataDevice : public QObject
{
    Q_OBJECT

public:
    explicit DataDevice(QObject *parent = nullptr);
    ~DataDevice();

    void setData(const QJsonObject &p_json);
    void setData(const QVariantList &p_data);


    long getDbFileSize() const;

    QString getDeviceID() const;    ///< mac address 반환
    QString getDeviceIP() const;
    QString getDeviceName() const;
    QString getDeviceRoseToken() const;
    QString getDeviceType() const;
    QString getDeviceVersion() const;
    bool getIsDbScanning() const;
    int getMusicPlayType() const;

    void setDeviceType(const QString &value);
    //void setcheckSum(const QString &value);//cheon210916-total
    void setIsDbScanning(const bool &flag);
    void setMusicPlayType(const int &playType);

    QString CurrPlayType = "NONE";
    QString CurrExtType = "NONE";//c220903_1
    QString titleName;

private:
    long dbFileSize;

    QString deviceID;   ///< mac address
    QString deviceIP;
    QString deviceName;
    QString deviceRoseToken;
    QString checkSum;//cheon210916-total
    QString deviceType;
    QString deviceVersion;
    bool isDbScanning;
    int musicPlayType;

};

#endif // DATADEVICE_H
