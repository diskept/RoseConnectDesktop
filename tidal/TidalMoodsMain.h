#ifndef TIDALMOODSMAIN_H
#define TIDALMOODSMAIN_H

#include "AbstractTidalSubWidget.h"
#include "tidal_struct.h"
#include <widget/flowlayout.h>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal > 탐색 > Moods & Activities-Relax > Mood 하나 선택했을 때의 메인 화면
     */
    class TidalMoodsMain : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMoodsMain(QWidget *parent = nullptr);
        ~TidalMoodsMain();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    protected:
        void proc_wheelEvent_to_getMoreData() override;


    private slots:
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;

    private:

        // 초기 UI화면 구성
        void setUIControl_playlist();


        // getting more data
        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;
        void request_more_playlistData();


        QLabel *label_mainTitle;        ///< main titel for specific mood's name
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        QString mood_path;
        QList<tidal::PlaylistItemData> *list_playlist;

        QJsonArray jsonArr_playlist;

    };

};

#endif // TIDALMOODSMAIN_H
