#ifndef ROSESETTINGS_H
#define ROSESETTINGS_H

#include <QString>
#include <QDateTime>


/**
 * @brief The RoseSettings class : 로즈 설정 변수
 * @details Rose 환경변수 키 상수
 */
class RoseSettings
{
public :

    QString ORGANIZATION_NAME = "CITECH";                           ///<"Goosl Company";
    QString ORGANIZATION_DOMAIN = "hifirose.com";                   ///<"goosl.co.kr";
    QString APPLICATION_NAME = "Rose_Connect";
    QString APPLICATION_VERSION = "3.8.8";

    QString SETTINGS_NEED_TUTORIAL = "settings/tutorial";           ///<튜토리얼 show/hide
    QString SETTINGS_W = "settings/width";                          ///<App width
    QString SETTINGS_H = "settings/height";                         ///<App height
    QString SETTINGS_AUTO_LOGIN = "settings/autoLogin";             ///<자동로그인 유무
    QString SETTINGS_LANG = "settings/lang";                        ///<언어
    QString SETTINGS_RADIO_ID = "settings/radio_countryID";         ///<라디오 국가 ID
    QString SETTINGS_PODCAST_COUNTRY = "settings/pod_countryID";    ///<팟캐스트 국가 ID
    QString SETTINGS_RADIO_NAME = "settings/countryNAME";           ///<라디오 국가명
    QString SETTINGS_TRACK_OPTION = "settings/track_optioin";       ///<재생 방식 설정

    QString SETTINGS_FILTER_FILETYPE = "filter/fileType";           ///<음악분류 파일타입
    QString SETTINGS_FILTER_BIT = "filter/bitDepth";                ///<음악분류 Bit Depth
    QString SETTINGS_FILTER_SAMPLEING = "filter/sampling";          ///<음악분류 Sampling Rate
    QString SETTINGS_FILTER_ORDER = "filter/order";                 ///<음악분류 정렬순서
    QString SETTINGS_FILTER_STAR = "filter/star";                   ///<음악분류 별점


public :
    RoseSettings() = default;
    RoseSettings(const RoseSettings&) = delete;
    RoseSettings(RoseSettings&&) = delete;

    static RoseSettings& Instance(){
        static RoseSettings rosesettings;
        return rosesettings;
    }
};

namespace  {
    RoseSettings& rosesettings = RoseSettings::Instance();
}
#endif // ROSESETTINGS_H
