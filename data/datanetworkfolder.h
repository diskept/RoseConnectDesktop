#ifndef DATANETWORKFOLDER_H
#define DATANETWORKFOLDER_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 네트워크 폴더 정보
 * @details API 키값 그대로 사용 @n
 * 관련 API : IP:PORT/external_usb
 */
class DataNetworkFolder : public QObject
{
    Q_OBJECT
public:
    explicit DataNetworkFolder(QObject *parent = nullptr);

    void setData(const QJsonObject &p_json);
    QJsonObject getData();
    QString getConvertSmbPath() const;

    QString getExt() const;

    QString getId() const;

    QString getIp() const;

    bool getIsCloud() const;

    bool getIsDir() const;

    bool getIsFile() const;

    bool getIsServer() const;

    bool getIsShare() const;

    bool getIsUp() const;

    QString getName() const;

    QString getPath() const;

    QString getPw() const;

    QString getRcloneServe() const;

    QString getSmbParamPath() const;

    QString getThumbnail() const;

    void setName(const QString &value);

    void setPath(const QString &value);

private:
    QString ext;
    QString id;
    QString ip;
    bool isCloud = false;
    bool isDir = true;
    bool isFile = false;
    bool isServer = true;
    bool isShare = false;
    bool isUp = false;
    QString name;
    QString path;
    QString pw;
    QString rcloneServe;
    QString smbParamPath;
    QString thumbnail;
};

#endif // DATANETWORKFOLDER_H
