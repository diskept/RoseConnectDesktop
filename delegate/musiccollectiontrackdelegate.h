#ifndef MUSICCOLLECTIONTRACKDELEGATE_H
#define MUSICCOLLECTIONTRACKDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 음악 마이컬렉션-트랙 delegate @n
 *      @sa FileDelegate Copy 수정
 */
class MusicCollectionTrackDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        playListAdd,
        more,
        etc
    };

    explicit MusicCollectionTrackDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:

    bool flagShowHeart= true;   /// 하트 show 유무
    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapMore;
    QPixmap pixmapPlay;
    QPixmap pixmapStarOn;
    QPixmap pixmapStarOff;
    QPixmap pixmapPlaylist;


};
#endif // MUSICCOLLECTIONTRACKDELEGATE_H
