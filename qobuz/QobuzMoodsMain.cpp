#include "QobuzMoodsMain.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "ItemPlaylist_forQobuz.h"
#include <QDebug>
#include <common/global.h>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include "common/ProcJsonEasy.h"
#include "qobuz/ProcCommon_forQobuz.h"

namespace qobuz{


    const int SECTION_FOR_MORE_POPUP___playlists = 1;

    const int GET_ITEM_SIZE___ONCE = 30;


    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzMoodsMain::QobuzMoodsMain(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {
        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("Moods & Activities name");
        this->setUIControl_playlist();

        this->list_playlist = new QList<qobuz::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    QobuzMoodsMain::~QobuzMoodsMain(){
    }



    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzMoodsMain::setJsonObject_forData(const QJsonObject &jsonObj){
        this->flagNeedReload =false;
        QString curr_path = this->mood_path;
        this->mood_path = QString("moods/%1").arg(ProcJsonEasy::getString(jsonObj, "path", ""));

        if(curr_path != this->mood_path){
            this->flagNeedReload = true;
            this->label_mainTitle->setText(ProcJsonEasy::getString(jsonObj, "name", "Noname genre"));
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 타이달 > 탐색 > "무드" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzMoodsMain::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        if(this->flagNeedReload){
            // init
            this->list_playlist->clear();
            GSCommon::clearLayout(this->flowLayout_playlists);

            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;
            this->request_more_playlistData();
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzMoodsMain::setUIControl_playlist(){
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void QobuzMoodsMain::proc_wheelEvent_to_getMoreData(){
        this->request_more_playlistData();
    }

    /**
     * @brief Qobuz API 요청
     */
    void QobuzMoodsMain::request_more_playlistData(){
        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // next_offset
            int next_offset = this->list_playlist->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &QobuzMoodsMain::slot_applyResult_playlists);
            proc->request_qobuz_getList_playlists(QString("%1/playlists").arg(this->mood_path), "", 0, GET_ITEM_SIZE___ONCE, next_offset);
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] Qobuz API playlist 반환값에 대하여 UI 적용
     * @param list_data
     */
    void QobuzMoodsMain::slot_applyResult_playlists(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = flag_lastPage;

        int start_index = this->list_playlist->count();
        this->list_playlist->append(list_data);
        this->createList_itemPlaylsit_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_playlists, start_index, SECTION_FOR_MORE_POPUP___playlists);
    }



    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzMoodsMain::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzMoodsMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
    }

}
