#ifndef DATAVIDEO_H
#define DATAVIDEO_H

#include <QObject>
#include <QJsonObject>

class DataVideo : public QObject
{
    Q_OBJECT
public:
    explicit DataVideo(QObject *parent = nullptr);
    ~DataVideo();

    void setData(const QJsonObject &p_json);

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

private:
    QString ext;
    QString id;
    QString ip;
    bool isCloud;
    bool isDir;
    bool isFile;
    bool isServer;
    bool isShare;
    bool isUp;
    QString name;
    QString path;
    QString pw;
    QString rcloneServe;
    QString smbParamPath;
    QString thumbnail;
};

#endif // DATAVIDEO_H
