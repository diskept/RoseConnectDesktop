#include "AbstractQobuzTrackDelegate.h"


#include <QDateTime>
#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include "qobuz/qobuz_struct.h"
#include "qobuz/ConvertData_forQobuz.h"


namespace qobuz {

    // 고정 사이즈
//    const int ITEM_ALL_WIDTH_MIN = 914;     ///< MIN WIDTH
    const int ITEM_ALL_HEIGHT = 70;         ///< Track 아이템 Height



    /**
     * @brief AbstractQobuzTrackDelegate::AbstractQobuzTrackDelegate
     * @param flagShowBtn_addCollection
     * @param parent
     */
    AbstractQobuzTrackDelegate::AbstractQobuzTrackDelegate(int section, UseMode useMode, QObject *parent)
        : QAbstractItemDelegate(parent)
    {

        this->m_useMose = useMode;
        this->flagShowBtn_like = ( (useMode & UseMode_Flag::Favorite) == UseMode_Flag::Favorite );
        this->flagShowBtn_addCollection = ( (useMode & UseMode_Flag::AddCollection) == UseMode_Flag::AddCollection );

        this->m_section = section;

        setInit();
    }


    /**
     * @brief AbstractQobuzTrackDelegate::setInit
     */
    void AbstractQobuzTrackDelegate::setInit(){
        this->pixmap_addCollection.load(":images/play_list_add_icon.png");
        this->pixmap_fav_off.load(":images/list_fav_icon.png");
        this->pixmap_fav_on.load(":images/list_fav_icon_on.png");
        this->pixmap_more.load(":images/icon_menu.png");

        this->fixed_whole_width = SPACE + BTN_ICON_WIDTH*3 + SPACE;

        // add collection 미사용
        if(!this->flagShowBtn_addCollection){
            this->fixed_whole_width -= BTN_ICON_WIDTH;
        }

        // fav on/off 미사용
        if(!this->flagShowBtn_like){
            this->fixed_whole_width -= BTN_ICON_WIDTH;
        }

    }


    /**
     * @brief section값 반환
     * @return
     */
    int AbstractQobuzTrackDelegate::section(){
        return this->m_section;
    }



