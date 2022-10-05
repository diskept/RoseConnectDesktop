#include "data_rt_playlist.h"

#include <QJsonArray>

/**
 * @brief Data_RT_PlayList::Data_RT_PlayList : 로즈 플레이리스트 데이터 클래스
 * @details API My 플레이리스트 목록의 키값을 변수로 사용
 * @param parent
 */
Data_RT_PlayList::Data_RT_PlayList(QObject *parent)
    : QObject(parent)
{

}

Data_RT_PlayList::~Data_RT_PlayList(){
    tags.clear();
}

void Data_RT_PlayList::setData(const QJsonObject &p_json){

    jsonData = p_json;

    const QString key_no = "no";
    const QString key_id = "id";
    const QString key_username = "username";
    const QString key_ownerName = "ownerName";
    const QString key_title = "title";
    const QString key_coverimg = "coverimg";
    const QString key_thumbnail = "thumbnail";
    const QString key_memo = "memo";
    const QString key_comment = "comment";
    const QString key_type = "type";
    const QString key_toshare = "toshare";
    const QString key_isdefault = "isdefault";
    const QString key_totalcount = "totalcount";
    const QString key_trackCount = "trackCount";
    const QString key_tags = "tags";
    const QString key_star = "star";
    const QString key_favorites = "favorites";
    const QString key_favorite = "favorite";
    const QString key_isRose = "isRose";
    const QString key_thumbup = "thumbup";
    const QString key_thumbupCount = "thumbupCount";

    if(p_json.contains(key_no)){
        no = p_json[key_no].toInt(0);
    }
    else if(p_json.contains(key_id)){
        no = p_json[key_id].toInt();
    }
    if(p_json.contains(key_username)){
        username = p_json[key_username].toString("");
    }
    else if(p_json.contains(key_ownerName)){
        username = p_json[key_ownerName].toString("");
    }
    if(p_json.contains(key_title)){
        title = p_json[key_title].toString("");
    }
    if(p_json.contains(key_coverimg)){
        converimg = p_json[key_coverimg].toString("");
    }
    else if(p_json.contains(key_thumbnail)){
        converimg = p_json[key_thumbnail].toString("");
    }
    if(p_json.contains(key_memo)){
        memo = p_json[key_memo].toString("");
    }
    else if(p_json.contains(key_comment)){
        memo = p_json[key_comment].toString("");
    }
    if(p_json.contains(key_type)){
        type = p_json[key_type].toString("");
    }
    if(p_json.contains(key_toshare)){
        // 0:비공개, 1: 전체공개, 2: 친구공개
        toshare = p_json[key_toshare].toString("");
    }
    if(p_json.contains(key_isdefault)){
        isdefault = p_json[key_isdefault].toInt(0);
    }
    if(p_json.contains(key_totalcount)){
        totalcount = p_json[key_totalcount].toInt(0);
    }
    else if(p_json.contains(key_trackCount)){
        totalcount = p_json[key_trackCount].toInt(0);
    }
    if(p_json.contains(key_tags)){
        QJsonArray jsonArrTags = p_json[key_tags].toArray();

        tags.clear();
        for(int i = 0 ; i < jsonArrTags.size(); i++){
            tags.append(jsonArrTags.at(i).toString());
        }
    }
    if(p_json.contains(key_star)){
        star = p_json[key_star].toInt(0);
    }else{
        star = 0;
    }
    if(p_json.contains(key_isRose)){
        isRose = p_json[key_isRose].toBool();
    }
    if(p_json.contains(key_thumbup)){
        isthumbup = p_json[key_thumbup].toBool();
    }
    if(p_json.contains(key_thumbupCount)){
        thumbupCount = p_json[key_thumbupCount].toInt();
    }

    // 즐겨찾기(하트) "0" or "1"
    if(p_json.contains(key_favorites)){
        favorites = p_json[key_favorites].toString();
    }
    else if(p_json.contains(key_favorite)){
        favorite = p_json[key_favorite].toBool();
    }
    else{
        favorites = "0";
    }

}

QJsonObject Data_RT_PlayList::getData(){
    return jsonData;
}


int Data_RT_PlayList::getNo() const
{
    return no;
}

QString Data_RT_PlayList::getUsername() const
{
    return username;
}

QString Data_RT_PlayList::getTitle() const
{
    return title;
}

QString Data_RT_PlayList::getConverimg() const
{
    return converimg;
}

QString Data_RT_PlayList::getMemo() const
{
    return memo;
}

QString Data_RT_PlayList::getType() const
{
    return type;
}

/**
 * @brief Data_RT_PlayList::getToshare 공개모드 (0:비공개, 1: 전체공개, 2: 친구공개)
 * @return
 */
QString Data_RT_PlayList::getToshare() const
{
    return toshare;
}

int Data_RT_PlayList::getIsdefault() const
{
    return isdefault;
}

int Data_RT_PlayList::getTotalcount() const
{
    return totalcount;
}

QList<QString> Data_RT_PlayList::getTags() const
{
    return tags;
}

void Data_RT_PlayList::setStar(int p_cnt){
    star = p_cnt;
}

QString Data_RT_PlayList::getFavorites() const
{
    return favorites;
}

void Data_RT_PlayList::setFavorites(QString p_numValue){
    favorites = p_numValue;
}

bool Data_RT_PlayList::getFavorite() const
{
    return isfavorite;
}

int Data_RT_PlayList::getStar() const
{
    return star;
}

bool Data_RT_PlayList::getThumbup() const
{
    return isthumbup;
}

int Data_RT_PlayList::getThumupCount() const
{
    return thumbupCount;
}
