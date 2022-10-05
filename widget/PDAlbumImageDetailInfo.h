#ifndef PDALBUMIMAGEDETAILINFO_H
#define PDALBUMIMAGEDETAILINFO_H

#include "widget/AbstractImageDetailInfo.h"
#include "tidal/tidal_struct.h"
#include "bugs/bugs_struct.h"
#include "widget/TagListWidget.h"

namespace bugs {

    /**
     * @brief PD 앨범 상세뷰에서 상단에 이미지와 앨범 타이틀, 태그 리스트 등의 정보가 보이는 Widget
     * @details Bugs 에서 사용
     */
    class PDAlbumImageDetailInfo : public AbstractImageDetailInfo
    {
        Q_OBJECT
    public:
        explicit PDAlbumImageDetailInfo(QWidget *parent = nullptr);

        void initView() override;           // 기본

        void setData_fromBugsData(const bugs::PD_AlbumItemData &data_pd_album);           // Bugs의 정보로 세팅


        // PD's Album 에만 있는 public function
        void setData_timeInfo(const QList<bugs::TrackItemData> &list_track);

    signals:
        void signal_tagBtnClicked(int index_tag);

    private slots:
        void slot_tagBtnClicked(int tag_index);

    private:
        bool flagNeededInit;

        void setUIControl_allTrack_info();
        void setUIControl_tagList();

        QLabel *label_trackAll_info;            ///< 트랙에 대한 정보
        TagListWidget *tagListWidget;           ///< 태그 리스트 위젯

        QList<int> list_tag_id;

    };

};

#endif // PDALBUMIMAGEDETAILINFO_H
