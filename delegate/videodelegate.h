#ifndef VIDEODELEGATE_H
#define VIDEODELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 비디오 delegate @n
 *      @sa FileDelegate Copy 수정
 *
 * @note
 *      use 비디오-상세 @n
 *      mod >> 하트, 더보기 옵션 미지원
 */
class VideoDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        trackHeart,
        more,
        etc
    };

    explicit VideoDelegate(QObject *parent = nullptr);

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
    QPixmap pixmapPlay;
};

#endif // VIDEODELEGATE_H
