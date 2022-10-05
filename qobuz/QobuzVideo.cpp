#include "ConvertData_forQobuz.h"
#include "ProcCommon_forQobuz.h"
#include "QobuzVideo.h"
#include "common/gscommon.h"

namespace qobuz {


    const int BTN_IDX_SUBTITLE_video_new = 1;
    const int BTN_IDX_SUBTITLE_video_top = 2;

    const int SECTION_FOR_MORE_POPUP___video_new = 1;
    const int SECTION_FOR_MORE_POPUP___video_top = 2;

    const char* KEY_SECTION = "section";


    const QString SUB_TITLE___new = "New Music Video";
    const QString SUB_TITLE___top = "Top Music Video";

    const QString TIDAL_API_PATH___new = "featured/new/videos";
    const QString TIDAL_API_PATH___top = "featured/top/videos";

    /**
     * @brief "타이달 > 비디오" 화면의 생성자
     * @param parent
     */
    QobuzVideo::QobuzVideo(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {

        this->list_video_new = new QList<qobuz::VideoItemData>();
        this->list_video_top = new QList<qobuz::VideoItemData>();

        this->setUIControl_basic();
    }

    /**
     * @brief 소멸자.
     */
    QobuzVideo::~QobuzVideo(){
    }


    /**
     * @brief 화면 활성화
     */
    void QobuzVideo::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        // 데이터가 없으면 강제로 다시 Draw 하도록 한다.
        if(this->list_video_new->length()==0 || this->list_video_top->length()==0){
            this->flagInitDraw = false;
        }

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            // init UI
            GSCommon::clearLayout(this->hBox_video_new);
            GSCommon::clearLayout(this->hBox_video_top);

            // init data
            this->list_video_new->clear();
            this->list_video_top->clear();

            // request HTTP API
            ProcCommon *proc_new = new ProcCommon(this);
            proc_new->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_new);
            connect(proc_new, &ProcCommon::completeReq_list_videos, this, &QobuzVideo::slot_applyResult_videos);
            proc_new->request_qobuz_getList_videos(TIDAL_API_PATH___new, 10, 0);

            ProcCommon *proc_top = new ProcCommon(this);
            proc_top->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_top);
            connect(proc_top, &ProcCommon::completeReq_list_videos, this, &QobuzVideo::slot_applyResult_videos);
            proc_top->request_qobuz_getList_videos(TIDAL_API_PATH___top, 10, 0);

        }


    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 기본 UI 구성
     */
    void QobuzVideo::setUIControl_basic(){
        // 기본 UI 세팅
        this->addUIControl_subTitle_withSideBtn_common(SUB_TITLE___new, tr("View All"), BTN_IDX_SUBTITLE_video_new);
        this->hBox_video_new = this->get_addUIControl_hBoxLayout_forVideo();

        this->addUIControl_subTitle_withSideBtn_common(SUB_TITLE___top, tr("View All"), BTN_IDX_SUBTITLE_video_top);
        this->hBox_video_top = this->get_addUIControl_hBoxLayout_forVideo();
    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot] functions
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief QobuzVideo::slot_applyResult_videos
     * @param list_data
     * @param flag_lastPage
     */
    void QobuzVideo::slot_applyResult_videos(const QList<qobuz::VideoItemData> &list_data, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        int section = sender()->property(KEY_SECTION).toInt();

        if(section == SECTION_FOR_MORE_POPUP___video_new){
            this->list_video_new->append(list_data);
            this->createList_itemVideo_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Ractangle_340x188, this->hBox_video_new, 0, SECTION_FOR_MORE_POPUP___video_new);
        }
        else if(section == SECTION_FOR_MORE_POPUP___video_top){
            // api가 limit 적용이 안됨
            QList<qobuz::VideoItemData> list_10;
            for(int i=0 ; i<10 ; i++){
                if(i >= list_data.size()){
                    break;
                }
                list_10.append(list_data.at(i));
            }

            this->list_video_top->append(list_10);
            this->createList_itemVideo_applyingWithData(list_10, tidal::AbstractItem::ImageSizeMode::Ractangle_340x188, this->hBox_video_top, 0, SECTION_FOR_MORE_POPUP___video_top);
        }


    }



    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void QobuzVideo::slot_clickBtn_subTitle_side(const int btnId){
        if(btnId == BTN_IDX_SUBTITLE_video_new){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = SUB_TITLE___new;
            data_page.api_subPath = TIDAL_API_PATH___new;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_video_top){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = SUB_TITLE___top;
            data_page.api_subPath = TIDAL_API_PATH___top;
            this->movePage_video_allView(data_page);
        }
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzVideo::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Video 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___video_new){
            this->proc_clicked_itemVideo(this->list_video_new, clickMode, index, section);
        }
        else{
            this->proc_clicked_itemVideo(this->list_video_top, clickMode, index, section);
        }
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
    void QobuzVideo::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___video_new){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video_new, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___video_top){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video_top, index, clickMode);
        }
    }

}
