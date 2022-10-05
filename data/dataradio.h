#ifndef DATARADIO_H
#define DATARADIO_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 라디오 채널 리스트 데이터 클래스
 * @details API 라디오-채널 리스트의 키값을 변수로 사용
 */
class DataRadio : public QObject
{
    Q_OBJECT
public:
    explicit DataRadio(QObject *parent = nullptr);
    ~DataRadio();


    void setData(const QJsonObject &p_json);
    void setDataForFavor(const QJsonObject &p_json);
    QJsonObject getDataJsonForOptionPopup();
    QJsonObject getDataJsonForUpdateFavor();

    // 라디오 국가 전용
    QString getRadio_key() const;
    QString getRadio_cate() const;
    QString getRadio_type() const;

    // 라디오 즐겨찾기 전용
    QString getId() const;
    QString getCountry() const;
    int getFavorite() const;
    bool getIsSelect() const;

    // 공통
    QString getRadio_nm() const;
    QString getRadio_url() const;
    QString getRadio_thumbnail_url() const;

    void setFavorite(const int &p_favorite);

private:

    // 라디오 국가 전용
    QString radio_key;
    QString radio_cate;
    QString radio_type;

    // 라디오 즐겨찾기 전용
    QString _id;

    int favorite=0;
    bool isSelect=false;

    // 라디오 국가/즐겨찾기 곹오
    QString country;
    QString radio_nm;
    QString radio_url;
    QString radio_thumbnail_url;


};

#endif // DATARADIO_H
