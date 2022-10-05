#ifndef VIDEOMAIN_H
#define VIDEOMAIN_H

#include "video/videohome.h"
#include "video/videodetail.h"
#include "video/videosearch.h"
#include "video/videofolder.h"
#include "music/musicfolder.h"

#include "home/abstractmaincontent.h"

#include "widget/myqwidget.h"//c220730

#include <QWidget>


namespace video {

    /**
     * @brief 비디오 메인메뉴의 총괄 @n
     *      비디오 대항목 관련 모든 QWidget 관리
     */
    class VideoMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit VideoMain(QWidget *parent = nullptr);

    signals:


    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private slots:
        void slot_dragEnterEvent_restore();//c220826_1
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

        void slot_search(const QString &p_text);

    private :
        // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요

        QWidget *curr_widget;//c220730

        // 메뉴 관련 페이지  ----------------------
        VideoHome *videoHome = nullptr;             ///< 메뉴 : 비디오 > 홈
        VideoDetail *videoDetail = nullptr;         ///< 비디오 상세
        MusicFolder *videoFolder = nullptr;         ///< 메뉴 : 비디오 > 폴더
        // 비디오 검색
        VideoSearch *videoSearch=nullptr;



        void setInit();
        void setUIControl();

    };
};


#endif // VIDEOMAIN_H
