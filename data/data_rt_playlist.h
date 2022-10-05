#ifndef DATA_RT_PLAYLIST_H
#define DATA_RT_PLAYLIST_H

#include <QObject>
#include <QJsonObject>
#include <QList>


/**
 * @brief 로즈 플레이리스트 데이터 클래스
 * @details API My 플레이리스트 목록의 키값을 변수로 사용
 */
class Data_RT_PlayList : public QObject
{
    Q_OBJECT
public:
    explicit Data_RT_PlayList(QObject *parent = nullptr);
    ~Data_RT_PlayList();

    void setData(const QJsonObject &p_json);
    QJsonObject getData();

    int getNo() const;
    QString getUsername() const;
    QString getTitle() const;
    QString getConverimg() const;
    QString getMemo() const;
    QString getType() const;
    QString getToshare() const;
    int getIsdefault() const;
    int getTotalcount() const;
    QList<QString> getTags() const;    
    QString getFavorites() const;    
    bool getFavorite() const;
    int getStar() const;
    bool getThumbup() const;
    int getThumupCount() const;

    void setStar(int p_cnt);
    void setFavorites(QString p_numValue);

private:
    int no;
    QString username;
    QString title;
    QString converimg;
    QString memo;
    QString type;
    QString toshare;        ///< 0:비공개, 1: 전체공개, 2: 친구공개
    int isdefault = 0;
    int totalcount = 0;
    int favorite = 0;
    QList<QString> tags;

    int star = 0;
    int thumbupCount = 0;
    QString favorites;
    bool isRose = false;
    bool isfavorite = false;
    bool isthumbup = false;

    QJsonObject jsonData;
};

#endif // DATA_RT_PLAYLIST_H
