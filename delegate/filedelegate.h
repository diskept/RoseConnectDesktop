#ifndef FILEDELEGATE_H
#define FILEDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 파일정보 basic delegate @n
 *      파일인 경우 플레이리스트담기, 하트, 더보기 옵션 제공 @n
 *      폴더인 경우 네트워크폴더 등록, 즐겨찾기 등록 제공
 * @note use 음악-폴더-네트워크 파일 목록 작업중 최초 작성 @n
 *      가로 1920 기준으로 작성, 가로 사이즈 변경시 각 항목 위치 재조정
 */
class FileDelegate : public QAbstractItemDelegate
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

    explicit FileDelegate(QObject *parent = nullptr);
    ~FileDelegate();

    void setVideoFileMode();
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
    QPixmap pixmapFavor;
    QPixmap pixmapBookMark;
    QPixmap pixmapAddFolder;

    bool flagIsMusicFileMode=true;

};

#endif // FILEDELEGATE_H
