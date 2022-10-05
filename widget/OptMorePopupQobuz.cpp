#include "OptMorePopup.h"
#include "common/gscommon.h"
#include "common/global.h"
#include <QPainter>
#include <QPainterPath>
#include "common/ProcStarData.h"

#include "qobuz/ProcCommon.h"
//#include "tidal/ProcCommon.h"
#include "common/ProcJsonEasy.h"
//#include "tidal/ConvertData.h"
#include "qobuz/ConvertData.h"






//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : Only QOBUZ
//
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief 일부, 즐겨찾기 추가/삭제를 위해서 http 요청 필요함
 */
void OptMorePopup::check_qobuzFavorites(){
    if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Video

    ){
        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &OptMorePopup::slot_qobuz_completeReq_listAll_myFavoritesIds);
        proc->request_qobuz_getAll_favorites();
    }
}

