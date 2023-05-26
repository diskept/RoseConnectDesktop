#include "NoData_Widget.h"

#include <QHBoxLayout>
#include <QMouseEvent>


/**
 * @brief NoData_Widget::NoData_Widget
 * @param parent
 */
NoData_Widget::NoData_Widget(const NoData_Message p_msgOpt, QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
    this->setText_forNoData(p_msgOpt);
}


/**
 * @brief NoData_Widget::setUIControl
 */
void NoData_Widget::setUIControl(){

    // label
    this->label_msg = new QLabel();
    this->label_msg->setStyleSheet("font-size:20px; color:#c7c7c7;");

    QHBoxLayout *tmp_hBox = new QHBoxLayout();
    tmp_hBox->setSpacing(0);
    tmp_hBox->setContentsMargins(0, 0, 0, 0);
    tmp_hBox->addWidget(this->label_msg, 1, Qt::AlignVCenter | Qt::AlignCenter);

    this->setLayout(tmp_hBox);
}



/**
 * @brief NoData_Widget::setText_forNoData
 * @param p_msgOpt
 */
void NoData_Widget::setText_forNoData(const NoData_Message p_msgOpt){

    switch(p_msgOpt){
        case NoData_Message::Unknown:
            this->label_msg->setText(tr("No data available."));
            break;

        case NoData_Message::FilterOptionBox:
            this->label_msg->setText(tr("No filtering options available to select."));
            break;

        case NoData_Message::Album_NoData:
            this->label_msg->setText(tr("No album information."));
            break;

        case NoData_Message::Playlist_NoData:
            this->label_msg->setText(tr("No playlist information."));
            break;

        case NoData_Message::Track_NoData:
            this->label_msg->setText(tr("No track information."));
            break;

        case NoData_Message::Artist_NoData:
            this->label_msg->setText(tr("No artist information."));
            break;

        case NoData_Message::PD_Album_NoData:
            this->label_msg->setText(tr("No music PD album information."));
            break;

        case NoData_Message::MyAlbum_NoData:
            this->label_msg->setText(tr("No Information on My Album."));
            break;

        case NoData_Message::MyPlaylist_NoData:
            this->label_msg->setText(tr("No Information on My Playlist."));
            break;

        case NoData_Message::Video_NoData:
            this->label_msg->setText(tr("No video information."));
            break;

        case NoData_Message::Rosetube_NoData:
            this->label_msg->setText(tr("No rosetube information."));
            break;

        case NoData_Message::Subscription_NoData:
            this->label_msg->setText(tr("No rosetube subscription information."));
            break;

        case NoData_Message::SearchResult_NoData:
            this->label_msg->setText(tr("No results were found for your search."));
            break;

        case NoData_Message::CDplay_NoData://c220721
            this->label_msg->setText(tr("List is empty."));
            break;

        case NoData_Message::Radio_NoData:
            this->label_msg->setText(tr("No rose radio channel information."));
            break;

        case NoData_Message::History_NoDate:
            this->label_msg->setText(tr("No Information on My History."));
            break;

/*
 *         case NoData_Message::Unknown:
            this->label_msg->setText(tr("데이터가 없습니다."));
            break;

        case NoData_Message::FilterOptionBox:
            this->label_msg->setText(tr("선택할 수 있는 필터링 옵션이 없습니다."));
            break;

        case NoData_Message::Track_NoData:
            this->label_msg->setText(tr("트랙 정보가 없습니다."));
            break;

        case NoData_Message::Album_NoData:
            this->label_msg->setText(tr("앨범 정보가 없습니다."));
            break;

        case NoData_Message::Artist_NoData:
            this->label_msg->setText(tr("아티스트 정보가 없습니다."));
            break;

        case NoData_Message::PD_Album_NoData:
            this->label_msg->setText(tr("뮤직PD앨범 정보가 없습니다."));
            break;

        case NoData_Message::MyAlbum_NoData:
            this->label_msg->setText(tr("내 앨범 정보가 없습니다."));
            break;

        case NoData_Message::Video_NoData:
            this->label_msg->setText(tr("영상 정보가 없습니다."));
            break;

        case NoData_Message::Playlist_NoData:
            this->label_msg->setText(tr("Playlist 정보가 없습니다."));
            break;

            */
    }
}


/**
 * @brief 마우스 press 되면 이벤트 발생
 * @param event
 */
void NoData_Widget::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);
    emit this->signal_mousePressed(event->button());
}
