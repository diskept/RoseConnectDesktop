#include "BugsChooseFilterOpt.h"
#include "DataCommon_Bugs.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "common/gscommon.h"
//#include "bugs/ProcBugsAPI.h"
#include "bugs/bugs_struct.h"
#include <QMouseEvent>
#include "widget/NoData_Widget.h"

namespace bugs {


    const char* KEY_CODE = "code";
    const char* KEY_NAME = "name";

    const int WIDTH_OPTION_UNIT = 130;                              ///< Option Group 그룹 하나에 대한 가로 사이즈.
    const int WIDTH_OPTION_BTN = 110;                               ///< Option 버튼의 Width




    const int HEIGHT_HEADER_LABEL___SMALL = 44;                             ///< Option Group의 헤더 라벨의 Height
    const int HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___SMALL = 8;       ///< Option 버튼의 위, 아래 spacing
    const int HEIGHT_OPTION_BTN___SMALL = 27;                               ///< Option 버튼의 Height
    const QString BTN_STYLE_NORMAL___SMALL = ".QPushButton{ "
                                            "background:transparent;color:#FFFFFF;font-size:16px;"
                                            "border:1px solid #707070; border-radius:13px;}"
                                     ".QPushButton:hover{background-color:#BE905F; }";
    const QString BTN_STYLE_SELECTED___SMALL = ".QPushButton{ "
                                            "background:#BE905F;color:#FFFFFF;font-size:16px;"
                                            "border:1px solid #BE905F; border-radius:13px;}";


    const int HEIGHT_HEADER_LABEL___NORMAL = 50;                             ///< Option Group의 헤더 라벨의 Height
    const int HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___NORMAL = 10;       ///< Option 버튼의 위, 아래 spacing
    const int HEIGHT_OPTION_BTN___NORMAL = 30;                               ///< Option 버튼의 Height
    const QString BTN_STYLE_NORMAL___NORMAL = ".QPushButton{ "
                                            "background:transparent;color:#FFFFFF;font-size:16px;"
                                            "border:1px solid #707070; border-radius:15px;}"
                                     ".QPushButton:hover{background-color:#BE905F; }";
    const QString BTN_STYLE_SELECTED___NORMAL = ".QPushButton{ "
                                            "background:#BE905F;color:#FFFFFF;font-size:16px;"
                                            "border:1px solid #BE905F; border-radius:15px;}";




    /**
     * @brief BugsChooseFilterOpt::BugsChooseFilterOpt
     * @param parent
     */
    BugsChooseFilterOpt::BugsChooseFilterOpt(QWidget *parent) : QWidget(parent)
    {
        this->m_filterOptSize = FilterOptSize::Normal;      // default
        this->setUIControl_basic();
    }

    /**
     * @brief BugsChooseFilterOpt::BugsChooseFilterOpt
     * @param optSize
     * @param parent
     */
    BugsChooseFilterOpt::BugsChooseFilterOpt(FilterOptSize optSize, QWidget *parent) : QWidget(parent){
        this->m_filterOptSize = optSize;
        this->setUIControl_basic();
    }

