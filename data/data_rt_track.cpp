#include "data_rt_track.h"
#include <QDebug>

/**
 * @brief Data_RT_Track::Data_RT_Track : 로즈 트랙 데이터 클래스
 * @details API 트랙 키값을 변수로 사용.
 * @param parent
 */
Data_RT_Track::Data_RT_Track(QObject *parent)
    :QObject(parent)
{

}

Data_RT_Track::~Data_RT_Track(){

}

void Data_RT_Track::setDataFromCrawling(const QJsonObject &p_json){

    const QString key_days = "days";
    const QString key_hit = "hit";
    const QString key_img = "img";
    const QString key_time = "time";
    const QString key_title = "title";
    const QString key_url = "url";
    const QString key_channelId = "channelId";
    const QString key_channelTitle = "channelTitle";

    if(p_json.contains(key_title)){
        title = p_json[key_title].toString();
    }
    if(p_json.contains(key_hit)){
        viewCount = p_json[key_hit].toString();
    }
    if(p_json.contains(key_img)){
        thumbnail = p_json[key_img].toString();
    }
    if(p_json.contains(key_time)){
        duration = p_json[key_time].toString();
    }
    if(p_json.contains(key_url)){
        QString tmp_url = p_json[key_url].toString();
        playurl = tmp_url.replace("https://youtube", "https://www.youtube");
    }
    if(p_json.contains(key_channelId)){
         channelId = p_json[key_channelId].toString();
    }
    if(p_json.contains(key_channelTitle)){
        channelTitle = p_json[key_channelTitle].toString();
    }
}

