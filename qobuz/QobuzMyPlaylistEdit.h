#ifndef QOBUZMYPLAYLISTEDIT_H
#define QOBUZMYPLAYLISTEDIT_H

#include "AbstractQobuzSubWidget.h"
#include <QCheckBox>
#include <QJsonArray>
#include <QListWidget>
#include "widget/PlaylistImageDetailInfo.h"
#include "delegate/QobuzEditTrackDelegate.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief My Playlist 1개 편집화면.
     */
    class QobuzMyPlaylistEdit : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyPlaylistEdit(QWidget *parent = nullptr);
        ~QobuzMyPlaylistEdit();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots :
        void slot_applyResult_items_of_playlist(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;

        // PlaylistImageDetailInfo의 signal 처리를 위해
        //void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode) override;
        void slot_detailInfo_btnClicked(const AbstractImageDetailInfo_forQobuz::BtnClickMode) override;


        void slot_editDelegate_thumbnailDownloaded_fotListWidget() override;

        // about OptMorePopup
//        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private slots:
        void slot_clickedAllSelected();
        void slot_clickedDelete();

        void slot_editDelegate_clickedItemTrack_inList(const int index, const AbstractEditListDelegate::ClickMode clickMode);
        void slot_completeReq_delete_items_in_myPlaylist();

    private:
        // function about UI
        void setUIControl_basic();
        void setUIControl_topBtns();

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;
        void request_more_trackData();

        // UI
        QLabel *label_mainTitle;

        // playlist info
        qobuz::PlaylistItemData data_playlist;
        //PlaylistImageDetailInfo *playlistImageDetailInfo;
        PlaylistImageDetailInfo_forQobuz *playlistImageDetailInfo;

        bool isAllSelected =false;      ///< 전체 체크선택 유무
        QCheckBox *checkbox_allSelected;

        // list track
        QListWidget *listWidget_track;
        QobuzEditTrackDelegate *delegate_edit;
        QList<qobuz::TrackItemData> *list_track;    ///< Playlist 내의 Track 정보

        QList<editDelegate::ItemEditData> *list_item_track;
//        QJsonArray jsonArr_tracks_toPlay;                  ///< Track 전체를 재생하기 위함


    };

};
#endif // QOBUZMYPLAYLISTEDIT_H
