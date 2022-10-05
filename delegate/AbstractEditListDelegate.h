#ifndef ABSTRACTEDITLISTDELEGATE_H
#define ABSTRACTEDITLISTDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPainter>

namespace editDelegate {

    /**
     * @brief The ItemEditData struct
     * 필드명은 Track 기준으로 잡아둔 것임
     */
    struct ItemEditData{
        bool flagChecked;
        QString album_image;
        QString track_title;
        QString artist_name;
        QString album_title;
    };

};

/**
 * @brief 편집용 list형 abstract delegate. (Tidal에서 상속 사용)
 */
class AbstractEditListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    enum ClickMode{
        CheckBox = 1        ///< 나머지 영역 전체 박스를 클릭
        , TrashBtn          ///< 개별삭제
    };
    Q_ENUM(ClickMode);

    const int BTN_ICON_WIDTH = 60;
    const int SPACE = 20;                   ///< 아이템 간의 간격
    const int ITEM_ALL_HEIGHT = 70;
    const int SIZE_CHECKBOX = 50;

    explicit AbstractEditListDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // about editDelegate::ItemEditData
    static QJsonObject getObjectJson_itemEditData(const editDelegate::ItemEditData &data);
    static editDelegate::ItemEditData convertData_itemEditData(const QJsonObject &jsonObj);

signals:
    void clicked(const int index, ClickMode clickMode);


protected:
    QString ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const;

//    QJsonObject conver

    int fixed_whole_width;                  ///< 고정 위젯들과 그 위젯들간의 space 통합

    int pos_more_x1;
    int pos_more_x2;
    int pos_fav_x1;
    int pos_fav_x2;
    int pos_addCollection_x1;
    int pos_addCollection_x2;

    QPixmap pixmapCheckOff;
    QPixmap pixmapCheckOn;
    QPixmap pixmapDel;



private:
    void setInit();
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;


};

#endif // ABSTRACTEDITLISTDELEGATE_H