void Data_RT_Track::setData(const QJsonObject &p_json){

    const QString key_no = "no";
    const QString key_id = "id";
    const QString key_playlistno = "playlistno";
    const QString key_title = "title";
    const QString key_playurl = "playurl";
    const QString key_playUrl = "playUrl";
    const QString key_favorites = "favorites";
    const QString key_favorite = "favorite";
    const QString key_clientKey = "clientKey";
    const QString key_comment = "comment";

    const QString key_snippet = "snippet";
    const QString key_channelId = "channelId";
    const QString key_thumbnails = "thumbnails";
    const QString key_medium = "medium";
    const QString key_high = "high";
    const QString key_default = "default";
    const QString key_url = "url";
    const QString key_channelTitle = "channelTitle";
    const QString key_categoryId = "categoryId";
    const QString key_description = "description";

    const QString key_contentDetails = "contentDetails";
    const QString key_duration = "duration";

    const QString key_detailContents = "detailContents";
    const QString key_definition = "definition";
    const QString key_dimension = "dimension";

    const QString key_statistic = "statistic";
    const QString key_viewCount = "viewCount";
    const QString key_star = "star";

    const QString key_trackStatistic = "trackStatistic";
    const QString key_likeCount = "likeCount";

    if(p_json.contains(key_star)){
        star = p_json[key_star].toInt();
    }
    if(p_json.contains(key_no)){
        no = p_json[key_no].toInt();
    }
    else if(p_json.contains(key_id)){
        no = p_json[key_id].toInt();
    }
    if(p_json.contains(key_playlistno)){
        playlistno = p_json[key_playlistno].toInt();
    }
    if(p_json.contains(key_title)){
        title = p_json[key_title].toString();
    }
    if(p_json.contains(key_clientKey)){
        clientKey = p_json[key_clientKey].toString();
    }
    if(p_json.contains(key_playurl)){
        playurl = p_json[key_playurl].toString();
    }
    if(p_json.contains(key_playUrl)){
        playurl = p_json[key_playUrl].toString();

        if(playurl.isEmpty() && !clientKey.isEmpty()){
            playurl = "https://www.youtube.com/watch?v=" + clientKey;
        }
    }
    if(p_json.contains(key_favorites)){
        favorites = p_json[key_favorites].toString();
    }
    else if(p_json.contains(key_favorite)){
        favorites = p_json[key_favorite].toString();
    }    
    if(p_json.contains(key_comment)){
        comment = p_json[key_comment].toString();
    }

    if(p_json.contains("data")){
        QJsonObject jsonData = p_json["data"].toObject();

        if(jsonData.contains(key_snippet)){
            QJsonObject jsonSnippet = jsonData[key_snippet].toObject();

            if(jsonSnippet.contains(key_categoryId)){
                categoryId = jsonSnippet[key_categoryId].toString();
            }
            if(jsonSnippet.contains(key_channelId)){
                channelId = jsonSnippet[key_channelId].toString();
            }
            if(jsonSnippet.contains(key_channelTitle)){
                channelTitle = jsonSnippet[key_channelTitle].toString();
            }
            if(jsonSnippet.contains(key_description)){
                description = jsonSnippet[key_description].toString();
            }

            if(jsonSnippet.contains(key_thumbnails)){
                QJsonObject jsonThumbNails = jsonSnippet[key_thumbnails].toObject();

                if(jsonThumbNails.contains(key_high)){
                    QJsonObject jsonMedium = jsonThumbNails[key_high].toObject();
                    if(jsonMedium.contains(key_url)){
                        thumbnail = jsonMedium[key_url].toString();
                    }
                }else if(jsonThumbNails.contains(key_medium)){
                    QJsonObject jsonMedium = jsonThumbNails[key_medium].toObject();
                    if(jsonMedium.contains(key_url)){
                        thumbnail = jsonMedium[key_url].toString();
                    }
                }else if(jsonThumbNails.contains(key_default)){
                    QJsonObject jsonMedium = jsonThumbNails[key_default].toObject();
                    if(jsonMedium.contains(key_url)){
                        thumbnail = jsonMedium[key_url].toString();
                    }
                }
            }
        }

        if(jsonData.contains(key_detailContents)){
            QJsonObject jsonDetails = jsonData[key_detailContents].toObject();

            if(jsonDetails.contains(key_duration)){
                duration = jsonDetails[key_duration].toString();
            }
            if(jsonDetails.contains(key_definition)){
                definition = jsonDetails[key_definition].toString();
            }
            if(jsonDetails.contains(key_dimension)){
                dimension = jsonDetails[key_dimension].toString();
            }
        }

        if(jsonData.contains(key_trackStatistic)){
            QJsonObject jsonStatistic = jsonData[key_trackStatistic].toObject();

            if(jsonStatistic.contains(key_viewCount)){
                viewCount = jsonStatistic[key_viewCount].toString();
                if(viewCount.isEmpty()){
                    int tmpView = jsonStatistic[key_viewCount].toInt(0);
                    if(tmpView > 0){
                        viewCount = QString("%1").arg(tmpView);
                    }
                }
            }
            if(jsonStatistic.contains(key_likeCount)){
                likeCount = jsonStatistic[key_likeCount].toString();
            }
        }
    }
    else{        
        if(thumbnail.isEmpty()){
            thumbnail = p_json["thumbnailUrl"].toString();
        }

        if(p_json.contains(key_snippet)){
            QJsonObject jsonSnippet = p_json[key_snippet].toObject();

            if(jsonSnippet.contains(key_channelId)){
                channelId = jsonSnippet[key_channelId].toString();
            }

            if(jsonSnippet.contains(key_thumbnails)){
                QJsonObject jsonThumbNails = jsonSnippet[key_thumbnails].toObject();

                if(jsonThumbNails.contains(key_medium)){
                    QJsonObject jsonMedium = jsonThumbNails[key_medium].toObject();
                    if(jsonMedium.contains(key_url)){
                        thumbnail = jsonMedium[key_url].toString();
                    }
                }else if(jsonThumbNails.contains(key_default)){
                    QJsonObject jsonMedium = jsonThumbNails[key_default].toObject();
                    if(jsonMedium.contains(key_url)){
                        thumbnail = jsonMedium[key_url].toString();
                    }
                }
            }

            if(jsonSnippet.contains(key_channelTitle)){
                channelTitle = jsonSnippet[key_channelTitle].toString();
            }
        }

        if(p_json.contains(key_detailContents)){
            QJsonObject jsonDetails = p_json[key_contentDetails].toObject();

            if(jsonDetails.contains(key_duration)){
                duration = jsonDetails[key_duration].toString();
            }
        }

        if(p_json.contains(key_statistic)){
            QJsonObject jsonStatistic = p_json[key_statistic].toObject();

            if(jsonStatistic.contains(key_viewCount)){
                viewCount = jsonStatistic[key_viewCount].toString();
                if(viewCount.isEmpty()){
                    int tmpView = jsonStatistic[key_viewCount].toInt(0);
                    if(tmpView > 0){
                        viewCount = QString("%1").arg(tmpView);
                    }
                }
            }
        }
    }
}


