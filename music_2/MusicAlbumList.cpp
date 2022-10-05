#include "MusicAlbumList.h"

namespace music {

    /**
     * @brief
     * @param parent
     */
    MusicAlbumList::MusicAlbumList(QWidget *parent) : AbstractMusicSubWidget(MainUIType::VerticalScroll, parent){
        // 기본 UI 세팅
        this->addUIControl_subTitle_withSideBtn_common(tr("Album List Test"), "", 0);
    }

}
