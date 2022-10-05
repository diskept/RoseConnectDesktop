#ifndef SUGGESTIONDELEGATE_H
#define SUGGESTIONDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

/**
 * @brief 검색 제안어 delegate @n
 *      @sa FileDelegate Copy 수정
 */
class SuggestionDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    enum BtnType{
        none,
        etc,
        del
    };


    explicit SuggestionDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void signal_clickedBtn(const int &p_index, const int &p_btnType);

private:
    void setInit();
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QPixmap pixmapSearch, pixmapDelete;//c220628
};

#endif // SUGGESTIONDELEGATE_H