QJsonObject Data_RT_Track::getDataJson(){

    const QString key_no = "no";
    const QString key_star = "star";
    const QString key_playlistno = "playlistno";
    const QString key_title = "title";
    const QString key_playurl = "playurl";
    const QString key_favorites = "favorites";
    const QString key_clientKey = "clientKey";
    const QString key_comment = "comment";

    const QString key_snippet = "snippet";
    const QString key_channelId = "channelId";
    const QString key_thumbnails = "thumbnails";
    const QString key_medium = "medium";
    const QString key_default = "default";
    const QString key_url = "url";
    const QString key_channelTitle = "channelTitle";
    const QString key_categoryId = "categoryId";
    const QString key_description = "description";

    const QString key_contentDetails = "contentDetails";
    const QString key_duration = "duration";
    const QString key_duration_convert = "duration_convert";

    const QString key_detailContents = "detailContents";
    const QString key_definition = "definition";
    const QString key_dimension = "dimension";

    const QString key_statistic = "statistic";
    const QString key_viewCount = "viewCount";

    const QString key_trackStatistic = "trackStatistic";
    const QString key_likeCount = "likeCount";


    QJsonObject tmp_data;
    tmp_data.insert(key_no, this->no);
    tmp_data.insert(key_star, this->star);
    tmp_data.insert(key_playlistno, this->playlistno);
    tmp_data.insert(key_title, this->title);
    tmp_data.insert(key_playurl, this->playurl);
    tmp_data.insert(key_favorites, this->favorites);
    tmp_data.insert(key_channelId, this->channelId);
    tmp_data.insert(key_channelTitle, this->channelTitle);
    tmp_data.insert(key_duration, this->duration);
    tmp_data.insert(key_duration_convert,getConvertDuration());
    tmp_data.insert(key_viewCount, this->viewCount);
    tmp_data.insert(key_thumbnails, this->thumbnail);

    tmp_data.insert(key_clientKey, this->clientKey);
    tmp_data.insert(key_comment, this->comment);
    tmp_data.insert(key_description, this->description);
    tmp_data.insert(key_definition, this->definition);
    tmp_data.insert(key_dimension, this->dimension);
    tmp_data.insert(key_likeCount, this->likeCount);

    return tmp_data;
}

int Data_RT_Track::getStar() const
{
    return star;
}

int Data_RT_Track::getNo() const
{
    return no;
}

int Data_RT_Track::getPlaylistno() const
{
    return playlistno;
}

QString Data_RT_Track::getTitle() const
{
    return title;
}

QString Data_RT_Track::getArtist() const
{
    return artist;
}

QString Data_RT_Track::getPlayurl() const
{
    return playurl;
}

QString Data_RT_Track::getChannelId() const
{
    return channelId;
}

QString Data_RT_Track::getThumbnail() const
{
    return thumbnail;
}

QString Data_RT_Track::getChannelTitle() const
{
    return channelTitle;
}

QString Data_RT_Track::getDuration() const
{
    return duration;
}

/**
 * @brief Data_RT_Track::getConvertDuration : duration 시간 변환
 * @details 시간 포맷 EX) PT3M18S
 * @return
 */
QString Data_RT_Track::getConvertDuration() const
{
    if(duration.size() > 2){


        QString time = duration;
        QString hour;
        QString minute;
        QString second;

        if(duration.indexOf("PT")>=0){
            time = duration.right(duration.size()-2);
        }
        if(time.indexOf("H")>=0){
            hour = time.section("H",0,-2);
            time = time.mid(time.indexOf("H")+1);
        }
        if(time.indexOf("M")>=0){
            minute = time.section("M",0,-2);
            time = time.mid(time.indexOf("M")+1);
        }
        if(time.indexOf("S")>=0){
            second = time.section("S",0,-2);
        }


        if(hour.isEmpty()){
            if(minute.isEmpty()){
                return "0:"+second.rightJustified(2,'0');
            }
            return minute+":"+second.rightJustified(2,'0');
        }

        return  hour+":"+minute.rightJustified(2,'0')+":"+second.rightJustified(2,'0');

    }
    return duration;
}

QString Data_RT_Track::getFavorites() const
{
    return favorites;
}

void Data_RT_Track::setFavorites(const QString &value)
{
    favorites = value;
}

QString Data_RT_Track::getViewCount() const
{
    return viewCount;
}
