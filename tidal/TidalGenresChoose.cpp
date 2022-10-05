#include "TidalGenresChoose.h"

#include <QLabel>
#include <QPushButton>
#include <QVariant>
#include <QScrollArea>
#include <QtDebug>

#include <widget/flowlayout.h>
#include "ProcCommon.h"
#include "common/gscommon.h"

namespace tidal {

    /**
     * @brief "TIDAL > 탐색 > Genre View as List" 화면을 위한 생성자. @see PAGECODE_T_EXPLORE_GENRES_PLAYLIST_VIEWALL
     * @param parent
     */
    TidalGenresChoose::TidalGenresChoose(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll, parent) {
        this->addUIControl_mainTitle("Genres");

        applyUIControl_chooseGenres_fromlistData();
    }


    /**
     * @brief 소멸자.
     */
    TidalGenresChoose::~TidalGenresChoose(){

        this->deleteLater();
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     */
    void TidalGenresChoose::setActivePage(){
        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 장르 정보를 화면에 출력
     */
    void TidalGenresChoose::applyUIControl_chooseGenres_fromlistData(){

        FlowLayout *flowLayout = new FlowLayout(0,20,0);
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        int dataSize = ProcCommon::getCount_listGenreInfo();
        for(int i=0 ; i<dataSize ; i++){
            QWidget *tmp_widget = this->get_genrePushButton(i, ProcCommon::getGenreInfo(i));
            flowLayout->addWidget(tmp_widget);
        }


        QWidget *widget_flow = new QWidget();
        widget_flow->setContentsMargins(0,5,33,10);
        widget_flow->setLayout(flowLayout);

        this->box_contents->addWidget(widget_flow);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }



    /**
     * @brief 장르 선택하는 버튼을 포함한 QWidget 생성 및 세팅 완료후 반환. FlowLayout 에 세팅하기 위해.
     * @param p_index
     * @param p_genreInfo
     * @return QWidget
     */
    QWidget* TidalGenresChoose::get_genrePushButton(const int p_index, const tidal::GenreInfo& p_genreInfo){

        //-------------------------------------------------------------------------------------------------------- 버튼 START

        QLabel *label_text = new QLabel(p_genreInfo.name);
        label_text->setContentsMargins(0,0,0,0);
        label_text->setStyleSheet(".QLabel { background-color:transparent; color:#ffffff; font-size:18px;}");

        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addWidget(label_text, 0, Qt::AlignVCenter);

        QPushButton *btn_clickable = new QPushButton();
        btn_clickable->setContentsMargins(34,0,0,0);
        btn_clickable->setLayout(vBox);
        btn_clickable->setFixedSize(785, 70);
        btn_clickable->setProperty("idx", p_index);
        btn_clickable->setCursor(Qt::PointingHandCursor);
        btn_clickable->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");
        connect(btn_clickable, SIGNAL(clicked()), this, SLOT(slot_clickGenreBtn()));

        //-------------------------------------------------------------------------------------------------------- 버튼 END


        QWidget *widget_line = new QWidget();
        widget_line->setContentsMargins(0,0,0,0);
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#4d4d4d;");

        QVBoxLayout *vBox_output = new QVBoxLayout();
        vBox_output->setSpacing(0);
        vBox_output->setContentsMargins(0,0,0,0);
        vBox_output->addWidget(btn_clickable, 0, Qt::AlignVCenter);
        vBox_output->addWidget(widget_line);


        QWidget *widget_output = new QWidget();
        widget_output->setContentsMargins(0,0,0,0);
        widget_output->setLayout(vBox_output);

        return widget_output;

    }


    /**
     * @brief 세부 장르버튼 클릭 - 해당 장르 메인화면으로 이동한다.
     */
    void TidalGenresChoose::slot_clickGenreBtn(){

        int idx = sender()->property("idx").toInt();
        tidal::GenreInfo tmp_option = ProcCommon::getGenreInfo(idx);

        QJsonObject tmp_data;
        tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_GENRES_MAIN);
        tmp_data.insert("name", tmp_option.name);
        tmp_data.insert("path", tmp_option.path);

        // 리시버에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedMovePage(tmp_data);

    }



}
