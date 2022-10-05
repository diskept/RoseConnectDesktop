#include "sharefolderdelegate.h"
#include <QRect>
#include <QMouseEvent>
#include <QDebug>//c220629

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";//c220710

const int thumbnail_width = 40;//c220904_1
const int space = 5;//c220904_1

/**
 * @brief SharefolderDelegate::SharefolderDelegate : 검색 제안단어 delegate
 * @param parent
 */
SharefolderDelegate::SharefolderDelegate(QObject *parent)
{
    Q_UNUSED(parent);           // 미사용 by sunnyfish
    setInit();
}

/**
 * @brief SharefolderDelegate::setInit : 초기 세팅
 */
void SharefolderDelegate::setInit(){
    pixmapSearch.load(":images/icon_ser_w_40.png");
    //pixmapSearch = pixmapSearch.scaled(25,25, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//c220904_1
    pixmapDelete.load(":images/icon_close_gray.png");//c220628
    pixmapDelete = pixmapDelete.scaled(40,40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//c220904_1
}

/**
 * @brief SharefolderDelegate::paint : 페인트 Draw
 * @param painter
 * @param option
 * @param index
 */
void SharefolderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const//c220620-search
{
    QRect rect = option.rect;


    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    //    int tmp_index = map["index"].toInt();                                 // 사용하지 않는 값으로 주석처리함 by sunnyfish
    QString tmp_suggestion = map["suggestion"].toString();
    bool tmp_suggestion_s = map["old"].toBool();
    bool tmp_suggestion_old_path_flag = false;
#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)//c220913_1
    QString tmp_ip = map["ip"].toString();
    print_debug();
    qDebug() << "tmp_ip = " << tmp_ip;

    tmp_suggestion_old_path_flag = map["old_flag"].toBool();
#endif
    QPen titlePen;//c220913_1
    if(!tmp_suggestion_s){


        // pen 정의
        QPen linePen(QColor::fromRgb(76,76,76), 1, Qt::SolidLine);
        if(tmp_suggestion_old_path_flag){//c220913_1
            titlePen.setColor(QColor::fromRgb(150,150,150));
            titlePen.setWidth(1);
            titlePen.setStyle(Qt::SolidLine);
        }else{//c220913_1
            //QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
            titlePen.setColor(QColor::fromRgb(255,255,255));
            titlePen.setWidth(1);
            titlePen.setStyle(Qt::SolidLine);
        }

        QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);

        // Line Draw
        painter->setPen(linePen);
        painter->drawLine(rect.bottomLeft().x()+15, rect.bottomLeft().y(), rect.bottomRight().rx()-15, rect.bottomRight().ry());


        // Set Hover Background color
        if(option.state & QStyle::State_MouseOver){
            rect = option.rect.adjusted(15,5,-15,-5);
            painter->fillRect(rect, QColor(77,77,77));
        }else if(option.state & QStyle::State_Selected){//c220903_1
            rect = option.rect.adjusted(15,5,-15,-5);
            painter->fillRect(rect, QColor(77,77,77));
        }



        // Font 설정 : 16px 고정
        QFont font_16;
        font_16.setPixelSize(16);
        painter->setFont(font_16);

        // 아이콘
        rect = option.rect.adjusted(15,0,0,0);
        painter->drawPixmap(rect.left(), rect.top(), 40,rect.height(), pixmapSearch);//c220904_1

        // 검색 제안어
        rect = option.rect.adjusted(15 + thumbnail_width + space, 0, 0, 0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), 400, rect.height()
                          , Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_suggestion, 400, font_16), &rect);


    }else{
        // pen 정의
        QPen linePen(QColor::fromRgb(76,76,76), 1, Qt::SolidLine);
        QPen titlePen(QColor::fromRgb(150,150,150), 1, Qt::SolidLine);
        QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);

        // Line Draw
        painter->setPen(linePen);
        painter->drawLine(rect.bottomLeft().x()+15, rect.bottomLeft().y(), rect.bottomRight().rx()-15, rect.bottomRight().ry());


        // Set Hover Background color
        if(option.state & QStyle::State_MouseOver){
            rect = option.rect.adjusted(15,5,-15,-5);
            painter->fillRect(rect, QColor(77,77,77));
        }else if(option.state & QStyle::State_Selected){//c220903_1
            rect = option.rect.adjusted(15,5,-15,-5);
            painter->fillRect(rect, QColor(77,77,77));
        }



        // Font 설정 : 16px 고정
        QFont font_16;
        font_16.setPixelSize(16);
        painter->setFont(font_16);

        // 아이콘
        rect = option.rect.adjusted(15,0,0,0);
        painter->drawPixmap(rect.left(), rect.top(), 40,rect.height(), pixmapSearch);//c220904_1

        rect = option.rect.adjusted(rect.right()-50,10,-10,0);//c220904_1
        painter->drawPixmap(rect.left(), rect.top(), 40,40, pixmapDelete);//c220904_1


        // 검색 제안어
        //painter->setBackground(QColor(255, 0, 0, 50));
        rect = option.rect.adjusted(15 + thumbnail_width + space, 0, 0, 0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), 400, rect.height()
                          , Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_suggestion, 400, font_16), &rect);

    }

}


