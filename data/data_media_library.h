#ifndef DATA_MEDIA_LIBRARY_H
#define DATA_MEDIA_LIBRARY_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 미디어 라이브러리 데이터 클래스 @n
 */
class Data_Media_Library : public QObject
{
    Q_OBJECT
public:
    explicit Data_Media_Library(QObject *parent = nullptr);
    explicit Data_Media_Library(const QJsonObject &p_json, QObject *parent = nullptr);

    void setData(const QJsonObject &p_json);
    QString getStrDbStatus() const;

    QString getDate_finished() const;
    int getDbStatus() const;
    int getNumalbums() const;
    int getNumartists() const;
    int getSongCnt() const;
    int getVideoCnt() const;
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

    QJsonObject getJsonData() const;

private:
    QJsonObject jsonData;

    QString date_finished;
    int dbStatus;
    int numalbums=0;
    int numartists=0;
    int songCnt=0;
    int videoCnt=0;
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

#endif // DATA_MEDIA_LIBRARY_H
