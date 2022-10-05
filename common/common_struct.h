#ifndef COMMON_STRUCT_H
#define COMMON_STRUCT_H

#include <QWidget>

namespace common {


    /**
     * @brief 최상단 검색 관련한 struct. 현재는 Tidal, Bugs, Qobuz만 사용
     */
    struct SearchData{
        QString search_word;
        QString search_keyword;
    };


    /**
     * @brief common 네임스페이스 범위의 struct 에 대해서 JSON 변환을 static 으로 지원하는 클래스
     */
    class ConvertData_common : public QObject {

    public:
        // about bugs::ItemPositionData
        static QJsonObject getObjectJson_searchData(const common::SearchData &data);
        static common::SearchData convertData_searchData(const QJsonObject &jsonObj);
    };

}

#endif // COMMON_STRUCT_H