    /**
     * @brief AbstractQobuzTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void AbstractQobuzTrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        Q_UNUSED(index);

        QRect rect = option.rect;
        int width = rect.width();


        // Line Draw
        QPen linePen(QColor::fromRgb(77,77,77), 1, Qt::SolidLine);          // #4d4d4d
        painter->setPen(linePen);
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());



        // 더보기 - the most right side
        int tmp_x1 = width-BTN_ICON_WIDTH*1-SPACE;
        int tmp_x2 = -SPACE;
        rect = option.rect.adjusted(tmp_x1,0,tmp_x2,0);
        painter->drawPixmap(rect , this->pixmap_more);


        // Fav on/off - data와의 연결이 필요해서, 이 부분은 자식 클래스에서 호출되도록 함 (not drawing)
        if(this->flagShowBtn_like){
            tmp_x1 = tmp_x1 - BTN_ICON_WIDTH;
            tmp_x2 = tmp_x2 - BTN_ICON_WIDTH;
        }


        // 플레이리스트 담기
        if(this->flagShowBtn_addCollection){
            tmp_x1 = tmp_x1 - BTN_ICON_WIDTH;
            tmp_x2 = tmp_x2 - BTN_ICON_WIDTH;
            rect = option.rect.adjusted(width-BTN_ICON_WIDTH*2-SPACE, 0, -BTN_ICON_WIDTH*1-SPACE, 0);
            painter->drawPixmap(rect, this->pixmap_addCollection );
        }

    }


    /**
     * @brief AbstractQobuzTrackDelegate::sizeHint
     * @param option
     * @param index
     * @return
     */
    QSize AbstractQobuzTrackDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
        Q_UNUSED(option);
        Q_UNUSED(index);
        return QSize(option.rect.width(), ITEM_ALL_HEIGHT);
    }


    /**
     * @brief AbstractQobuzTrackDelegate::ellipseString : Text ellipse
     * @details Text가 정해진 영역보다 긴 경우 ... 표기
     * @param p_str
     * @param p_width
     * @param p_font
     * @return
     */
    QString AbstractQobuzTrackDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
        QFontMetrics metrics(p_font);
        return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
    }


    /**
     * @brief AbstractQobuzTrackDelegate::editorEvent : 마우스 이벤트 처리
     * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
     * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
     * @param event
     * @param model
     * @param option
     * @param index
     * @return
     */
    bool AbstractQobuzTrackDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

        QMouseEvent *eventMouse = (QMouseEvent*)event;
        if(eventMouse->button() == Qt::LeftButton){

            QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
            int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();              // 이전에 클릭했던 Btn.

            // init
            int tmp_x1 = option.rect.width();
            int tmp_x2 = 0;
            int tmp_width_allBtn = 0;

            QRect rectBtn_more;
            QRect rectBtn_favorite;
            QRect rectBtn_addCollection;
            QRect rectBtn_remain;

            // 더보기
            tmp_x1 -= BTN_ICON_WIDTH;
            rectBtn_more = option.rect.adjusted(tmp_x1, 0, tmp_x2, 0);
            tmp_width_allBtn += BTN_ICON_WIDTH;

            if(this->flagShowBtn_like){
                // fav on/off
                tmp_x1 -= BTN_ICON_WIDTH;
                tmp_x2 -= BTN_ICON_WIDTH;
                rectBtn_favorite = option.rect.adjusted(tmp_x1, 0, tmp_x2, 0);
                tmp_width_allBtn += BTN_ICON_WIDTH;
            }

            if(this->flagShowBtn_addCollection){
                // 플레이리스트 담기
                tmp_x1 -= BTN_ICON_WIDTH;
                tmp_x2 -= BTN_ICON_WIDTH;
                rectBtn_addCollection = option.rect.adjusted(tmp_x1, 0, tmp_x2, 0);
                tmp_width_allBtn += BTN_ICON_WIDTH;
            }

            // 나머지 영역
            rectBtn_remain = option.rect.adjusted(0, 0, -tmp_width_allBtn, 0);




            // 마우스 Event 별로 처리
            if(event->type() == QEvent::MouseButtonPress){

                if(rectBtn_more.contains(eventMouse->pos())){
                    model->setData(index, ClickMode::MoreBtn, Qt::UserRole+1);
                }
                else if(this->flagShowBtn_like && rectBtn_favorite.contains(eventMouse->pos())){
                    model->setData(index, ClickMode::FavBtn, Qt::UserRole+1);
                }
                else if(this->flagShowBtn_addCollection && rectBtn_addCollection.contains(eventMouse->pos())){
                    model->setData(index, ClickMode::AddCollectionBtn, Qt::UserRole+1);
                }
                else if(rectBtn_remain.contains(eventMouse->pos())){
                    model->setData(index, ClickMode::AllBox, Qt::UserRole+1);
                }
            }
            else if(event->type() == QEvent::MouseButtonRelease){
                if(tmp_pressBtn==ClickMode::MoreBtn && rectBtn_more.contains(eventMouse->pos())){
                    emit this->clicked(index.row(), ClickMode::MoreBtn);
                }
                else if(this->flagShowBtn_like && tmp_pressBtn==ClickMode::FavBtn && rectBtn_favorite.contains(eventMouse->pos())){
                    emit this->clicked(index.row(), ClickMode::FavBtn);
                }
                else if(this->flagShowBtn_addCollection && tmp_pressBtn==ClickMode::AddCollectionBtn && rectBtn_addCollection.contains(eventMouse->pos())){
                    emit this->clicked(index.row(), ClickMode::AddCollectionBtn);
                }
                else if(tmp_pressBtn==ClickMode::AllBox && rectBtn_remain.contains(eventMouse->pos())){
                    emit this->clicked(index.row(), ClickMode::AllBox);
                }

                model->setData(index, 0, Qt::UserRole+1);       // init
            }


        }

        return true;
    }

}
