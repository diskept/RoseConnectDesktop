#ifndef MUSICMYCOLLECTION_H
#define MUSICMYCOLLECTION_H

#include "login/dialoglogin.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "tidal/PushBtn_withID.h"


namespace music {

    enum MusicMyCollectionPart{
        MyPlaylist,
        Favorites,
    };

    class MusicMyCollection : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicMyCollection(QWidget * parent = nullptr);
        ~MusicMyCollection();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    signals:
        void signal_clickedViewAll(QString p_viewAllMode);

    private slots:
        void slot_clickBtn_specificPart();

        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

    private:
        // 초기 UI화면 구성
        void setUIControl_notLogin();
        void setUIControl_logined();

        void setUIControl_btnH_part();

        QWidget* setUIControl_LoginBefore();

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QWidget *widget_logined;
        QWidget *widget_notLogin;

        QString page = "";

        bool list_drawing = false;
    };
};

#endif // MUSICMYCOLLECTION_H
