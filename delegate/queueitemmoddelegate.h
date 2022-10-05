#ifndef QUEUEITEMMODDELEGATE_H
#define QUEUEITEMMODDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

namespace QueueItemModDelegate_C {
    const QString KEY_INDEX = "d_index";      ///< 인덱스
    const QString KEY_TYPE = "d_itemType";      ///< 큐 아이템의 유형 (음악 인지 로즈튜브 인지)
    const QString KEY_TITLE = "d_title";      ///< 제목
    const QString KEY_ARTIST = "d_artist";    ///< 아티스트명
    const QString KEY_ISDIRECT = "d_isDirect";    ///< 음악의 네트워크파일 인지 유무 (음악인 경우에서만 사용함)
    const QString KEY_ISPLAY = "d_isPlay";    ///< 재생중 유무
    const QString KEY_ISCHECK = "d_isCheck"; ///< 체크 유무
}

/**
 * @brief 재생 큐 목록 편집 delegate @n
 *      @sa FileDelegate Copy 수정
 */
class QueueItemModDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        remove,
        etc
    };

    enum QueueItemTypes{
        Music
        ,Rosetube
    };

    explicit QueueItemModDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:

    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapCheckOff;
    QPixmap pixmapCheckOn;
    QPixmap pixmapDelete;
    QPixmap pixmapPlaying;  ///< 현재 재생중인 파일을 표현하는 아이콘
    QPixmap pixmapNet;      ///< 음악의 네트워크 파일을 의미하는 아이콘
};

#endif // QUEUEITEMMODDELEGATE_H