    /**
     * @brief BugsChooseFilterOpt::setUIControl_basic
     */
    void BugsChooseFilterOpt::setUIControl_basic(){

        // 필터링 박스에서 사용하는 레이아웃
        this->hBox_filterBox_main = new QHBoxLayout();
        this->hBox_filterBox_main->setSpacing(0);
        this->hBox_filterBox_main->setContentsMargins(0,0,0,0);

        // 필터링 박스 위젯
        this->widget_filterBox = new QWidget(this);
        this->widget_filterBox->setContentsMargins(0,0,0,0);
        this->widget_filterBox->setLayout(this->hBox_filterBox_main);
        this->widget_filterBox->setObjectName("widget_filterBox");
        this->widget_filterBox->setStyleSheet("#widget_filterBox { background-color:#333232; border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filterBox->setFixedSize(600,600);

        this->setContentsMargins(0,0,0,0);

    }


    /**
     * @brief BugsChooseFilterOpt::addFilterGroup
     * @param groupName
     * @param list_optData
     */
    void BugsChooseFilterOpt::addFilterGroup(QString groupName, const QList<FilterOptionData> list_optData, QVariant selected_code){

        //---------------------------------------------------
        // Vertical layout
        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,0,0,0);

        int btn_spacing = HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___NORMAL;
        int btn_height = HEIGHT_OPTION_BTN___NORMAL;
        QString btn_style_normal = BTN_STYLE_NORMAL___NORMAL;
        QString btn_style_selected = BTN_STYLE_SELECTED___NORMAL;
        if(this->m_filterOptSize == FilterOptSize::Small){
            btn_spacing = HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___SMALL;
            btn_height = HEIGHT_OPTION_BTN___SMALL;
            btn_style_normal = BTN_STYLE_NORMAL___SMALL;
            btn_style_selected = BTN_STYLE_SELECTED___SMALL;
        }


        // Header Label
        QLabel *label_header = this->make_headerLabel(groupName);
        tmp_vBox->addWidget(label_header);

        // making Option Button using the list data
        int max_cnt = list_optData.size();
        for(int i=0 ; i<max_cnt ; i++){
            bugs::FilterOptionData tmp_filterOptData = list_optData.at(i);

            bool flagSelected = false;
            if(selected_code==tmp_filterOptData.opt_code){
                flagSelected = true;
                this->selected_optCode = selected_code;
            }

            QPushButton *btn_filter = this->make_fiterButton(tmp_filterOptData.opt_name
                                                             , WIDTH_OPTION_BTN
                                                             , btn_height
                                                             , btn_style_normal
                                                             , btn_style_selected
                                                             , flagSelected);

            btn_filter->setProperty(KEY_CODE, tmp_filterOptData.opt_code);
            btn_filter->setProperty(KEY_NAME, tmp_filterOptData.opt_name);
            connect(btn_filter, &QPushButton::clicked, this, &BugsChooseFilterOpt::slot_clicked_filterOptDataBtn);

            tmp_vBox->addSpacing(btn_spacing);
            tmp_vBox->addWidget(btn_filter, 0, Qt::AlignCenter);
            tmp_vBox->addSpacing(btn_spacing);
        }
        //---------------------------------------------------



        // Total Widget (Option Group)
        QWidget *tmp_widget_optGroup = new QWidget();
        tmp_widget_optGroup->setLayout(tmp_vBox);
        tmp_widget_optGroup->setFixedWidth(WIDTH_OPTION_UNIT);

        this->hBox_filterBox_main->addWidget(tmp_widget_optGroup, 0, Qt::AlignTop);


        // 사이즈 업데이
        this->update_fixedSize_afterAddGroup(max_cnt);
    }


    /**
     * @brief BugsChooseFilterOpt::clearFilter
     */
    void BugsChooseFilterOpt::clearFilter(){
        // UI Clear
        GSCommon::clearLayout(this->hBox_filterBox_main);

        // Data Clear
        this->selected_optCode = "";
        this->cnt_optGroup = 0;
        this->max_optData = 0;
    }


    /**
     * @brief BugsChooseFilterOpt::showFilterBox
     */
    void BugsChooseFilterOpt::showFilterBox(const QPoint point_filterBox, const QSize size_parentSize){
        this->setVisible(true);
        this->setFixedSize(size_parentSize);

        this->update_fixedSize();
        this->widget_filterBox->move(point_filterBox);
    }


    /**
     * @brief BugsChooseFilterOpt::hideFilterBox
     */
    void BugsChooseFilterOpt::hideFilterBox(){
        this->hide();
        this->clearFilter();
        this->update_fixedSize();
    }




    /**
     * @brief BugsChooseFilterOpt::mousePressEvent
     * @param event
     */
    void BugsChooseFilterOpt::mousePressEvent(QMouseEvent *event){
        if(event->button() == Qt::LeftButton){
            if(!this->widget_filterBox->geometry().contains(event->pos())){
                // 필터링 박스 바깥쪽을 Left 마우스 클릭 >> 사용자가 선택을 하지 않았다고 보고, 팝업을 내림
                this->hide();
            }
        }
    }


    /**
     * @brief 현재 선택된 selected_optCode 반환
     * @return
     */
    QVariant BugsChooseFilterOpt::get_selectedCode(){
        return this->selected_optCode;
    }


    /**
     * @brief BugsChooseFilterOpt::slot_mousePressed_noData_widget
     */
    void BugsChooseFilterOpt::slot_mousePressed_noData_widget(const Qt::MouseButton p_button){
        if(p_button == Qt::LeftButton){
            this->hide();
        }
    }




    //--------------------------------------------------------------------------------------------------------------
    //
    // MARK : private functions
    //
    //--------------------------------------------------------------------------------------------------------------

