#ifndef cloudListDelegate_H
#define cloudListDelegate_H

#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief USB 파일 목록 delegate @n
 *      @sa FileDelegate Copy 수정
 * @note use 음악 or 비디오 - 폴더 - USB 저장장치 파일 목록
 */
class cloudListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        more,
        etc
    };

    explicit cloudListDelegate(QObject *parent = nullptr);

    void setVideoFileMode();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    //void signal_clickedBtn(const int &p_index, const int &p_btnType);
    void signal_clickedBtn(const bool isFile, const int &p_index, const int &p_btnType);

private:
    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapMore;
    QPixmap pixmapNext;

    bool flagIsMusicFileMode = true;
};

#endif // cloudListDelegate_H
