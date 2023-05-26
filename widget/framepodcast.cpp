#include "framepodcast.h"


FramePodcast::FramePodcast(QWidget *parent)
    : FrameWidget(Type::PodCast, parent)
{
    setInit();
    setUIControl();
}

FramePodcast::~FramePodcast(){

}

/**
 * @brief FramePodcast::setInit : 초기 세팅
 */
void FramePodcast::setInit(){

    linker = Linker::getInstance();
    data = new Data_PC_Channel(this);
}

/**
 * @brief FramePodcast::setUIControl : UI 세팅
 */
void FramePodcast::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    QWidget *label_base = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(ImageLabel::Type::Podcast);
    this->lb_img->setHidden_more(true);
    this->lb_img->setHidden_play(true);
    this->lb_img->setHidden_favorite(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS * 3;
    label_base->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 2.1;text-align: left;color:#FFFFFF;");
    this->lb_title->setFixedWidth(IMG_WIDTH);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    this->lb_artist = new QLabel(label_base);
    this->lb_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_artist->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 1.88;text-align: left;color:#999999;");
    this->lb_artist->setFixedWidth(IMG_WIDTH);
    this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);


    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(0,0,0,0);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    boxLayout->addSpacing(10);
    //boxLayout->addWidget(lb_title);
    //boxLayout->addWidget(lb_artist);
    boxLayout->addWidget(label_base);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}

/**
 * @brief FramePodcast::setHover : [override] 호버 세팅
 */
void FramePodcast::setHover(){

    lb_img->setHover();
}

/**
 * @brief FramePodcast::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FramePodcast::getDataForPopup(){

    /*
    QJsonArray jsonArrAlbum;
    jsonArrAlbum.append(dataPopup->getJsonData());

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert("isAlbum", true);

    */
    return QJsonObject();
}

/**
 * @brief FramePodcast::setData : [override] 데이터 세팅
 * @param p_jsonObject QJsonObject
 */
void FramePodcast::setData(const QJsonObject &p_jsonObject){

    data->setData(p_jsonObject);
}

/**
 * @brief FramePodcast::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 * @note unused , Hover Item 디자인및 기능정의 없음.
 */
void FramePodcast::slot_clickedHover(const QString &p_code){

    Q_UNUSED(p_code);
    //emit linker->signal_clickedHoverItem(p_code, getDataForPopup());
}


/**
 * @brief FramePodcast::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 팟캐스터 채널 상세 페이지로 이동
 */
void FramePodcast::slot_clickedItem(){

    QJsonObject jsonData;
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_PC_SUB_CHANNEL);
    jsonData.insert(KEY_DATA, data->getData());

    emit linker->signal_clickedMovePage(jsonData);
}


void FramePodcast::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        if(!data->getArtworkUrl600().isEmpty()){
            this->lb_img->setImageUrl(data->getArtworkUrl600());
        }

        int all_width = 0;
        all_width = this->lb_img->sizeHint().width();

        QString title = data->getCollectionName();

        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
        tmp_wordwrap->setText(title);

        int title_width = tmp_wordwrap->sizeHint().width();

        if(title_width > all_width){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_title_line1 = "";

            tmp_split = title;
            splitToken = tmp_split.split(" ");

            qDebug() << "splitToken : "<< splitToken;

            tmp_wordwrap->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_title_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_title_line1 += " " + splitToken.at(i);
                    }
                    tmp_wordwrap->setText(tmp_title_line1);

                    qDebug() << "tmp_wordwrap->text() : " << tmp_wordwrap->text();
                    if(tmp_wordwrap->sizeHint().width() > all_width){
                        tmp_wordwrap->setText("");
                        tmp_title_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }
            tmp_wordwrap->setText("");
            tmp_wordwrap->setText(tmp_title_line1);

            title_width = tmp_wordwrap->sizeHint().width() + all_width;

            this->lb_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT * 2);
            this->lb_title->setFixedWidth(all_width);
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->lb_title->font()));
            if(this->lb_title->text().contains("…")){
                this->lb_title->setToolTip(title);//c230321
                this->lb_title->setToolTipDuration(2000);//c230321
            }

            this->lb_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
        }
        else{
            this->lb_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(title);

            this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
        }

        if(data->getArtistName() != ""){
            this->lb_artist->setText(GSCommon::getTextCutFromLabelWidth(data->getArtistName(), all_width, this->lb_artist->font()));
            this->lb_artist->show();
            if(this->lb_artist->text().contains("…")){
                this->lb_artist->setToolTip(data->getArtistName());//c230321
                this->lb_artist->setToolTipDuration(2000);//c230321
            }
        }

    }
}



