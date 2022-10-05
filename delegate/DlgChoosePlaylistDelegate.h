#ifndef DLGCHOOSEPLAYLISTDELEGATE_H
#define DLGCHOOSEPLAYLISTDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPainter>


namespace tidal {

    /**
     * @brief 플레이리스트 담기 다이얼로그에서 사용하는 Delegate. (Tidal 에서 사용중)
     */
    class DlgChoosePlaylistDelegate : public QAbstractItemDelegate
    {
        Q_OBJECT
    public:
        explicit DlgChoosePlaylistDelegate(QObject *parent = nullptr);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    signals:
        void clicked(const int index);


    protected:
        QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;

    private:
        void setInit();
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

        QPixmap pixmap_new;
        QPixmap pixmap_empty;

    };


};

namespace qobuz {

    /**
     * @brief 플레이리스트 담기 다이얼로그에서 사용하는 Delegate. (Qobuz 에서 사용중)
     */
    class DlgChoosePlaylistDelegate : public QAbstractItemDelegate
    {
        Q_OBJECT
    public:
        explicit DlgChoosePlaylistDelegate(QObject *parent = nullptr);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    signals:
        void clicked(const int index);


    protected:
        QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;

    private:
        void setInit();
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

        QPixmap pixmap_new;
        QPixmap pixmap_empty;

    };


};
#endif // DLGCHOOSEPLAYLISTDELEGATE_H
