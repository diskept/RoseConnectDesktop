#ifndef TIDALALBUMLISTALL_H
#define TIDALALBUMLISTALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"

#include <QCoreApplication>



#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 앨범 전체보기 화면 클래스
     */
    class TidalAlbumListAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalAlbumListAll(QWidget *parent = nullptr);
        ~TidalAlbumListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;

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
        tidal::ItemAlbum *listAlbum_all[999999];

        QList<tidal::AlbumItemData> *list_album;

        QString api_subPath;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;

        bool visualAlbum_first = false;
    };

};

#endif // TIDALALBUMLISTALL_H
