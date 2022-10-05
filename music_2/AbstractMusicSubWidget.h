#ifndef ABSTRACTMUSICSUBWIDGET_H
#define ABSTRACTMUSICSUBWIDGET_H

#include "common/AbstractCommonSubWidget.h"

namespace music {

    /**
     * @brief 음악 서브 페이지를 구성하는데 있어, 상속체계의 부모가 되는 클래스
     * 공통으로 사용하는 시그널 및 공통 클래스 구성 스타일을 정의함 (클래스 변수 및 형태)
     */
    class AbstractMusicSubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT
    public:
        explicit AbstractMusicSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractMusicSubWidget();
    };

};

#endif // ABSTRACTMUSICSUBWIDGET_H
