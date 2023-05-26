#ifndef TIDALRECENTLYLISTDELETE_H
#define TIDALRECENTLYLISTDELETE_H

#include "tidal/AbstractTidalSubWidget.h"

#include <QCheckBox>


namespace tidal {

    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class TidalRecentlyListDelete: public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalRecentlyListDelete(QWidget *parent = nullptr);
        ~TidalRecentlyListDelete();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_albumlists(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_playlistLists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_rosetubelists(const QJsonArray&, const int&, const bool);
        void slot_applyResult_tracklists(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_artistlists(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getDeleteList(const bool);

        void slot_thumbnailDownload_list();

        void slot_clickedSelected(int state);
        void slot_clicked_btnSelectAll();

        void slot_clicked_Delete();
        void slot_clicked_DeleteAll();

        bool selectDelete_DialogConfirm(const QString type);

    private:
        void setUIControl_lists();

        void request_more_listData();
        void request_more_listDraw();

        inline void iniDataInfo_RoseList_Item(const QJsonObject &trackInfo, const int &p_index);
        inline void setDataInfo_RoseList_Item(const QJsonObject &trackInfo, const int &p_index);

    private:
        Linker *linker;

        QString contents_type = "";
        QString list_type = "";
        QString api_subPath = "";
        QString filter_type = "";

        QWidget *widget_mainTitle;
        QLabel *label_mainTitle;
        QPushButton *btn_deleteAll;
        QPushButton *btn_delete;
        QPushButton *btn_selectAll;

        QLabel *label_deleteAll;
        QLabel *label_delete;
        QLabel *label_selectAll;

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QListWidget *listWidget_recentlyList;

        QCheckBox *checkbox_selectAll;
        QCheckBox *checkbox_Selected[1000];

        QLabel *label_Img_recently[1000];
        QLabel *lb_explicit[1000];
        QLabel *lb_title[1000];
        QLabel *lb_album[1000];
        QLabel *lb_hires[1000];
        QLabel *lb_resolution[1000];
        QLabel *lb_artist[1000];

        // data
        QJsonArray jsonArr_list_data;

        QList<QString> selectIds;

        bool flag_widget_visible = false;

        bool flagReqMore_data = false;
        bool flag_lastPage_data = false;

        bool flag_draw = false;

        int next_offset = 0;
        int list_totalCount = 0;
        int list_drawCount = 0;
        int before_scrollMax = 0;
    };
};
#endif // TIDALRECENTLYLISTDELETE_H
