#include "music_2/MusicMain_2.h"

#include "common/global.h"
#include <QDebug>

#include <QApplication>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <qapplication.h>

#include <common/gscommon.h>

namespace music {
    /**
     * @brief MusicMain::MusicMain 음악
     * @details 음악 메뉴(홈,분류,앨범,폴더,My collection) 관리
     * @param parent QWidget
     */
    MusicMain_2::MusicMain_2(QWidget *parent) : AbstractMainContent(parent)
    {
        this->setUIControl();
    }


    /**
     * @brief MusicMain::setUIControl : UI 세팅
     */
    void MusicMain_2::setUIControl(){

        this->sub_home = new music::MusicHome_2();

        this->stackedWidget_content->addWidget(this->sub_home);

        // default : 첫번째 서브메뉴
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_M2_HOME;
        this->goToMoveNewOrderPage(tmp_data);

    }


////        // 페이지 히스토리 관리 관련된 Abs함수
////        QJsonObject tmp_data;
////        tmp_data[KEY_PAGE_CODE] = p_viewAllMode;
////        this->goToMoveNewOrderPage(tmp_data);
//    }

//    /**
//     * @brief MusicMain::slot_clickedFavoriteFolderEdit [SLOT] 음악>폴더 의 즐겨찾기 편집 화면 클릭시
//     */
//    void MusicMain_2::slot_clickedFavoriteFolderEdit(const QJsonArray &QJsonArray){

////        QJsonObject tmp_data;
////        tmp_data[KEY_PAGE_CODE] = PAGECODE_ETC_EDIT_FAVORITE;
////        tmp_data.insert(KEY_DATA, QJsonArray);

////        this->goToMoveNewOrderPage(tmp_data);
//    }


    /**
     * @brief MusicMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void MusicMain_2::movePageOnly(QString p_pageCode, const QJsonObject &p_data){


        QJsonObject p_jsonObject_titleMain;
        p_jsonObject_titleMain["name"] = tr("Music (Test)");
        //p_jsonObject_titleMain["name"] = "음악 (Test)";
        p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Music_2;
        QJsonArray *p_jsonArray_titlSub = new QJsonArray();
        QJsonObject sub1 { {"name", tr("Home")}, {"code", PAGECODE_M2_HOME} };
        //QJsonObject sub1 { {"name", "홈"}, {"code", PAGECODE_M2_HOME} };
        p_jsonArray_titlSub->push_back(sub1);


        this->topMenuBar->setVisible(true);


        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);

        // 자식 override 함수 서술
        if(p_pageCode==PAGECODE_M2_HOME){
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            stackedWidget_content->setCurrentWidget(this->sub_home);
        }

    }

}