    /**
     * @brief 헤더 라벨 생성 후 반환
     * @param headerText
     * @return
     */
    QLabel* BugsChooseFilterOpt::make_headerLabel(QString headerText){
        int height_label = HEIGHT_HEADER_LABEL___NORMAL;
        if(this->m_filterOptSize == FilterOptSize::Small){
            height_label = HEIGHT_HEADER_LABEL___SMALL;
        }

        QLabel *label_header = new QLabel();
        label_header->setText(headerText);
        label_header->setStyleSheet("color:#cccccc; font-size:14px; border-bottom:1px solid #4d4d4d;");
        label_header->setFixedSize(WIDTH_OPTION_UNIT, height_label);
        label_header->setAlignment(Qt::AlignCenter);
        return label_header;
    }


    /**
     * @brief 버튼 생성 후 반환
     * @param btnText
     * @param flagSelected
     * @return
     */
    QPushButton* BugsChooseFilterOpt::make_fiterButton(QString btnText, int width, int height, QString style_normal, QString style_selected, bool flagSelected){
        QPushButton *btn_filter = new QPushButton;
        btn_filter->setText(btnText);
        btn_filter->setFixedSize(width, height);
        btn_filter->setStyleSheet(flagSelected ? style_selected : style_normal);
        btn_filter->setCursor(Qt::PointingHandCursor);

        return btn_filter;
    }


    /**
     * @brief [Slot] 필터링 옵션 버튼 클릭에 대한 처리
     */
    void BugsChooseFilterOpt::slot_clicked_filterOptDataBtn(){
        this->selected_optCode = sender()->property(KEY_CODE);

        emit this->signal_clicked_filterOpt(this->selected_optCode, sender()->property(KEY_NAME).toString());

        // 바로 Hide 처리함
        this->hideFilterBox();
    }


    /**
     * @brief 필터박스 위젯의 사이즈를 업데이트 처리
     * @details 내부 멤버변수를 조정하고, 그것으로 사이즈 계산해서 fixed 처리함. @n
     * 항상 @ref addFilterGroup 함수 마지막에 호출. 내부적으로는 @ref update_fixedSize 함수 호출함
     */
    void BugsChooseFilterOpt::update_fixedSize_afterAddGroup(const int maxCnt_forAddGroup){

        // 멤버변수 업데이트
        this->cnt_optGroup += 1;            // 옵션이 하나 추가됨
        if(this->max_optData < maxCnt_forAddGroup){
            this->max_optData =maxCnt_forAddGroup;
        }

        this->update_fixedSize();
    }



    /**
     * @brief BugsChooseFilterOpt::update_fixedSize
     */
    QSize BugsChooseFilterOpt::update_fixedSize() const{

        int fixed_width = 0;            // Default Width
        int fixed_height = 0;           // Default Height

        if(this->cnt_optGroup==0 || this->max_optData==0){\
            // 옵션 그룹이 하나도 없는 경우
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::FilterOptionBox);
            connect(noData_widget, &NoData_Widget::signal_mousePressed, this, &BugsChooseFilterOpt::slot_mousePressed_noData_widget);

            this->hBox_filterBox_main->addWidget(noData_widget, 1, Qt::AlignCenter | Qt::AlignVCenter);
            fixed_width = 300;
            fixed_height = 100;
        }
        else{
            // Width
            fixed_width = this->cnt_optGroup * WIDTH_OPTION_UNIT;

            // 보기 쉽게 나눠서 계산 - Height
            if(this->m_filterOptSize == FilterOptSize::Small){
                fixed_height = HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___SMALL + HEIGHT_OPTION_BTN___SMALL + HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___SMALL;       // 옵션버튼 1개의 Height 영역
                fixed_height = fixed_height * this->max_optData;            // 옵션버튼의 max 개수로 multiple
                fixed_height += HEIGHT_HEADER_LABEL___SMALL;                        // 옵션그룹의 헤더의 높이
            }
            else{
                // default - nnormal;
                fixed_height = HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___NORMAL + HEIGHT_OPTION_BTN___NORMAL + HEIGHT_SPACING_OF_OPTION_BTN___TOP_BOTTOM___NORMAL;       // 옵션버튼 1개의 Height 영역
                fixed_height = fixed_height * this->max_optData;            // 옵션버튼의 max 개수로 multiple
                fixed_height += HEIGHT_HEADER_LABEL___NORMAL;                        // 옵션그룹의 헤더의 높이
            }
        }

        // update size of the Filtering Option Box
        QSize size_output(fixed_width, fixed_height);
        this->widget_filterBox->setFixedSize(size_output);

        return size_output;

    }

}





