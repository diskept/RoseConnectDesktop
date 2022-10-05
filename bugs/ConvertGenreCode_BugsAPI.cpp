#include "ConvertGenreCode_BugsAPI.h"

namespace bugs {


    /**
     * @brief 벅스 장르에 대해서, "홈 > 최신 음악" 에서 api subpath 생성하여 반환
     *      ContentType과 genre_code를 고려하여 Bugs의 API subPage를 반환한다.
     * @param p_contentType
     * @param genre_code
     * @return
     */
    QString ConvertGenreCode_BugsAPI::convert_api_subPath_musicRecently(const ConvertGenreCode_BugsAPI::ContentType p_contentType, const QVariant p_genre_code) {

        int genre_code = p_genre_code.toInt();
        if(genre_code==20151){
            // "전체 > 전체 장르" 선택한 경우 -> total/all 을 붙임. 장르코드 사용하지 않음
            return QString("genre/%1/total/all").arg(QVariant::fromValue(p_contentType).toString());
        }
        else if(genre_code==20152){
            // "전체 > 국내 전체장르" 선택한 경우 -> total/all 을 붙임. 장르코드 사용하지 않음
            return QString("genre/%1/total/kor").arg(QVariant::fromValue(p_contentType).toString());
        }
        else if(genre_code==20153){
            // "전체 > 해외 전체장르" 선택한 경우 -> total/all 을 붙임. 장르코드 사용하지 않음
            return QString("genre/%1/total/for").arg(QVariant::fromValue(p_contentType).toString());
        }
        else{
            // 그 이외 선택 -> 장르 코드를 붙임
            return QString("genre/%1/%2").arg(QVariant::fromValue(p_contentType).toString()).arg(genre_code);
        }
    }



    /**
     * @brief 벅스 장르에 대해서, "홈 > 벅스 차트" 에서 api subpath 생성하여 반환
     *      ContentType과 genre_code를 고려하여 Bugs의 API subPage를 반환한다.
     * @param p_contentType
     * @param p_genre_code
     * @return
     */
    QString ConvertGenreCode_BugsAPI::convert_api_subPath_bugsChart(const ConvertGenreCode_BugsAPI::ContentType p_contentType, const QVariant p_genre_code) {

        int genre_code = p_genre_code.toInt();
        if(p_contentType == ContentType::track){
            // Track
            if(genre_code==20151 || genre_code==20152 || genre_code==20153){
                // "전체 > 전체 장르, 국내 전체 장르, 해외 전체 장르" 선택한 경우 -> realtime, 장르 코드 붙임
                return QString("chart/%1/realtime/%2").arg(QVariant::fromValue(p_contentType).toString()).arg(genre_code);
            }
            else{
                // 그 이외 선택 -> day, 장르 코드를 붙임
                return QString("chart/%1/day/%2").arg(QVariant::fromValue(p_contentType).toString()).arg(genre_code);
            }
        }
        else{
            // Album, Video -> day, 장르 코드를 붙임
            return QString("chart/%1/day/%2").arg(QVariant::fromValue(p_contentType).toString()).arg(genre_code);
        }
    }


}

