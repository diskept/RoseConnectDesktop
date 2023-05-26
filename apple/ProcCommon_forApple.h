#ifndef PROCCOMMON_FORAPPLE_H
#define PROCCOMMON_FORAPPLE_H

#include "apple/Apple_struct.h"

#include <QObject>
#include <QJsonObject>


namespace apple {

    static QList<apple::GenreInfo> list_genre;


    /**
     * @brief Apple에서만 사용하는 전용 처리담당 클래스.
     */
    class ProcCommon : public QObject
    {
        Q_OBJECT

    public :
        /**
         * @brief Tidal용 Request - about TIDAL API
         */
        enum HttpRequestType {
            Get_userToken = 0

            , GetRecommendations

            , GetAlbumInfo
            , GetPlaylistinfo

            , GetChartInfo

            , GetList_Genres = 1001
        };

        /**
         * @brief 타이달 데이터 정렬 옵션
         */
        enum DataOrderOption {
            NAME            ///< 이름순 정렬
            , DATE          ///< 날짜순 정렬
        };
        Q_ENUM(DataOrderOption);

        /**
         * @brief 타이달 데이터 정렬 방향 (오름차순, 내림차순)
         */
        enum DataOrderDirection {
            DESC            ///< 내림차순
            , ASC           ///< 올림차순
        };
        Q_ENUM(DataOrderDirection);

    signals:
        void signal_completeReq_getListGenre(const bool flagSuccess, const QString errorMsg);

        void completeReq_album(const QJsonObject&);
        void completeReq_playlist(const QJsonObject&);
        void completeReq_tracks(const QJsonArray&);

    public:
        explicit ProcCommon(QWidget *parent = nullptr);
        ~ProcCommon();

        // HTTP 요청 - 공통으로 사용하는 Apple API
        void request_apple_getListGenres();

        void request_apple_get_recommendations();

        void request_apple_get_album(const int album_id);
        void request_apple_get_playlist(const QString playlist_id);

        void request_apple_get_chart(const QString types, const int genre, const int limit, const int offset);

        // Data 관련 반환 - Genres
        static int getCount_listGenreInfo();
        static apple::GenreInfo getGenreInfo(int idx);

    private slots:
        void slot_responseHttp(int p_id, QJsonObject p_jsonObj);

    private:
        void setResult_listGenres(const QJsonObject&);

        void setResult_recommendations(const QJsonObject&);

        void setResult_album(const QJsonObject&);
        void setResult_playlist(const QJsonObject&);

        void setResult_chart(const QJsonObject&);
    };
};
#endif // PROCCOMMON_FORAPPLE_H
