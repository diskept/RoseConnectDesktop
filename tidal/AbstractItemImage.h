#ifndef ABSTRACTITEMIMAGE_H
#define ABSTRACTITEMIMAGE_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "common/filedownloader.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief @sa AbstractItem 의 자식클래스에서 사용하는 ItemImage 들에 대한 Abstract Class
     */
    class AbstractItemImage : public QWidget
    {
        Q_OBJECT
    public:

        /**
         * @brief 좋아요, 재생, 더보기 등의 버튼 클릭 모드
         */
        enum BtnClickMode{
            Favorite_toAdd
            , Favorite_toAddx2
            , Favorite_toAddx3
            , Favorite_toDelete
            , Play_RightNow
            , More_showPopupMenu
            , AllBox
        };
        Q_ENUM(BtnClickMode);


        explicit AbstractItemImage(QWidget *parent = nullptr);
        ~AbstractItemImage();

        void setImage(QString imagePath);
        void setTypeImage(QString imagePath);
        void setTypeCache(QString imagePath);
        void setCacheShow(bool flag);
        void setTypeMime(QString mime);
        void setTypeDuration(QString duration);
        void setTypeOnAir(QString imagePath);
        void setFavorite(bool flagFavorite);                    // 좋아요 초기값 설정

        void setUseBtn_play(bool flagUse);                      // protected -> public 으로 옮김. (Bugs > My Album 에서 제어 필요)
        void setUseBtn_more(bool flagUse);                      // protected -> public 으로 옮김. (Bugs > My Album 에서 제어 필요)
        void setUseBtn_favoriteRT(bool flagUse);               // protected -> public 으로 옮김. (Bugs > My Album 에서 제어 필요)
        void setUseBtn_favoriteRose(bool flagUse, int favCnt);  // protected -> public 으로 옮김. (Bugs > My Album 에서 제어 필요)

        QSize sizeHint() const override;

    signals:
        void signal_clickedBtn(BtnClickMode);

    protected:
        // Data
        int fixed_imgWidth;
        int fixed_imgHeight;
        int fixed_cornerRadius;

        bool flagFavorite;
        bool flagUseFavorite;
        bool flagUseFavoriteRose;

        bool flagUseHover;
        bool flagUseHover2;
        bool flagUseHoverRose;

        bool flagArtist = false;

        int UseFavoriteRoseCnt;

        QLabel *label_imageBig;         ///< 본 이미지
        QLabel *label_imageType;
        QLabel *label_imageCache;
        QLabel *label_imageMime;
        QLabel *label_imageDuration;
        QLabel *label_imageOnAir;
        QLabel *label_topListBG;        ///< 이미지 상단의 여러겹 이미지
        bool flagUseTopBG;

        void setUIControl();
        void setUIControl_forQobuz();
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;


        void show_topListBG(bool flagShow);


    protected slots:
        void slot_clickedBtnItem();
        void slot_fileDownload_loadImage();
        virtual QString getImagePath_default();

        void setUseHover(bool flagUse);
        void setUseHover2(bool flagUse);
        void setUseHoverRose(bool flagUse);
        void setUseBtn_favorite(bool flagUse);

        void setUseArtist(bool flagUse);


//        void setUseBtn_allBox(bool flagUse);


    private:

        QString getStr_btnStyle(const int index, QString btn_imgPath, QString btn_imgPath_forPress);
        QString getStr_hovorBtnStyle(const QString objName);
        void applyHoverOn();
        void applyHoverOff();
        void applyHoverOn2();
        void applyHoverOff2();
        void applyHoverOnRose();
        void applyHoverOffRose();

        void paint_imageBig(QPixmap &pixmap);


        // UI
        QPushButton *btn_play;
        QPushButton *btn_fav_toAdd;
        QPushButton *btn_fav_toAddx2;
        QPushButton *btn_fav_toAddx3;
        QPushButton *btn_fav_toDelete;
        QPushButton *btn_more;

        QPushButton *btn_allBox;
        QVBoxLayout *vBoxLayout_total;


        FileDownloader *fileDownLoader;

    private:
        bool flag_type_image = false;
        bool flag_type_cache = false;
        bool flag_cache_show = false;
        bool flag_type_mime = false;
        bool flag_type_duration = false;
        bool flag_type_onAir = false;

        QString type_image_path = "";
        QString type_image_cache = "";
        QString type_image_mime = "";
        QString type_image_duration = "";
        QString type_image_onAir = "";
    };
};

#endif // ABSTRACTITEMIMAGE_H
