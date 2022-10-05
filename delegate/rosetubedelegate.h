#ifndef ROSETUBEDELEGATE_H
#define ROSETUBEDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>


/**
 * @brief 로즈튜브 비디오 delegate @n
 *      @sa FileDelegate Copy 수정
 * @note use 로즈튜브 플레이리스트 상세
 */
class RosetubeDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        playListAdd,
        trackHeart,
        more,
        etc
    };

    explicit RosetubeDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:

    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapPlaylist;
    QPixmap pixmapHeartOn;
    QPixmap pixmapHeartOff;
    QPixmap pixmapMore;
    QPixmap pixmapPlay;
};

#endif // ROSETUBEDELEGATE_H
