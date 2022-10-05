#ifndef CONVERTGENRECODE_BUGSAPI_H
#define CONVERTGENRECODE_BUGSAPI_H

#include <QObject>
#include <QWidget>
#include <QVariant>

namespace bugs {

    namespace ConvertGenreCode_BugsAPI_const {
        const int DEFAULT_GENRE_CODE = 20151;
    };

    /**
     * @brief Bugs의  장르에 대해서 장르 code 값으로, 필요한 API subpath를 반환해주는 클래스
     */
    class ConvertGenreCode_BugsAPI : public QObject
    {
        Q_OBJECT

    public:

        /**
         * @brief 컨텐츠 타입. Bugs API sub_path로 변환하는데, 그대로 사용하므로 함부로 수정하면 안됨.
         */
        enum class ContentType{
            track
            , album
            , mv            ///< video (Music Video 의미)
        };
        Q_ENUM(ContentType);


        static QString convert_api_subPath_musicRecently(const ContentType p_contentType, const QVariant p_genre_code);
        static QString convert_api_subPath_bugsChart(const ContentType p_contentType, const QVariant p_genre_code);
    };

};

#endif // CONVERTGENRECODE_BUGSAPI_H
