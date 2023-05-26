#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Artist, Album, Track(Square), Playlist, PD's Album, My Album, Video... 등등의 Rose 음원 관련된 Item 들의 Abstract Class
     */
    class AbstractItem : public QWidget
    {
        Q_OBJECT

    public:
        enum ClickMode{
            AllBox = 1      ///< Hover 영역 전체 박스를 클릭
            , PlayBtn       ///< 재생버튼 클릭
            , MoreBtn       ///< 더보기 버튼 클릭
            , FavBtn_Add    ///< 좋아요(하트) 버튼 클릭
            , FavBtn_Addx2  ///< 좋아요(하트) 버튼 클릭
            , FavBtn_Addx3  ///< 좋아요(하트) 버튼 클릭
            , FavBtn_Delete ///< 좋아요(하트) 버튼 클릭
            , AddCollectionBtn
        };
        Q_ENUM(ClickMode);

        enum ImageSizeMode{
            Square_120x120
            , Square_200x200
            , Square_180x180
            , Square_186x186
            , Square_284x284
            , Ractangle_200x281
            , Ractangle_251x139
            , Ractangle_284x157
            , Ractangle_358x174
            , Ractangle_360x176
            , Ractangle_360x200
            , Ractangle_340x188
            , Ractangle_384x186
            , Ractangle_450x120
            , Ractangle_480x120
        };
        Q_ENUM(ImageSizeMode);


        explicit AbstractItem(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        int index();
        int section();

        virtual int get_fixedHeight() = 0;

    signals:
        void signal_clicked(ClickMode);

    protected:
        bool flagInitDraw = false;
        tidal::AbstractItemImage *itemImage;

        int m_index;          ///< 화면에서 index를 가지고 있음. 화면의 해당 Layout에 몇번째 위젯으로 보이고 있는지의 값. click 이벤트에 대해서 owner object에게 알려줌.
        int m_section;        ///< 화면에서 section을 가지고 있음. 화면에 어떤 Section Layout에 있는지의 값. click 이벤트에 대해서 owner object에게 알려줌.

        ImageSizeMode m_imageSizeMode;

        int get_imageWidth(ImageSizeMode& imageSizeMode);
        int get_imageHeight(ImageSizeMode& imageSizeMode);
        int get_spaceImageAndTitle(ImageSizeMode& imageSizeMode);


        const int LABEL_HEIGHT = 24;
        const int SPACE_LABELS = 3;
        const int ICON_HEIGHT = 30;

    protected slots:
        void slot_itemImage_clickedBtn(AbstractItemImage::BtnClickMode);

    };

};

#endif // ABSTRACTITEM_H
