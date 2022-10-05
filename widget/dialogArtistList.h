#ifndef DIALOGARTISTLIST_H
#define DIALOGARTISTLIST_H

#include "common/linker.h"
#include "widget/flowlayout.h"
#include "widget/OptMorePopup.h"

#include "bugs/bugs_struct.h"
#include "qobuz/qobuz_struct.h"
#include "tidal/tidal_struct.h"

#include "tidal/AbstractItem.h"

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>


class DialogArtistList : public QDialog
{
    Q_OBJECT
public:
    explicit DialogArtistList(QWidget *parent = nullptr);
    ~DialogArtistList();

    void setData(const QJsonObject& jsonObj);

private slots:
    // Bugs
    void slot_applyResult_artistInfo_forBugs(const bugs::ArtistItemData &data_artist);
    void slot_clickedItemArtist_forBugs(const tidal::AbstractItem::ClickMode);

    // Tidal
    void slot_applyResult_artistInfo_forTidal(const tidal::ArtistItemData &data_artist);
    void slot_clickedItemArtist_forTidal(const tidal::AbstractItem::ClickMode);

    void slot_optMorePopup_menuClicked_forTidal(const OptMorePopup::ClickMode, const int, const int);               // about OptMorePopup

    // Qobuz
    void slot_applyResult_artistInfo_forQobuz(const qobuz::ArtistItemData &data_artist);
    void slot_clickedItemArtist_forQobuz(const tidal::AbstractItem::ClickMode);

    void slot_optMorePopup_menuClicked_forQobuz(const OptMorePopup::ClickMode, const int, const int);               // about OptMorePopup


private:
    void setInit();
    void setUIControl();

    int checkValid_index(int length_ofData, int index);

    // Bugs
    void proc_clicked_itemArtist_forBugs(bugs::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
    void proc_clicked_itemArtist_forBugs(QList<bugs::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

    void proc_preUpdate_favorite_artist_forBugs(const bugs::ItemPositionData&);

    // Tidal
    void proc_clicked_itemArtist_forTidal(tidal::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
    void proc_clicked_itemArtist_forTidal(QList<tidal::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

    void proc_clicked_optMorePopup_fromArtist_forTidal(tidal::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode);
    void proc_clicked_optMorePopup_fromArtist_forTidal(QList<tidal::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode);

    void makeObj_optMorePopup_forTidal(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index=0, const int section=0, const bool flagForceHide_favorite=false);

    // Qobuz
    void proc_clicked_itemArtist_forQobuz(qobuz::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
    void proc_clicked_itemArtist_forQobuz(QList<qobuz::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

    void proc_clicked_optMorePopup_fromArtist_forQobuz(qobuz::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode);
    void proc_clicked_optMorePopup_fromArtist_forQobuz(QList<qobuz::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode);

    void makeObj_optMorePopup_forQobuz(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index=0, const int section=0, const bool flagForceHide_favorite=false);


private:
    Linker *linker;

    QWidget *dlg_widget;
    QWidget *dlg_body_widget;
    QScrollArea *dlg_scrollArea;

    FlowLayout *dlg_layout;

    QLabel *lb_title;

    QList<int> list_artist_id;                                  // list Artists Parsing    
    QList<bugs::ArtistItemData> *list_artist_forBugs;           // list Artists of Bugs
    QList<tidal::ArtistItemData> *list_artist_forTidal;         // list Artists of Tidal
    QList<qobuz::ArtistItemData> *list_artist_forQobuz;         // list Artists of Tidal

};

#endif // DIALOGARTISTLIST_H
