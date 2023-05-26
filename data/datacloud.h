#ifndef DATACLOUD_H
#define DATACLOUD_H

#include <QObject>
#include <QJsonObject>


/**
 * @brief 클라우드 정보 데이터 클래스
 * @details API 키값 그대로 사용 @n
 *      관련 API : IP:PORT/get_rclone_info
 */
class DataCloud : public QObject
{
    Q_OBJECT
public:
    explicit DataCloud(QObject *parent = nullptr);

    void setData(const QJsonObject &p_json);
    QJsonObject getData() const{
        return json;
    };
    bool getIsAlias() const;
    bool getIsCache() const;
    bool getIsCrypt() const;
    bool getIsDrawerPinned() const;
    bool getIsPinned() const;
    QString getName() const;
    int getType() const;
    QString getTypeReadable() const;

    QString getPath() const;
    bool getisisInstall() const;
    QString getMimeType() const;

private:
    QJsonObject json;
    bool isAlias;
    bool isCache;
    bool isCrypt;
    bool isDrawerPinned;
    bool isPinned;
    QString name;
    int type;
    QString typeReadable;

    QString path;
    bool isInstall;
    QString mimeType;
};

#endif // DATACLOUD_H
