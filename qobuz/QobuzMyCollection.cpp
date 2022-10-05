#include "qobuz/QobuzMyCollection.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "common/global.h"
#include "common/gscommon.h"


namespace qobuz {

    /**
     * @brief "TIDAL > My Collection" 관련 화면
     * @param parent
     */
    QobuzMycollection::QobuzMycollection(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {

        // 코부즈 로그인 여부에 따라서
        if(this->is_qobuz_logined()){
            this->setUIControl_logined();
        }
        else{
            this->setUIControl_notLogin();
        }
    }


    /**
     * @brief 소멸자.
     */
    QobuzMycollection::~QobuzMycollection(){

        this->deleteLater();
    }


    /**
     * @brief 화면 활성화
     */
    void QobuzMycollection::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        // 타이달 로그인 여부에 따라서
        if(this->is_qobuz_logined()){
            // 로그인 되어 있음            
            this->setUIControl_logined();
        }
        else{
            // 로그인 안되어 있음
            this->setUIControl_notLogin();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief  사용자가 타이달 로그인했을 경우의 화면을 위한 Widget
     */
    void QobuzMycollection::setUIControl_logined(){

        // 기본 UI 세팅        
        if(!this->list_drawing)
        {
            setUIControl_btnH_part();
            this->list_drawing = true;
        }
    }


    /**
     * @brief QobuzMycollection::setUIControl_notLogin
     */
    void QobuzMycollection::setUIControl_notLogin(){

        this->widget_notLogin = this->get_createUIControl_notLoginView();
        this->box_contents->addWidget(this->widget_notLogin, 1, Qt::AlignCenter | Qt::AlignVCenter);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief My Collection 가로 버튼을 추가한다.
     */
    void QobuzMycollection::setUIControl_btnH_part(){

        QList<QString> list_img = QList<QString>{ ":/images/qobuz/qobuz_myp_ico.png", ":/images/qobuz/bugs_fav_ico.png", ":/images/qobuz/Purchases_ico.png"};   //, ":/images/qobuz/qobuz_set_ico.png" };
        QList<QString> list_name = QList<QString>{ tr("My Playlists"), tr("Favorites"), tr("My Purchases")};    //, tr("Settings") };
        QList<QobuzMyCollectionPart> list_code = QList<QobuzMyCollectionPart>{ MyPlaylist, Favorites, MyPunchases}; //, Settings };

        for(int i = 0; i < list_img.count(); i++){

            //---------------------------------------------------------------------------------------- 버튼 영역 START

            QLabel *label_image = GSCommon::getUILabelImg(list_img.at(i));
            QLabel *label_text = new QLabel(list_name.at(i));
            label_text->setStyleSheet("font-size:18px; color:#FFFFFF;");

            QHBoxLayout *tmp_hBox_new = new QHBoxLayout();
            tmp_hBox_new->setContentsMargins(0,0,0,0);
            tmp_hBox_new->setSpacing(0);
            tmp_hBox_new->setAlignment(Qt::AlignLeft);
            tmp_hBox_new->addWidget(label_image);
            tmp_hBox_new->addSpacing(30);
            tmp_hBox_new->addWidget(label_text, 1, Qt::AlignVCenter | Qt::AlignLeft);

            QPushButton *btn_clickableBox = new QPushButton();
            btn_clickableBox->setContentsMargins(10,16,10,9);
            btn_clickableBox->setProperty("code", list_code.at(i));
            btn_clickableBox->setLayout(tmp_hBox_new);
            btn_clickableBox->setStyleSheet("QLabel{ background-color:transparent; } QPushButton:hover { background-color:#4d4d4d; } ");
            btn_clickableBox->setFixedHeight(70);
            btn_clickableBox->setCursor(Qt::PointingHandCursor);
            connect(btn_clickableBox, SIGNAL(clicked()), this, SLOT(slot_clickBtn_specificPart()));

            //---------------------------------------------------------------------------------------- 버튼 영역 END


            // 버튼 하단에 라인 넣기
            QWidget *widget_lineB = new QWidget();
            widget_lineB->setStyleSheet("background-color:#333333;");
            widget_lineB->setFixedHeight(1);

            QVBoxLayout *tmp_vBox_new = new QVBoxLayout();
            tmp_vBox_new->setContentsMargins(0,0,30,0);
            tmp_vBox_new->setSpacing(0);
            tmp_vBox_new->addWidget(btn_clickableBox);
            tmp_vBox_new->addWidget(widget_lineB);


            QWidget *widget_hBtn = new QWidget();
            widget_hBtn->setLayout(tmp_vBox_new);
            widget_hBtn->setContentsMargins(0,0,0,0);

            box_contents->addWidget(widget_hBtn);

        }

        this->addSpacing_forNewPart();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 타이달 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    void QobuzMycollection::slot_acceptedDialogLogin(){
        // 부모 함수 호출
        AbstractQobuzSubWidget::slot_acceptedDialogLogin();

        // 로그인한 UI로 변경함
        this->setActivePage();
    }


    /**
     * @brief NEW, TOP, Rising 버튼 클릭 - 각 파트별로 상세페이지 이동 처리
     */
    void QobuzMycollection::slot_clickBtn_specificPart(){

        QobuzMyCollectionPart part = static_cast<QobuzMyCollectionPart>(sender()->property("code").toInt());

        if(part == QobuzMyCollectionPart::MyPlaylist){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_COLLECTION_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }
        else if(part == QobuzMyCollectionPart::Favorites){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_COLLECTION_FAVORITES);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }
        else if(part == QobuzMyCollectionPart::MyPunchases){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_COLLECTION_PURCHASES);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }
        /*else if(part == QobuzMyCollectionPart::Settings){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_SETTINGS);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }*/
    }

}
