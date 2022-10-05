#include "common_struct.h"
#include <QJsonObject>
#include "common/ProcJsonEasy.h"

namespace common {

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about TrackItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData_common::getObjectJson_searchData(const common::SearchData &data){
        QJsonObject jsonObj;
        jsonObj.insert("search_word", data.search_word);
        jsonObj.insert("search_keyword", data.search_keyword);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 TrackItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    common::SearchData ConvertData_common::convertData_searchData(const QJsonObject &jsonObj){
        common::SearchData data_output;
        data_output.search_word = ProcJsonEasy::getString(jsonObj, "search_word");
        data_output.search_keyword = ProcJsonEasy::getString(jsonObj, "search_keyword");

        return data_output;
    }


}
