#include "music_2/MusicHome_2.h"

namespace music {

    /**
     * @brief MusicHome::MusicHome
     * @param parent
     */
    MusicHome_2::MusicHome_2(QWidget *parent) : AbstractMusicSubWidget(MainUIType::NoneScroll, parent){
        // 기본 UI 세팅
        this->addUIControl_subTitle_withSideBtn_common(tr("Home Test"), "", 0);
    }

}
