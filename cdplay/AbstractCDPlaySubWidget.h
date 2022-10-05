#ifndef ABSTRACTCDPLAYSUBWIDGET_H
#define ABSTRACTCDPLAYSUBWIDGET_H

#include "common/AbstractCommonSubWidget.h"

#include "cdplay/CDplay_struct.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QJsonArray>
#include <QStackedWidget>


namespace CDplay {

    class AbstractCDPlaySubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT
    public:
        explicit AbstractCDPlaySubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractCDPlaySubWidget();

    protected slots:
        // AbstractImageDetailInfo 관련 - 로즈홈 전용 (Album 상세, Playlist 상세)
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode){ }

        // ListWidget 관련 - 로즈홈 전용/
        virtual void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode){ }

    signals:
        /// 부모에게 페이지가 변경되었다고 히스토리 관리하라고 알려줘야한다. (Main에서 이전/다음 버튼 컨트롤하기 위함)
        void clickedSubTab(const QJsonObject &p_data);

    protected slots:
        /// 내부 페이지 변경되었을때 슬롯 함수
        virtual void slot_changedSubTabUI(const QJsonObject &p_data);

    protected:
        /// AbstractMainContent가 보여야할 위젯의 코드
        QString pageCode = "";
        /// 현재 클래스에서 step관리 하는 경우 코드값 (없으면 step이 존재하지 않는다는 뜻임)
        QString contentStep = "";
        virtual void setDataABSPage(QJsonObject p_data);
        virtual QJsonObject getDataJson();
    };
};
#endif // ABSTRACTCDPLAYSUBWIDGET_H
