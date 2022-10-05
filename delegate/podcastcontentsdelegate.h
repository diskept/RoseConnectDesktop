#ifndef PODCASTCONTENTSDELEGATE_H
#define PODCASTCONTENTSDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 팟캐스트 채널 컨텐츠 delegate @n
 *      @sa FileDelegate Copy 수정
 */
class PodcastContentsDelegate : public QAbstractItemDelegate
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

    explicit PodcastContentsDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:
    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapHeart;
    QPixmap pixmapMore;
    QPixmap pixmapFavor;
};

#endif // PODCASTCONTENTSDELEGATE_H
