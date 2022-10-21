#include "customlineedit.h"
#include <QKeyEvent>
#include <QMenu>//cheon210728-test
#include <QAction>//cheon210728-test
//#include <common/global.h>
#include <QDrag>
#include <QMimeData>

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

/**
 * @brief CustomLineEdit::CustomLineEdit : 커스틈 QLineEdit
 * @param parent
 * @note 한글 입력 처리를 위함</br>
 * QLineEdit 의 textChanged() textEdited() 를 통해서는 작성중인 글자에 대한 정보를 확인할 수 없음.</br>
 * ex ) 아이유 입력시 포커싱 이벤트 또는 엔터를 입력해야 아이유 full text 를 받을 수 있다.
 */
CustomLineEdit::CustomLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    linker = Linker::getInstance();

}

void CustomLineEdit::contextMenuEvent(QContextMenuEvent *event)//cheon210728-test
{
    QMenu *menu = createStandardContextMenu();
    //menu->setStyleSheet("*:hover { color:#FFF; } *:!hover { color:#aaa; }");//"background-color:#333333;"
    menu->setStyleSheet("background-color:#c1c1c1;");//"background-color:#333333;"
    //menu->addAction(tr("Paste\tCtrl+V"));
    //...
    menu->exec(event->globalPos());
    delete menu;

}

void CustomLineEdit::mouseReleaseEvent(QMouseEvent *){//c220726
    print_debug();
    qDebug() << " CustomLineEdit::mouseReleaseEvent=" << this->text();
}


void CustomLineEdit::dragLeaveEvent(QDragLeaveEvent *event)
{

    print_debug();
    //qDebug() << "QDragMoveEvent"<<event;
        QWidget::dragLeaveEvent(event);

        emit linker->signal_dragEnterEvent_hide_show(false);//c220727

       // event->acceptProposedAction();
}
/*
void CustomLineEdit::dragMoveEvent(QDragMoveEvent *event)
{

    print_debug();
    //qDebug() << "QDragMoveEvent"<<event;
        QWidget::dragMoveEvent(event);

        event->acceptProposedAction();
}
*/
/*
void CustomLineEdit::filterEvent(const QEvent *event)//c220826_1
{

    if (event->type() == QEvent::KeyPress){
        print_debug();
    }

}
*/
void CustomLineEdit::dragEnterEvent(QDragEnterEvent* event) {

    QWidget::dragEnterEvent(event);
    event->accept();
    print_debug();
    qDebug() << "QDragEnterEvent"<<event;
    emit linker->signal_dragEnterEvent_hide_show(true);//c220727
}

void CustomLineEdit::dropEvent(QDropEvent *event)//c220726
{
    print_debug();
    qDebug() << "dropEvent"<<event;
    const QMimeData *mimeData = event->mimeData();

#if defined(Q_OS_WINDOWS)
        //if (mimeData->hasFormat("application/x-qt-windows-mime;value=\"DragContext\"")) {//c220818
        if (1) {//c220818
   #endif
   #if defined(Q_OS_MAC)
        if (1) {
   #endif
        print_debug();

        qDebug() << "le_search->text()=" << this->text();
        this->clear();
        emit signal_dragDrop_search();
        emit signal_dragDrop_search(true);
        emit linker->signal_dropEvent_hide_show(true);

    }else{
        print_debug();
        emit signal_dragDrop_clear();
        this->clear();
        emit linker->signal_dragEnterEvent_hide_show(false);
    }

    QLineEdit::dropEvent(event);

}

/**
 * @brief CustomLineEdit::inputMethodEvent
 * @param p_event
 * @note 한글 입력의 경우 작성중인 마지막 한글(커서 글자)이 text()로 반환받은 문자열에 포함되지 않음</br>
 * 작성중인 글자는 따로 처리가 필요하다.
 */
void CustomLineEdit::inputMethodEvent(QInputMethodEvent *p_event){
    QLineEdit::inputMethodEvent(p_event);

    preeditStr = p_event->preeditString();

    if(!preeditStr.isEmpty()){
        emit signal_textChanged(getTextAddedPreedistStr());
    }
}

/**
 * @brief CustomLineEdit::keyPressEvent : keypress
 */
void CustomLineEdit::keyPressEvent(QKeyEvent *p_event){//c220904_1

    QLineEdit::keyPressEvent(p_event);

    if(p_event->key() == Qt::Key_Return || p_event->key() == Qt::Key_Enter){
        this->clearFocus();
        emit signal_clickedEnter(getTextAddedPreedistStr());
    }else if(p_event->key() == Qt::Key_Right || p_event->key() == Qt::Key_Down){
        print_debug();
        emit signal_clickedDown(getTextAddedPreedistStr());
    }else if(p_event->key() == Qt::Key_Left || p_event->key() == Qt::Key_Up){
        print_debug();
        emit signal_clickedUp(getTextAddedPreedistStr());
    }else if(p_event->key() == Qt::Key_Escape){
        emit linker->signal_searchBarFocusChanged(false);
        this->clearFocus();
        this->clear();
    }
    else{

        if(p_event->key() != 0){
            emit signal_textChanged(getTextAddedPreedistStr());
        }
    }
}

/**
 * @brief CustomLineEdit::getTextAddedPreedistStr : text() 함수에 PreeditString 포함하여 반환
 * @return QString
 */
QString CustomLineEdit::getTextAddedPreedistStr(){

    QString tmp_text = text();
    tmp_text.append(preeditStr);
    preeditStr.clear();

    return tmp_text;
}

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

/**
 * @brief CustomLineEdit::focusInEvent : 포커스 IN Override
 * @param p_event
 */
void CustomLineEdit::focusInEvent(QFocusEvent *p_event){
    QLineEdit::focusInEvent(p_event);
print_debug();
this->setCursor(Qt::PointingHandCursor);//c221020_1
    // ---------------------------------------------------
    //  포커스 시그널 : 검색 제안어 UI Show/Hide 용도로 사용
    //      :: 입력된 문자열이 2글자 이상인 경우에만 시그널 발생
    // ---------------------------------------------------
    //if(text().size() >= 2){//c220620
        emit linker->signal_searchBarFocusChanged(true);
    //}
}

/**
 * @brief CustomLineEdit::focusOutEvent : 포커스 Out Override
 * @param p_event
 */
void CustomLineEdit::focusOutEvent(QFocusEvent *p_event){
    QLineEdit::focusOutEvent(p_event);
    print_debug();
    this->clearFocus();//c220802

    emit linker->signal_searchBarFocusChanged(false);//c220904_1
}
