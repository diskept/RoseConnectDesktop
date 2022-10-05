#ifndef MUSICDELEGATE_H
#define MUSICDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 음악 플레이리스트 상세 delegate @n
 *      @sa FileDelegate Copy 수정
 */
class MusicDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        playListAdd,
        trackHeart,
        more,
        folderFav,
        folderAdd,
        etc
    };

    explicit MusicDelegate(QObject *parent = nullptr);
    ~MusicDelegate();
    void setShowHeartIcon(bool p_flagShowHeart);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:

    bool flagShowHeart= true;   /// 하트 show 유무
    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapHeartOn;
    QPixmap pixmapHeartOff;
    QPixmap pixmapMore;
    QPixmap pixmapPlaylist;
    QPixmap pixmapPlay;


};


#endif // MUSICDELEGATE_H
