#ifndef BUGSALBUMLISTALL_OFARTIST_H
#define BUGSALBUMLISTALL_OFARTIST_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief Artist에 대한 Album 전체 목록
     */
    class BugsAlbumListAll_ofArtist : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsAlbumListAll_ofArtist(QWidget *parent = nullptr);
        ~BugsAlbumListAll_ofArtist();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_preUpdate_favorite_album(const ItemPositionData&) override;
        void proc_wheelEvent_to_getMoreData() override;

    private slots:

    private:
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        bugs::ItemAlbum_bugs *listAlbum_all[999999];

        QList<bugs::AlbumItemData> *list_album;

        QString api_subPath;
        bugs::PageInfo_AlbumAllView_ofArtist data_pageInfo;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };

};


#endif // BUGSALBUMLISTALL_OFARTIST_H
