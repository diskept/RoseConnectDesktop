#include "PDAlbumImageDetailInfo.h"
#include "tidal/ProcCommon.h"
#include "common/ProcJsonEasy.h"
#include "bugs/ConvertData_forBugs.h"

namespace bugs{

    /**
     * @brief 생성자
     * @param parent
     */
    PDAlbumImageDetailInfo::PDAlbumImageDetailInfo(QWidget *parent) : AbstractImageDetailInfo(parent){
        this->setImageSizeAndCorner(300, 300, 4);

        this->btn_addCollect->setVisible(false);

        // 추가적으로 필요한 UI 구성
        this->setUIControl_allTrack_info();
        this->setUIControl_tagList();


        // init
        this->initView();
    }


    /**
     * @brief UI 구성 - 전체 트랙에 대한 정보 표현
     */
    void PDAlbumImageDetailInfo::setUIControl_allTrack_info(){
        this->label_trackAll_info = new QLabel();
        this->label_trackAll_info->setStyleSheet("color:#777777; font-size:16px;");
        this->label_trackAll_info->setContentsMargins(30,5,0,10);

        this->vBox_albumInfo->insertWidget(3, this->label_trackAll_info, 1, Qt::AlignLeft | Qt::AlignBottom);
    }


    /**
     * @brief
     */
    void PDAlbumImageDetailInfo::setUIControl_tagList(){
        this->tagListWidget = new TagListWidget();
        this->tagListWidget->setContentsMargins(30,0,0,0);
        this->tagListWidget->setObjectName("tagListWidget");
        this->tagListWidget->setStyleSheet("#tagListWidget { background-color:#ffff00; border:2px solid #ff0000; }");
        connect(this->tagListWidget, &TagListWidget::signal_clickedTags, this, &PDAlbumImageDetailInfo::slot_tagBtnClicked);
        this->tagListWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        this->vBox_albumInfo->insertWidget(4, this->tagListWidget, 1, Qt::AlignBottom);
    }


    /**
     * @brief [Slot] 태그 버튼 클릭에 대한 처리 - signal_tagClicked 전달
     * @param tag_index
     */
    void PDAlbumImageDetailInfo::slot_tagBtnClicked(int tag_index){
        emit this->signal_tagBtnClicked(tag_index);
    }


    // public functions ---------

    /**
     * @brief PDAlbumImageDetailInfo::initView
     */
    void PDAlbumImageDetailInfo::initView(){
        this->setImage("");
        this->label_title->setText("");
        this->label_creatorName->setText("");

        this->elidedLabel_description->setVisible(false);
        this->label_artist->setVisible(false);

        this->setFavorite(false);

        // 추가 data & UI - PD's Album에 대한
        this->list_tag_id.clear();
        this->label_trackAll_info->setText("");
        this->tagListWidget->clear_tags();
    }



    /**
     * @brief Bugs 앨범 적용 관련
     * @param data_album
     */
    void PDAlbumImageDetailInfo::setData_fromBugsData(const bugs::PD_AlbumItemData &data_pd_album){
        //qDebug()<< "\n\n----------------------------------------------------------------------------data_pd_album.descr" <<data_pd_album.descr ;
        // Album 모드로 배치한다.
        this->label_title->setVisible(true);
        this->label_creatorName->setVisible(true);

        // 데이터를 UI에 세팅한다.
        this->setImage(data_pd_album.image);
        this->label_title->setText(data_pd_album.title);
        this->label_creatorName->setText(data_pd_album.pd_nickname);

        //this->label_descr->setText(data_pd_album.descr.chopped(1));//cheon210604
        //this->label_track_count->setText(QString("Tracks: %1").arg(data_pd_album.track_count));//cheon210604
        //this->label_upd_dt->setText(QString("%1").arg(data_pd_album.upd_dt).remove(10,18));//cheon210604

        this->label_upd_dt->setText(QString("%1").arg(data_pd_album.upd_dt).remove(10,18));//cheon210606

        //if(!data_pd_album.genre_nm.isEmpty())//cheon210605
        //this->label_descr->setText(QString("%1").arg(data_pd_album.genre_nm));//cheon210605 remove(10,18)

        this->widget_spaceWithTextAndBtn->setFixedHeight(30);           // 텍스트와 버튼아이콘 사이의 간격

        this->data_key = data_pd_album.es_album_id;
        this->setFavorite(data_pd_album.likes_yn);

        this->btn_addCollect->setVisible(false);


        // 태그 정보
        this->list_tag_id = data_pd_album.list_tag_id;
        this->tagListWidget->add_tag_list(data_pd_album.list_tag_nm);

    }


    /**
     * @brief list_track 의 Track 정보에 대해서, 전체적인 트랙정보를
     * @param list_track
     */
    void PDAlbumImageDetailInfo::setData_timeInfo(const QList<bugs::TrackItemData> &list_track){

        int num_track = list_track.count();
        if(num_track > 0){

            // 전체 seconds 구하기
            int total_sec = 0;
            for(int i=0 ; i<list_track.count() ; i++){
                QTime tmp_time = QTime::fromString(list_track.at(i).len, "m:ss");
                int tmp_sec = tmp_time.hour() * 3600 + tmp_time.minute() * 60 + tmp_time.second();
                total_sec += tmp_sec;
            }

            // seconds --> string
            QString total_len = "";
            if(total_sec >= 3600){
                total_len = QDateTime::fromTime_t(total_sec).toUTC().toString("hh:mm:ss");
                total_len.remove(QRegExp("^0"));
            }
            else{
                total_len = QDateTime::fromTime_t(total_sec).toUTC().toString("mm:ss");
                total_len.remove(QRegExp("^0"));
            }

            // Text Setting
            QString trackInfo = QString("총 %1개 트랙 · %2").arg(num_track).arg(total_len);
            this->label_trackAll_info->setVisible(true);
            this->label_trackAll_info->setText(trackInfo);
        }
        else{
            // 없으면, 가리기
            this->label_trackAll_info->setVisible(false);
        }


    }


}
