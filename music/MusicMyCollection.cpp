#include "music/MusicMyCollection.h"

#include "common/global.h"
#include "common/gscommon.h"


namespace music {

    MusicMyCollection::MusicMyCollection(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_rosefilter, parent){

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
    }


    MusicMyCollection::~MusicMyCollection(){

        this->deleteLater();
    }


    void MusicMyCollection::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;
        }
    }


    void MusicMyCollection::setActivePage(){

        if(this->flagNeedReload){

            GSCommon::clearLayout(this->box_contents);

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

            if(global.user.isValid()){
                this->setUIControl_logined();
            }
            else{
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
     * @brief  사용자가 타이달 로그인했을 경우의 화면을 위한 Widget
     */
    void MusicMyCollection::setUIControl_logined(){

        // 기본 UI 세팅
        if(!this->list_drawing)
        {
            this->setUIControl_btnH_part();
            this->list_drawing = true;
        }
    }


    /**
     * @brief QobuzMycollection::setUIControl_notLogin
     */
    void MusicMyCollection::setUIControl_notLogin(){

        this->widget_notLogin = this->setUIControl_LoginBefore();
        this->box_contents->addWidget(this->widget_notLogin);
    }


    /**
     * @brief My Collection 가로 버튼을 추가한다.
     */
    void MusicMyCollection::setUIControl_btnH_part(){

        QList<QString> list_img = QList<QString>{ ":/images/qobuz/qobuz_myp_ico.png", ":/images/qobuz/bugs_fav_ico.png"};
        QList<QString> list_name = QList<QString>{ tr("My Playlists"), tr("Favorites")};
        QList<MusicMyCollectionPart> list_code = QList<MusicMyCollectionPart>{ MyPlaylist, Favorites};

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

            this->box_contents->addWidget(widget_hBtn);
        }

        this->addSpacing_forNewPart();
    }


    QWidget* MusicMyCollection::setUIControl_LoginBefore(){

        QWidget *widget_loginBefore = new QWidget();
        widget_loginBefore->setObjectName("widget_loginBefore");
        widget_loginBefore->setFixedHeight(500);
        widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");

        int left, top = 0;
        QLabel *lb_text = new QLabel(widget_loginBefore);
        lb_text->setText(tr("You can use various services through Login."));
        lb_text->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:30px;");

        left = (1500 - lb_text->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - lb_text->sizeHint().height()) / 2;
        lb_text->setGeometry(left, top, lb_text->sizeHint().width(), lb_text->sizeHint().height());

        QPushButton *btn_login = new QPushButton(widget_loginBefore);
        btn_login->setText(tr("Log-in"));
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
        btn_login->setFixedSize(230,55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);


        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &MusicMyCollection::slot_showLoginPage);

        return widget_loginBefore;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief NEW, TOP, Rising 버튼 클릭 - 각 파트별로 상세페이지 이동 처리
     */
    void MusicMyCollection::slot_clickBtn_specificPart(){

        MusicMyCollectionPart part = static_cast<MusicMyCollectionPart>(sender()->property("code").toInt());

        if(part == MusicMyCollectionPart::MyPlaylist){
            QJsonObject jsonObj;
            jsonObj.insert("pathTitle", tr("My Playlists"));
            jsonObj.insert("api_subPath", "member/playlist");
            jsonObj.insert("type", "MyPlaylist");
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_MYPLAYLIST);

            emit linker->signal_clickedMovePage(jsonObj);
            //emit this->signal_clickedViewAll(PAGECODE_VA_MYPLAYLIST);
        }
        else if(part == MusicMyCollectionPart::Favorites){
            emit this->signal_clickedViewAll(PAGECODE_M_FAVORITE);
        }
    }


    void MusicMyCollection::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    void MusicMyCollection::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true){

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "my");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();
        }
    }


    void MusicMyCollection::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "my");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();
        }
    }
}
