#ifndef BUGSTRACKDELEGATE_H
#define BUGSTRACKDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace bugs {

    /**
     * @brief Bugs의 Track 리스트 관련.
     */
    class BugsTrackDelegate : public tidal::AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit BugsTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    protected:
        bool flagShowRank;
    };

};

#endif // BUGSTRACKDELEGATE_H
