#ifndef ABSTRACTTIDALTRACKDELEGATE_H
#define ABSTRACTTIDALTRACKDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPainter>

namespace tidal {

    /**
     * @brief 일반 트랙 리스트 처리하는 abstract delegate 클래스 (Tidal, Bugs, Qobuz 파트에서 상속받아서 사용)
     */
    class AbstractTidalTrackDelegate : public QAbstractItemDelegate
    {
        Q_OBJECT
    public:
        enum ClickMode{
            AllBox = 1              ///< Hover 영역 전체 박스를 클릭
            , MoreBtn               ///< 더보기 버튼 클릭
            , FavBtn                ///< 좋아요(하트) 버튼 클릭
            , AddCollectionBtn      ///< My Collection 추가 버튼 클릭
            , MusicVideoBtn         ///< Bugs Music Video 버튼 클릭
        };
        Q_ENUM(ClickMode);

        const int BTN_ICON_WIDTH = 60;
        const int SPACE = 20;                   ///< 아이템 간의 간격

        enum UseMode_Flag{
            More = 0x0001
            , Favorite = 0x0002
            , AddCollection = 0x0004

            , TopRank = 0x0008

            , Tidal_Track_General = More | AddCollection
            , Tidal_Track_Mine = More                           ///< My Created Playlist 에서는 AddCollection 사용하지 않음

            , Bugs_Track_General = More | Favorite
            , Bugs_Track_withRank = More | Favorite | TopRank
            , Bugs_Artist_General = More | Favorite

            ,Qobuz_Track_General = More | AddCollection
            ,Qobuz_Track_Mine = More

        };
        Q_DECLARE_FLAGS(UseMode, UseMode_Flag);

        explicit AbstractTidalTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);

        void isVideo_bugs(bool flag);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        void bugs_paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, bool flag) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        int section();


    signals:
        void clicked(const int index, ClickMode clickMode);


    protected:
        QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;

        int m_section;            ///< 화면에서 section을 가지고 있음. 화면에 어떤 Section Layout에 있는지의 값. click 이벤트에 대해서 owner object에게 알려줌.
        UseMode m_useMose;

        // data
        bool flagShowBtn_addCollection;         ///< 사용할지 말지의 여부 (Add My Collection 버튼). 기본값은 true
        bool flagShowBtn_like;                  ///< 좋아요 버튼 사용할지 말지의 여부. 기본값은 false. (Tidal에선 false. Bugs에선 true)
        bool flagShowBtn_bugsVideo[1000];       ///< Bugs Music Video 용

        int fixed_whole_width;                  ///< 고정 위젯들과 그 위젯들간의 space 통합


        // 자식 클래스에서 제어 필요함 (data 연결됨)
        QPixmap pixmap_fav_on;
        QPixmap pixmap_fav_off;

        int pos_more_x1;
        int pos_more_x2;
        int pos_fav_x1;
        int pos_fav_x2;
        int pos_addCollection_x1;
        int pos_addCollection_x2;

        QPixmap pixmap_video;

        int pos_video_x1;
        int pos_video_x2;


    private:
        void setInit();
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
        void setBugsVideo(const bool flag) const;

        QPixmap pixmap_addCollection;
        QPixmap pixmap_more;

    };

};

#endif // ABSTRACTTIDALTRACKDELEGATE_H
