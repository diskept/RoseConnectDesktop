#ifndef ROSETUBERELATIONDELEGATE_H
#define ROSETUBERELATIONDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 로즈튜브 연관컨텐츠 delegate @n
 *      @sa FileDelegate Copy 수정
 * @note @sa PlayFullScreenRelation 전체재생화면에서 사용
 */
class RoseTubeRelationDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        trackHeart,
        more,
        etc
    };

    explicit RoseTubeRelationDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:

    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapHeartOn;
    QPixmap pixmapHeartOff;
    QPixmap pixmapMore;
    QPixmap pixmapPlay;
};

#endif // ROSETUBERELATIONDELEGATE_H
