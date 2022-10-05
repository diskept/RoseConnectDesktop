#ifndef MUSICALBUMLIST_2_H
#define MUSICALBUMLIST_2_H

#include "AbstractMusicSubWidget.h"

namespace music {

    class MusicAlbumList : public AbstractMusicSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicAlbumList(QWidget *parent = nullptr);

    signals:

    };

};


#endif // MUSICALBUMLIST_2_H
