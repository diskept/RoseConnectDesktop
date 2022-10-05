#include "AbstractProcRosePlay.h"
#include "common/rosesettings.h"

#include <QSettings>

/**
 * @brief ProcRosePlay_withTidal::ProcRosePlay_withTidal
 * @param parent
 */
AbstractProcRosePlay::AbstractProcRosePlay(QWidget *parent) : QObject(parent) {
    this->parent = parent;
    this->curr_shuffleMode = PlayShuffleMode::NoUse;
}


/**
 * @brief OptMorePopup::ClickMode 에 따라 Rose로 요청할 playType 정보를 int 로 반환한다.
 * @details Tidal의 음원, 비디오 모두 똑같다. playType 옵션값이.   음악, 비디오 등의 메뉴에서도 똑같은지는 확인해봐야.
 * @param clickMode
 * @return
 */
int AbstractProcRosePlay::get_rose_playType(OptMorePopup::ClickMode clickMode){

    if(clickMode == OptMorePopup::ClickMode::Play_RightNow
            || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
    ){
        // 바로 재생을 요청하는 타입
        return 15;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
    ){
        // 현재 큐 끝에 추가
        return 13;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
    ){
        // 큐를 비우고 추가(재생)
        return 12;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
    ){
        return 16;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
        // 여기부터 재생
        return 17;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
        // 큐 비우고 여기부터 재생
        return 18;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
        // 여기부터 큐 끝에 추가
        return 19;
    }
    else{
        return 0;           // unknown or not supported
    }
}


/**
 * @brief OptMorePopup::ClickMode에 따라서 JsonArray(Track정보)를 재가공해서 반환한다.
 * @param jsonArr_toPlayAll
 * @param curr_index
 * @param clickMode
 * @return
 */
QJsonArray AbstractProcRosePlay::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){
    if(clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
            || clickMode == OptMorePopup::ClickMode::Play_RightNow
    ){
        if(curr_index > 0){
            // 전체이지만 re-ordering
            QJsonArray jsonArr_output = this->reorderJsonArray(jsonArr_toPlayAll, curr_index);
            return jsonArr_output;
        }
        else{
            return jsonArr_toPlayAll;
        }
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
    ){
        // 1곡만
        QJsonArray jsonArr_output;
        jsonArr_output.append(jsonArr_toPlayAll.at(curr_index).toObject());
        return jsonArr_output;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
    ){
        // index ~ 마지막까지 구성
        if(curr_index > 0){
            QJsonArray jsonArr_output = this->subRange_JsonArray(jsonArr_toPlayAll, curr_index);
            return jsonArr_output;
        }
        else{
            return jsonArr_toPlayAll;
        }
    }
    else{
        // 그 이외의 경우
        return jsonArr_toPlayAll;
    }

}


/**
 * @brief AbstractProcRosePlay::subRange_JsonArray
 * @param p_jsonArr
 * @param startIndex
 * @return
 */
QJsonArray AbstractProcRosePlay::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){
    const int len_data = p_jsonArr.count();
    if(startIndex>0 && startIndex<len_data){
        QJsonArray jsonArr_subRange;
        for(int i=startIndex ; i<len_data ; i++){
            jsonArr_subRange.append(p_jsonArr.at(i));
        }
        return jsonArr_subRange;
    }
    else{
        // 유효하지 않는 경우, 그대로 반환
        return p_jsonArr;
    }
}


/**
 * @brief AbstractProcRosePlay::reorderJsonArray
 * @param p_jsonArr
 * @param startIndex
 * @return
 */
QJsonArray AbstractProcRosePlay::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){
    const int len_data = p_jsonArr.count();
    if(startIndex>0 && startIndex<len_data){
        QJsonArray jsonArr_reorder;
        for(int i=startIndex ; i<len_data ; i++){
            jsonArr_reorder.append(p_jsonArr.at(i));
        }
        for(int i=0 ; i<startIndex ; i++){
            jsonArr_reorder.append(p_jsonArr.at(i));
        }
        return jsonArr_reorder;
    }
    else{
        // 유효하지 않는 경우, 그대로 반환
        return p_jsonArr;
    }
}
