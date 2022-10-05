#include "QobuzGenresChoose.h"

#include <QLabel>
#include <QPushButton>
#include <QVariant>
#include <QScrollArea>
#include <QtDebug>

#include <widget/flowlayout.h>
#include "ProcCommon_forQobuz.h"
#include "common/gscommon.h"

namespace qobuz {

    /**
     * @brief "TIDAL > 탐색 > Genre View as List" 화면을 위한 생성자. @see PAGECODE_T_EXPLORE_GENRES_PLAYLIST_VIEWALL
     * @param parent
     */
    QobuzGenresChoose::QobuzGenresChoose(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {
        this->addUIControl_mainTitle("Genres");

        applyUIControl_chooseGenres_fromlistData();
    }


    /**
     * @brief 소멸자.
     */
    QobuzGenresChoose::~QobuzGenresChoose(){

        this->deleteLater();
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     */
    void QobuzGenresChoose::setActivePage(){
        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 장르 정보를 화면에 출력
     */
    void QobuzGenresChoose::applyUIControl_chooseGenres_fromlistData(){

        FlowLayout *flowLayout = new FlowLayout(0,20,0);
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        int dataSize = ProcCommon::getCount_listGenreInfo();
        for(int i=0 ; i<dataSize ; i++){
            QWidget *tmp_widget = this->get_genrePushButton(i, ProcCommon::getGenreInfo(i));
            flowLayout->addWidget(tmp_widget);
        }


        QWidget *widget_flow = new QWidget();
        widget_flow->setContentsMargins(0, 5, 33, 10);
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
    QWidget* QobuzGenresChoose::get_genrePushButton(const int p_index, const qobuz::GenreInfo& p_genreInfo){

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
    void QobuzGenresChoose::slot_clickGenreBtn(){

        int idx = sender()->property("idx").toInt();
        qobuz::GenreInfo tmp_option = ProcCommon::getGenreInfo(idx);

        QJsonObject tmp_data;
        switch(tmp_option.id)
        {
            case 112:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_POP);
                break;

            case 80:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_JAZZ);
                break;

            case 10:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_CLASSIC);
                break;

            case 64:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_ELECTRO);
                break;

            case 127:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_SOUL);
                break;

            case 5:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_FOLK);
                break;

            case 133:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_HIPHOP);
                break;

            case 4:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_COUNTRY);
                break;

            case 116:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_METAL);
                break;

            case 3:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_BLUES);
                break;

            case 149:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_LATIN);
                break;

            case 91:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_SOUNDTRACKS);
                break;

            case 94:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_WORLD);
                break;

            case 59:
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_OTHER);
                break;
        }

        int genre_cnt = ProcCommon::getCount_listGenreInfo();
        QString genre_ids = "";

        for(int i = 0; i < genre_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                if(genre_ids.isEmpty()){
                    genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(i).id);
                }
                else{
                    genre_ids += QString(", %1").arg(ProcCommon::getGenreInfo(i).id);
                }
            }
        }

        //tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_EXPLORE_GENRES_MAIN);
        tmp_data.insert("name", tmp_option.name);        
        tmp_data.insert("path", tmp_option.path);
        tmp_data.insert("genre_id", genre_ids);
        tmp_data.insert("genre_cnt", QString("%1").number(genre_cnt));

        // 리시버에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedMovePage(tmp_data);

    }



}
