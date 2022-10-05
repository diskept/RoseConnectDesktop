#include "BugsThemesChoose.h"
#include "DataCommon_Bugs.h"
#include <QPixmapCache>

#include "common/gscommon.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "bugs/ConvertData_forBugs.h"



namespace bugs {


    const char* KEY_INDEX = "idx";

    /**
     * @brief
     * @param parent
     */
    BugsThemesChoose::BugsThemesChoose(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll, parent) {
        this->list_tag = new QList<bugs::BugsTag>();
    }



    /**
     * @brief BugsThemesChoose::setJsonObject_forData
     * @param jsonObj
     */
    void BugsThemesChoose::setJsonObject_forData(const QJsonObject &jsonObj){

        FilterOptionData tmp_data_filterOpt = ConvertData_forBugs::convertData_filterOptionData(jsonObj);

        this->flagNeedReload = false;
        if(this->data_filterOpt.opt_code.toString() != tmp_data_filterOpt.opt_code.toString()){
            this->flagNeedReload = true;
            this->data_filterOpt.opt_code = tmp_data_filterOpt.opt_code;
            this->data_filterOpt.opt_name = tmp_data_filterOpt.opt_name;
        }

    }


    /**
     * @brief BugsThemesChoose::applyLoginSuccessState
     */
    void BugsThemesChoose::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                this->list_tag->clear();

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }


            // 필요에 따라 리로드 한다.
            if(this->flagNeedReload){

                // 선택된 code에 대한 index를 찾아서 세팅. (BugsExplore에서 코드 선택해서 온 경우 처리)
                DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
                int tmp_index = 0;
                for(int i=0 ; i<dataCommon_bugs.list_theme.count() ; i++){
                    if(dataCommon_bugs.list_theme.at(i).opt_code.toString() == this->data_filterOpt.opt_code.toString()){
                        tmp_index = i;
                        break;
                    }
                }
                this->segmentTabWidget->set_selectedIndex(tmp_index);

                // request HTTP API - 초기 요청값으로 처리함
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                connect(proc, &ProcBugsAPI::completeReq_list_tags_in_theme, this, &BugsThemesChoose::slot_applyResult_tags_in_theme);
                proc->request_bugs_getListTags_in_theme(this->data_filterOpt.opt_code.toString());
            }
        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 화면 UI 세팅
     */
    void BugsThemesChoose::setUIControl_logined(){

        // 메인 타이틀
        this->addUIControl_mainTitle("장르/테마");
        // Tab 구성
        this->setUIControl_themesTab();

        // FlowLayout 구성
        this->flowLayout_tags = new FlowLayout(0, 22, 0);
        this->flowLayout_tags->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(this->flowLayout_tags);
        widget_content->setContentsMargins(0,0,10,0);
        widget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        this->box_contents->addWidget(widget_content, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

    }

    /**
     * @brief BugsThemesChoose::setUIControl_themesTab
     */
    void BugsThemesChoose::setUIControl_themesTab(){

        // 좌측의 탭 메뉴
        DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
        QList<QString> list_tab;
        foreach(bugs::FilterOptionData tmp_optData, dataCommon_bugs.list_theme){
            list_tab.append(tmp_optData.opt_name);
        }

        this->segmentTabWidget = new SegmentTabWidget();
        this->segmentTabWidget->setList_tabName(list_tab);
        this->segmentTabWidget->set_selectedIndex(0);
        connect(this->segmentTabWidget, &SegmentTabWidget::signal_clicked, this, &BugsThemesChoose::slot_segmentTag_clicked);

        QHBoxLayout *hBox_filter = new QHBoxLayout();
        hBox_filter->setSpacing(0);
        hBox_filter->setContentsMargins(0,0,0,0);
        hBox_filter->addStretch(1);
        hBox_filter->addWidget(this->segmentTabWidget, 0, Qt::AlignVCenter);
        hBox_filter->addSpacing(77);

        QWidget *tmp_widget_forFilter = new QWidget();
        tmp_widget_forFilter->setContentsMargins(0,0,0,0);
        tmp_widget_forFilter->setObjectName("widget_forFilter");
        tmp_widget_forFilter->setStyleSheet("#widget_forFilter { background-color:#171717; }");
        tmp_widget_forFilter->setLayout(hBox_filter);
        tmp_widget_forFilter->setFixedHeight(70);

        this->box_mainTitle->addWidget(tmp_widget_forFilter);
        this->addUIControl_dividedLine_onMain();

    }





    /**
     * @brief [slot] BugsThemesChoose::slot_segmentTag_clicked
     * @param clickedIndex
     */
    void BugsThemesChoose::slot_segmentTag_clicked(int clickedIndex){

        DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
        FilterOptionData tmp_filterOpt = dataCommon_bugs.list_theme.at(clickedIndex);

        // 원래의 theme_code와 다른 경우에만 동작
        if(tmp_filterOpt.opt_code.toString() != this->data_filterOpt.opt_code.toString()){

            this->data_filterOpt = tmp_filterOpt;

            // init
            GSCommon::clearLayout(this->flowLayout_tags);
            this->list_tag->clear();

            this->scrollTop();

            // 해당 index 관련해서 http API 요청
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_tags_in_theme, this, &BugsThemesChoose::slot_applyResult_tags_in_theme);
            proc->request_bugs_getListTags_in_theme(this->data_filterOpt.opt_code.toString());
        }
    }


    /**
     * @brief [Slot] Tag 목록 HTTP 반환에 대하여 화면 처리
     * @param list_tag
     */
    void BugsThemesChoose::slot_applyResult_tags_in_theme(const QList<bugs::BugsTag>& list_tag){

        this->list_tag->append(list_tag);       // data

        // loop
        for(int i=0 ; i<list_tag.count() ; i++){

            // 라벨 먼저
            QLabel *label_tagName = new QLabel();
            label_tagName->setText("#" + list_tag.at(i).tag_nm);
            label_tagName->setStyleSheet("background-color:transparent; font-size:18px; padding-top:8px; padding-left:5px; color:#ffffff; border-bottom:2px solid #333333;");        // 2px 이군
            label_tagName->setFixedSize(524, 70);

            QHBoxLayout *tmp_hBox = new QHBoxLayout();
            tmp_hBox->setSpacing(0);
            tmp_hBox->setContentsMargins(0,0,0,0);
            tmp_hBox->addWidget(label_tagName);

            QPushButton *btn_tag = new QPushButton();
            btn_tag->setProperty(KEY_INDEX, i);
            btn_tag->setLayout(tmp_hBox);
            btn_tag->setCursor(Qt::PointingHandCursor);
            btn_tag->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");
            connect(btn_tag, &QPushButton::clicked, this, &BugsThemesChoose::slot_tagBtnClicked);
            btn_tag->setFixedSize(524, 70);


            this->flowLayout_tags->addWidget(btn_tag);
        }

    }


    /**
     * @brief [Slot] Tag 버튼 클릭에 대한 처리
     */
    void BugsThemesChoose::slot_tagBtnClicked(){
        int index = sender()->property(KEY_INDEX).toInt();
        if(index>=0 && index<this->list_tag->size()){
            // 해당 Tag 의 PD's Album All View 페이지 이동
            PageInfo_PDAlbumAllView data_pageInfo;
            data_pageInfo.tag_ids = QString("%1").arg(this->list_tag->at(index).tag_id);
            data_pageInfo.mainTitle = this->list_tag->at(index).tag_nm;
            data_pageInfo.sortOpt = QVariant::fromValue(ProcBugsAPI::BugsPDAlbum_Sort::Recent).toString();

            this->movePage_pd_album_allView(data_pageInfo);
        }
    }

}
