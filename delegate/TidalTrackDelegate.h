#ifndef TIDALTRACKDELEGATE_H
#define TIDALTRACKDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace tidal {

    /**
     * @brief 일반 트랙 리스트 관련. 전체 트랙목록 (All View 에서). 아니면 PlayList의 트랙목록.
     * 트랙 아이템에는 image 부분이 포함되어 있음
     */
    class TidalTrackDelegate : public AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit TidalTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QPixmap pixmap_mqa;
    };

};

#endif // TIDALTRACKDELEGATE_H