/**
 * @brief SharefolderDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize SharefolderDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(500,70);
    return QSize(option.rect.width(),60);//c220904_1
}

/**
 * @brief SharefolderDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString SharefolderDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{

    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief SharefolderDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool SharefolderDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)//c220629
{
    Q_UNUSED(option);               // 미사용으로 주석처리함 by sunnyfish
    QMouseEvent *eventMouse = (QMouseEvent*)event;
/*   //c220710
    qDebug() << "\noption.rect.rect.x()=" << option.rect.x();
    qDebug() << "option.rect.rect.y()=" << option.rect.y();
    qDebug() << "option.rect.width()=" << option.rect.width();
    qDebug() << "option.rect.height()=" << option.rect.height();
*/
    //QRect rectBtn_1 = option.rect.adjusted(option.rect.width()-180,5,-120,-5);
    //QRect rectBtn_2 = option.rect.adjusted(option.rect.width()-120,0,-60,0);
    //QRect rectBtn_3 = option.rect.adjusted(option.rect.width()-60,0,0,0);

    QRect rectBtn_etc = option.rect.adjusted(option.rect.width()-450,0,-100, 0);
    QRect rectBtn_del = option.rect.adjusted(option.rect.width()-100, 0, 0, 0);
/*
 * print_dedbug();
    qDebug() << "\nrectBtn_etc.rect.x()=" << rectBtn_etc.x();
    qDebug() << "rectBtn_etc.rect.y()=" << rectBtn_etc.y();
    qDebug() << "rectBtn_etc.rect.width()=" << rectBtn_etc.width();
    qDebug() << "rectBtn_etc.rect.height()=" << rectBtn_etc.height();

    qDebug() << "\nrectBtn_del.rect.x()=" << rectBtn_del.x();
    qDebug() << "rectBtn_del.rect.y()=" << rectBtn_del.y();
    qDebug() << "rectBtn_del.rect.width()=" << rectBtn_del.width();
    qDebug() << "rectBtn_del.rect.height()=" << rectBtn_del.height() << "\n";
*/



    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        int tmp_pressBtn = index.data(Qt::UserRole+2).toInt();
#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)//c220913_1
    print_debug();
    bool tmp_suggestion_old_path_flag = map["old_flag"].toBool();
    if(tmp_suggestion_old_path_flag){
        qDebug() << "It already exists as a shared folder.";
        return true;
    }else{

    }
#endif
        //print_debug();
       // qDebug() << "tmp_index=" << tmp_index;
       // qDebug() << "tmp_pressBtn=" << tmp_pressBtn;

        if (event->type() == QEvent::MouseButtonPress){
            if(rectBtn_del.contains(eventMouse->pos())){
               // print_debug();
                model->setData(index, BtnType::del, Qt::UserRole+2);

            }else if(rectBtn_etc.contains(eventMouse->pos()) ){
                // print_debug();
                model->setData(index, BtnType::etc, Qt::UserRole+2);
            }else{
                // print_debug();
                model->setData(index, BtnType::none, Qt::UserRole+2);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){
            if(rectBtn_etc.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::etc){
               // qDebug() << "etc-tmp_pressBtn=" << tmp_pressBtn;
                emit signal_clickedBtn(tmp_index, BtnType::etc);
            }
            if(rectBtn_del.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::del){
               // qDebug() << "del-tmp_pressBtn=" << tmp_pressBtn;
                emit signal_clickedBtn(tmp_index, BtnType::del);
            }else{
                //qDebug() << "none-tmp_pressBtn=" << tmp_pressBtn;
            }

        }
    }
     return true;
}
