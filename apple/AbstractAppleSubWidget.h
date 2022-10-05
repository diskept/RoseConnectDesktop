#ifndef ABSTRACTAPPLESUBWIDGET_H
#define ABSTRACTAPPLESUBWIDGET_H

#include "apple/Apple_struct.h"
#include "apple/ConvertData_forApple.h"

#include "common/AbstractCommonSubWidget.h"

#include "login/dialog_playlist_onRose.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogArtistList.h"
#include "widget/dialogNotice.h"
#include "widget/dialogconfirm.h"

#include "bugs/BugsChooseFilterOpt.h"       // for filtering

#include <QJsonArray>
#include <QStackedWidget>


namespace apple {

    /**
     * @brief 애플뮤직 서브 페이지를 구성하는데 있어, 상속체계의 부모가 되는 클래스
     * @details 공통으로 사용하는 시그널 및 공통 클래스 구성 스타일을 정의함 (클래스 변수 및 형태)
     */
    class AbstractAppleSubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT

    public:
        explicit AbstractAppleSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractAppleSubWidget();

        // APPLE 로그인 관련
        void slot_clickBtnLogin_toShowDialog();

    protected slots:

        // ProcCommon 으로부터 데이터 수신 처리 - 데이터 받는 경우 override 처리할 것
        virtual void slot_applyResult_playlists(const QJsonArray&, const bool){ }
        virtual void slot_applyResult_albums(const QJsonArray&, const bool){ }
        virtual void slot_applyResult_tracks(const QJsonArray&, const bool){ }
        virtual void slot_applyResult_artists(const QJsonArray&, const bool){ }

        // ItemWidget 관련
        virtual void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode){ }

        // AbstractImageDetailInfo 관련
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode){ }

        // ListWidget 관련
        virtual void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode){ }

        /// 내부 페이지 변경되었을때 슬롯 함수
        virtual void slot_changedSubTabUI(const QJsonObject&){ }

    protected:
        QWidget* get_createUIControl_notLoginView();

        // Filtering 관련
        static QList<bugs::FilterOptionData> get_list_filterOptData_forMyCollection();

    private:
        Linker *linker;

        DialogConfirm *dlgNotice;
    };
};
#endif // ABSTRACTAPPLESUBWIDGET_H
