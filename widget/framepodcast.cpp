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
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(IMG_WIDTH);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    this->lb_artist = new QLabel(label_base);
    this->lb_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_artist->setStyleSheet("font-size:16px;color:#999999;");
    this->lb_artist->setFixedWidth(IMG_WIDTH);
    this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);


    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(0,0,0,0);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
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

        int all_width = this->lb_img->sizeHint().width();
        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setText(data->getCollectionName());
        //qDebug() << tmp_wordwrap->sizeHint().width();

        if(tmp_wordwrap->sizeHint().width() > all_width){
            if(data->getCollectionName().contains(" ")){
                this->lb_title->setGeometry(0, (SPACE_LABELS * 2), all_width, (LABEL_HEIGHT *2));
                this->lb_artist->setGeometry(0, (SPACE_LABELS * 3) + (LABEL_HEIGHT *2), all_width, LABEL_HEIGHT);

                tmp_wordwrap->clear();
                tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
                tmp_wordwrap->setWordWrap(true);
                tmp_wordwrap->setText(GSCommon::getTextCutFromLabelWidth(data->getCollectionName(), (all_width * 2)-30, this->lb_title->font()));

                if(tmp_wordwrap->sizeHint().height() > this->LABEL_HEIGHT * 2){
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getCollectionName(), (all_width * 2)-80, this->lb_title->font()));
                }
                else{
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getCollectionName(), (all_width * 2)-30, this->lb_title->font()));
                }
            }
            else{
                this->lb_title->setGeometry(0, (SPACE_LABELS * 2), lb_img->sizeHint().width(), LABEL_HEIGHT);
                this->lb_artist->setGeometry(0, (SPACE_LABELS * 3) + LABEL_HEIGHT, lb_img->sizeHint().width(), LABEL_HEIGHT);
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getCollectionName(), lb_img->sizeHint().width(), lb_title->font()));//cheon-210708-album
            }
        }
        else if(tmp_wordwrap->sizeHint().width() == this->lb_img->sizeHint().width() - 5){
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
            this->lb_artist->setGeometry(0, (SPACE_LABELS * 3) + LABEL_HEIGHT, lb_img->sizeHint().width(), LABEL_HEIGHT);
            this->lb_title->setText(data->getCollectionName());
        }
        else {
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
            this->lb_artist->setGeometry(0, (SPACE_LABELS * 3) + LABEL_HEIGHT, lb_img->sizeHint().width(), LABEL_HEIGHT);
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getCollectionName(), IMG_WIDTH, lb_title->font()));//cheon-210708-album
        }

        lb_artist->setText(GSCommon::getTextCutFromLabelWidth(data->getArtistName(), IMG_WIDTH, lb_artist->font()));
    }
}



