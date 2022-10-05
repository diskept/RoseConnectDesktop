#ifndef QOBUZALBUMLISTALL_H
#define QOBUZALBUMLISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 앨범 전체보기 화면 클래스
     */
    class QobuzAlbumListAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzAlbumListAll(QWidget *parent = nullptr);
        ~QobuzAlbumListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        qobuz::ItemAlbum_qobuz *listAlbum_all[999999];

        QList<qobuz::AlbumItemData> *list_album;

        QString api_subPath;
        QString api_mainTitle;
        QString type;
        QString page;
        QString genre_ids;

        int genre_id;
        int artist_id;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    class QobuzLastReleaseList : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzLastReleaseList(QWidget *parent = nullptr);
        ~QobuzLastReleaseList();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:        
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        qobuz::ItemAlbum_qobuz *listAlbum_all[999999];

        QList<qobuz::AlbumItemData> *list_album;

        QString api_mainTitle;
        QString api_subPath;
        QString type;

        int artist_id;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    class QobuzSameArtistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzSameArtistAll(QWidget *parent = nullptr);
        ~QobuzSameArtistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private:
        // 초기 UI화면 구성
        void setUIControl_albums();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        qobuz::ItemAlbum_qobuz *sameArits_all[999999];

        QList<qobuz::AlbumItemData> *list_album;

        QString api_mainTitle;
        QString api_subPath;
        QString type;

        int artist_id;
    };

};

#endif // QOBUZALBUMLISTALL_H
