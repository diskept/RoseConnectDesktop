#include "StackedWidget_AR.h"

/**
 * @brief StackedWidget_AR::StackedWidget_AR
 * @param parent
 */
StackedWidget_AR::StackedWidget_AR(QWidget *parent) : QWidget(parent){

    // Data
    this->flagAutoResize = true;            // 기본값
    this->m_currentIndex = -1;              // 기본값
    this->list_childWidget.clear();


    // UI
    this->vBox_main = new QVBoxLayout();
    this->vBox_main->setSpacing(0);
    this->vBox_main->setContentsMargins(0,0,0,0);

    this->setLayout(this->vBox_main);



}


/**
 * @brief StackedWidget_AR::slot_currentChanged
 * @param index
 */
void StackedWidget_AR::slot_currentChanged(int index){
    Q_UNUSED(index);
    if(this->flagAutoResize){
    }
}


//------------------------------------------------------------------------------------------
//
// MARK - Auto Resizing을 위해 - 추가사항
//
//------------------------------------------------------------------------------------------

/**
 * @brief StackedWidget_AR::setAutoResize
 * @param flagAuto
 */
void StackedWidget_AR::setAutoResize(bool flagAuto){
    this->flagAutoResize = flagAuto;
}


/**
 * @brief sizeHint
 * @return
 */
QSize StackedWidget_AR::sizeHint() const{
    if(this->flagAutoResize && this->count()>0){
        // 자식이 있는 경우.
        QSize size = this->currentWidget()->sizeHint();
        return size;
    }
    else{
        return QWidget::sizeHint();
    }
}




//------------------------------------------------------------------------------------------
//
// MARK - QStackedWidget 에서 지원하는 function들 처럼 구성  - public functions
//
//------------------------------------------------------------------------------------------

/**
 * @brief 자식 widget 을 뒤로 추가함. appending
 * @param child_widget
 * @return
 */
int StackedWidget_AR::addWidget(QWidget *child_widget){

    // no child -> has a child  :  자식이 없을 때는 index가 -1 이므로, 자식이 생기면 0 으로 초기화 처리
    if(this->list_childWidget.count()){
        this->m_currentIndex = 0;
    }

    this->list_childWidget.append(child_widget);
    this->vBox_main->addWidget(child_widget);
    this->setCurrentIndex(this->m_currentIndex);

    return this->list_childWidget.count()-1;
}

/**
 * @brief 자식 widget 개수 반환
 * @return
 */
int StackedWidget_AR::count() const{
    return this->list_childWidget.count();
}

/**
 * @brief 현재 보이는 인덱스 반환. 자식 없으면 -1 반환
 * @return
 */
int StackedWidget_AR::currentIndex() const{
    return this->m_currentIndex;
}


/**
 * @brief 현재 보이는 자식 widget 을 반환. 자식이 하나도 없으면 nullptr 반환
 * @return
 */
QWidget* StackedWidget_AR::currentWidget() const{
    if(this->m_currentIndex>=0 && this->m_currentIndex<this->list_childWidget.count()){
        return this->list_childWidget.at(this->m_currentIndex);
    }
    else{
        return nullptr;
    }
}


/**
 * @brief 자식 위젯의 index 반환
 * @param widget
 * @return
 */
int StackedWidget_AR::indexOf(QWidget *child_widget) const{
    return this->list_childWidget.indexOf(child_widget);
}


/**
 * @brief 해당 index 에다가 자식을 반환
 * @param index
 * @param child_widget
 * @return
 */
int StackedWidget_AR::insertWidget(int index, QWidget *child_widget){

    // no child -> has a child  :  자식이 없을 때는 index가 -1 이므로, 자식이 생기면 0 으로 초기화 처리
    if(this->list_childWidget.count()){
        this->m_currentIndex = 0;
    }

    // index checking
    if(index<0 || index>=this->list_childWidget.count()){
        // index 유효하지 않으면 appding 처리. like QStackedWidget
        return this->addWidget(child_widget);
    }
    else{
        this->list_childWidget.insert(index, child_widget);
        this->vBox_main->addWidget(child_widget);
        this->setCurrentIndex(this->m_currentIndex);
        return index;
    }
}

/**
 * @brief 자식 위젯을 삭제함. currentIndex 조정은 어떻게 해야할지 잘 모르겠다.
 * @details QStackedWidget Doc 봤는데, 상세 내용이 없어서.
 * 그냥 유지를 하는지, 자동으로 변경하는지 잘 모르겠음. 우선은 그냥 유지하는 것으로..
 * @param wichild_widgetdget
 */
void StackedWidget_AR::removeWidget(QWidget *child_widget){
    int index = this->indexOf(child_widget);
    if(index >= 0){
        child_widget->hide();                       // not delete (remain).
        this->list_childWidget.removeAt(index);     // 목록에서만 제거함

        emit this->widgetRemoved(index);
    }
}


/**
 * @brief 해당 인덱스에 대하여 자식 widget 반환함. 없으면 nullptr 반환
 * @param index
 * @return
 */
QWidget* StackedWidget_AR::widget(int index) const{
    if(index>=0 && index<this->list_childWidget.count()){
        return this->list_childWidget.at(index);
    }
    else{
        return nullptr;     // index 못찾은 경우
    }
}




//------------------------------------------------------------------------------------------
//
// MARK - QStackedWidget 에서 지원하는 function들 처럼 구성  - public [SLOT] function
//
//------------------------------------------------------------------------------------------

/**
 * @brief [Slot] 해당 index에 대한 자식 widget 보이도록 한다. 유효하지 않은 index요청하면 아무것도 안보임
 * @param index
 */
void StackedWidget_AR::setCurrentIndex(int index){

    // 해당 index의 자식 보이도록
    if(index>=0 && index<this->list_childWidget.count()){
        foreach(QWidget *child_widget, this->list_childWidget){
            child_widget->hide();
        }
        this->list_childWidget.at(index)->show();
        updateGeometry();
    }

    // index 변경된 경우, signal 발생 위해
    if(index != this->m_currentIndex){
        this->m_currentIndex = index;
        emit this->currentChanged(this->m_currentIndex);
    }
}


/**
 * @brief [Slot] widget 자식을 찾아서, 화면에 보이도록 한다.
 * @param widget
 */
void StackedWidget_AR::setCurrentWidget(QWidget *widget){
    int index = this->indexOf(widget);
    this->setCurrentIndex(index);
}




// MARK - override

/**
 * @brief StackedWidget_AR::event
 * @param ev
 * @return
 */
bool StackedWidget_AR::event(QEvent *ev){
    return QWidget::event(ev);

    // 추가적으로 어떤것을 해야할지 모르겠다.
    // QStackedWidget 에서는
    //      Reimplements: QFrame::event(QEvent *e). 라는 말만 적혀있음.... ^-^;;;
}
