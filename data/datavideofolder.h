#ifndef DATAVIDEOFOLDER_H
#define DATAVIDEOFOLDER_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief Video 데이터 클래스
 * @details API 비디오-폴더-USB의 키값을 변수로 사용
 */
class DataVideoFolder : public QObject
{
    Q_OBJECT
public:
    explicit DataVideoFolder(QObject *parent = nullptr);
    ~DataVideoFolder();

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

#endif // DATAVIDEOFOLDER_H
