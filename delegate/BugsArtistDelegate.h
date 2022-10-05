#ifndef BUGSARTISTDELEGATE_H
#define BUGSARTISTDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace bugs {

    /**
     * @brief Bugs의 Artist 리스트 관련.
     */
    class BugsArtistDelegate : public tidal::AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit BugsArtistDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    protected:
        bool flagShowRank;
    };

};
#endif // BUGSARTISTDELEGATE_H
