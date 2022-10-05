#ifndef MUSICMAIN_2_H
#define MUSICMAIN_2_H


#include "home/abstractmaincontent.h"

// 서브 페이지
#include "music_2/MusicHome_2.h"
#include "music_2/MusicAlbumList.h"



namespace music {

    class MusicMain_2 : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit MusicMain_2(QWidget *parent = nullptr);

    signals:

    private slots:
//        void slot_clickedViewAll(QString p_viewAllMode);
//        void slot_clickedFavoriteFolderEdit(const QJsonArray &p_jsonArrFavor);

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    private :

        // 메뉴 관련 페이지  ----------------------
        MusicHome_2 *sub_home;
        MusicAlbumList *sub_albumList;

        void setUIControl();

    };

};

#endif // MUSICMAIN_2_H
