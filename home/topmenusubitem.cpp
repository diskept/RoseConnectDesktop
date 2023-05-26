#include "topmenusubitem.h"

#include <QPushButton>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QDebug>
#include "common/gscommon.h"
#include "widget/pushBottonWidget.h"//c230328
#include "widget/toastmsg.h"

#define print_debug() qDebug() << "\n" << "file_name: " << FILE << "function_name: " << FUNCTION << "line: " << LINE << "\n";

// 2020-06-18 [C-01]화면에 메뉴바길이 수정요청 들어와서 스타일 변경 적용함
//const QString tmp_styleBasic            = "color:white;padding-left:16px;padding-right:16px;padding-bottom:1px;padding-top:5px;font-size:18px;border:1px solid transparent;";
const QString style_subitem_basic           = "color:white;padding-left:9px;padding-right:9px;padding-bottom:2px;padding-top:5px;font-size:18px;border:1px solid transparent;";
const QString style_subitem_albumDetail     = "color:white;padding-left:8px;padding-right:8px;padding-bottom:11px;padding-top:5px;font-size:18px;border:1px solid transparent;";
const QString style_subitem_listMenubar     = "color:white;padding-left:8px;padding-right:8px;padding-bottom:2px;padding-top:5px;font-size:18px;border:1px solid transparent;";
const QString style_subitem_friendManager   = "color:white;padding-left:40px;padding-right:40px;padding-bottom:11px;padding-top:5px;font-size:18px;border:1px solid transparent;";
const QString style_subitem_friendSubmenubar   = "color:white;padding-left:9px;padding-right:9px;padding-bottom:11px;padding-top:5px;font-size:18px;border:1px solid transparent;";//cheon211018
const QString style_subitem_friendSubmenubar_hover   = "color:#B18658;padding-left:9px;padding-right:9px;padding-bottom:11px;padding-top:5px;font-size:18px;border:1px solid transparent;";//cheon211018

TopMenuSubItem::TopMenuSubItem(QJsonObject p_data, UIMode p_uiMode, QWidget *parent) : QWidget(parent)
{
    this->uiMode = p_uiMode;
    this->subMenuCode = p_data["code"].toString();

    QString tmp_style = style_subitem_basic;    // 기본 상단 MenuBar 스타일
    if(this->uiMode==AlbumDetail){
        tmp_style = style_subitem_albumDetail;  // 앨범 상세에서 Tab 메뉴로 사용할때 스타일 다르게 적용하기 위함
    }else if(this->uiMode==FriendManager){
        tmp_style = style_subitem_friendManager;  // 친구관리 에서 Tab 메뉴로 사용할때 스타일 다르게 적용하기 위함
    }else if(this->uiMode==ListSubMenuBar){
        tmp_style = style_subitem_listMenubar;
    }else if(this->uiMode==FriendSubmenubar){//cheon211018
        //print_debug();
        tmp_style = style_subitem_friendSubmenubar;
    }
    this->btn_icon = new QPushButton();//c230328
    this->btn_icon->setObjectName("btn_sub");
    this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:4px solid transparent; } #btn_sub:hover { border-bottom:4px solid #B18658;  }").arg(tmp_style));

    this->btn_icon->setText(p_data["name"].toString());//
    this->subMenuName = p_data["name"].toString();//c230321


    this->btn_icon->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *vlayout_total = new QVBoxLayout();
    vlayout_total->setContentsMargins(0,0,0,0);
    vlayout_total->setSpacing(0);
    vlayout_total->addWidget(this->btn_icon);
    this->setLayout(vlayout_total);

    // 커넥션
    connect(this->btn_icon, SIGNAL(clicked()), this, SLOT(clickedSubMenu()));//c230328
}
/**
 * @brief TopMenuSubItem::setSelectedStyle 현재 선택된 스타일로 세팅
 * @param p_flagSelected
 */
void TopMenuSubItem::setSelectedStyle(bool p_flagSelected){

    QString tmp_style = style_subitem_basic;
    QString tmp_style_friend = style_subitem_basic;
    if(this->uiMode==AlbumDetail){
        tmp_style = style_subitem_albumDetail;
    }else if(this->uiMode==FriendManager){
        tmp_style = style_subitem_friendManager;
    }else if(this->uiMode==ListSubMenuBar){
        tmp_style = style_subitem_listMenubar;
    }else if(this->uiMode==FriendSubmenubar){//cheon211018
        //print_debug();
        tmp_style = style_subitem_friendSubmenubar;
        tmp_style_friend = style_subitem_friendSubmenubar_hover;
    }
    if(p_flagSelected==true){
        if(this->uiMode==ListSubMenuBar){
            this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:2px solid #B18658; } #btn_sub:hover { border-bottom:2px solid #B18658;  }").arg(tmp_style));
        }else if(this->uiMode==FriendSubmenubar){//cheon211018
            this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:4px solid #B18658; } #btn_sub:hover {  %2 border-bottom:4px solid #B18658;   }").arg(tmp_style_friend).arg(tmp_style_friend));
        }else{
            this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:4px solid #B18658; } #btn_sub:hover { border-bottom:4px solid #B18658;  }").arg(tmp_style));
        }
    }else{
        if(this->uiMode==ListSubMenuBar){
            this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:2px solid transparent; } #btn_sub:hover { border-bottom:2px solid #B18658;  }").arg(tmp_style));
        }else if(this->uiMode==FriendSubmenubar){//cheon211018
            this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:4px solid transparent; } #btn_sub:hover { %2 border-bottom:4px solid #B18658;   }").arg(tmp_style).arg(tmp_style_friend));
        }else{
            this->btn_icon->setStyleSheet(QString("#btn_sub { %1 border-bottom:4px solid transparent; } #btn_sub:hover { border-bottom:4px solid #B18658;  }").arg(tmp_style));
        }
    }
}

/**
 * @brief TopMenuSubItem::clickedSubMenu [SLOT] 서브메뉴를 클릭했다는 시그널 발생
 */
void TopMenuSubItem::clickedSubMenu(){
    ToastMsg::delay(this,"", tr("delay"), 1000);//c230316
    emit clickedBtn(this->subMenuCode);
}
