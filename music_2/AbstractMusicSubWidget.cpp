#include "AbstractMusicSubWidget.h"

#include <QDebug>

namespace music {

    /**
     * @brief 음악 서브페이지 위젯을 위한 부모 클래스
     * @param p_mainUIType
     * @param parent
     */
    AbstractMusicSubWidget::AbstractMusicSubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent){
    }

    /**
     * @brief 소멸자 체크. 메모리 해제 확인을
     */
    AbstractMusicSubWidget::~AbstractMusicSubWidget(){
    }


}
