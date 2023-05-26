#include "rosetube/rosetube_HistoryListAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace rosetube {

    const int SECTION_FOR_MORE_POPUP___history = 0;

    /**
     * @brief RoseTubeHistoryListAll::RoseTubeHistoryListAll
     * @param parent
     */
    RoseTubeHistoryListAll::RoseTubeHistoryListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_history = new QList<roseHome::HistoryItemData>();

        // 기본 UI 세팅
        this->setUIControl_history();
    }


    /**
     * @brief 소멸자.
     */
    RoseTubeHistoryListAll::~RoseTubeHistoryListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseTubeHistoryListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_HistoryAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_historyAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.pathTitle != data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type || tmp_pageInfo.list_history.count() != this->data_pageInfo.list_history.count()){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseTubeHistoryListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_history);

            QJsonArray tmpArr;

            this->slot_applyResult_history(this->data_pageInfo.list_history, tmpArr, true);
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
    void RoseTubeHistoryListAll::setUIControl_history(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("History");
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemHistory_rosehome *listAlbum =  new roseHome::ItemHistory_rosehome(0, SECTION_FOR_MORE_POPUP___history, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
        this->history_widget_width = listAlbum->get_fixedWidth();
        this->history_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeHisotryListAll::setUIControl_history " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_history = this->get_addUIControl_flowLayout(0, 30);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void RoseTubeHistoryListAll::slot_applyResult_history(const QList<roseHome::HistoryItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            int start_index = this->list_history->size();
            this->list_history->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_history->size();

                for(int i = start_index; i < max_cnt; i++){
                    this->listHistory_all[i] = new roseHome::ItemHistory_rosehome(i, SECTION_FOR_MORE_POPUP___history, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listHistory_all[i]->setData(this->list_history->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < max_cnt; i++){
                    connect(this->listHistory_all[i], &roseHome::ItemHistory_rosehome::signal_clicked, this, &RoseTubeHistoryListAll::slot_clickedItemAlbum);
                    this->flowLayout_history->addWidget(this->listHistory_all[i]);
                }
            }

            ContentLoadingwaitingMsgHide();
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_history->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_history->geometry().width();

                if(this->flowLayout_history->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::History_NoDate);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_history->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void RoseTubeHistoryListAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        //int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        //this->proc_clicked_itemHistory(this->list_history, clickMode, index, section);

        roseHome::HistoryItemData tmpData;
        tmpData.yearMonth = this->list_history->at(index).yearMonth;
        tmpData.visible = this->list_history->at(index).visible;
        tmpData.filter_type = "YOUTUBE";

        if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
            // History 페이지 진입
            QJsonObject tmp_data = roseHome::ConvertData::getObjectJson_historyData(tmpData);
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_HISTORY_DETAIL);
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_HISTORY_DETAIL);

            emit this->signal_clickedViewAll(tmp_data);
        }
    }

    /**
     * @brief RoseHomeHisotryListAll::resizeEvent
     * @param event
     */
    void RoseTubeHistoryListAll::resizeEvent(QResizeEvent *event){//c230223

        Q_UNUSED(event);

        // list count check - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_history, this->history_widget_width, this->history_widget_margin);
        // list count check - by diskept j230317 finish

    }
}
