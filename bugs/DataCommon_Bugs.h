#ifndef DATACOMMON_BUGS_H
#define DATACOMMON_BUGS_H

#include <QList>
//#include "bugs_struct.h"
#include "BugsChooseFilterOpt.h"

namespace bugs {

    /**
     * @brief 벅스 메뉴 전체적으로 공용으로 사용을 해야하는 데이터. (데이터 유지 필요. Singletone design pattern)
     * @see Qt-forum https://forum.qt.io/topic/58688/singleton-class-example
     */
    class DataCommon_Bugs : public QObject
    {
        Q_OBJECT

    public:

        // 장르 선택 (최신음악, 벅스 차트)
        QList<bugs::FilterOptionData> list_all;
        QList<bugs::FilterOptionData> list_kpop;
        QList<bugs::FilterOptionData> list_pop;
        QList<bugs::FilterOptionData> list_etc;

        // 테마 선택
        QList<bugs::FilterOptionData> list_theme;

        // singleton
        static DataCommon_Bugs &instance() {
            static DataCommon_Bugs * _instance = 0;
            if ( _instance == 0 ) {
                _instance = new DataCommon_Bugs();
            }
            return *_instance;
        }

        DataCommon_Bugs() {}                                    // Private constructor
        DataCommon_Bugs(const DataCommon_Bugs &);               // Non copy-constructible
        DataCommon_Bugs &operator=(const DataCommon_Bugs &);    // Non copyable

    };

};

#endif // DATACOMMON_BUGS_H
