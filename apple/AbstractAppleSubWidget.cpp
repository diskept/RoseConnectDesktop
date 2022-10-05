#include "apple/AbstractAppleSubWidget.h"

#include "apple/ConvertData_forApple.h"
#include "apple/ProcCommon_forApple.h"
#include "apple/ProcRosePlay_withApple.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "widget/toastmsg.h"
#include "widget/NoData_Widget.h"

#include <QPixmapCache>
#include <QSettings>

namespace apple {

    /**
     * @brief Apple 서브페이지 위젯을 위한 부모 클래스
     * @param parent
     */
    AbstractAppleSubWidget::AbstractAppleSubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent) {

        this->linker = Linker::getInstance();
        connect(this->linker, SIGNAL(signal_Artist_movePage(QJsonObject)), SLOT(slot_dlg_signal_artist(QJsonObject)));
    }

    /**
     * @brief 소멸자 체크.
     */
    AbstractAppleSubWidget::~AbstractAppleSubWidget(){

        this->deleteLater();
    }


    /**
     * @brief APPLE 로그인이 필요하다는 화면 Widget을 반환한다. box_layout에 넣지는 않음
     * @return
     */
    QWidget* AbstractAppleSubWidget::get_createUIControl_notLoginView(){

        QLabel *label_topComment = new QLabel(tr("You can use it by logging in to your AppleMusic account."));
        label_topComment->setStyleSheet("font-size: 18px; color: #ffffff;");
        label_topComment->setContentsMargins(0,0,0,0);
        label_topComment->setAlignment(Qt::AlignCenter);

        QPushButton *btn_login = new QPushButton();
        btn_login->setText(tr("Log-in"));
        //btn_login->setText(tr("로그인"));
        btn_login->setFixedHeight(40);
        btn_login->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px; padding-left:44px; padding-right:44px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setContentsMargins(0,0,0,0);
        connect(btn_login, &QPushButton::clicked, this, &AbstractAppleSubWidget::slot_clickBtnLogin_toShowDialog);

        // 라벨과 버튼을 세로로 배치
        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addWidget(label_topComment);
        vBox->addSpacing(30);
        vBox->addWidget(btn_login, 0, Qt::AlignCenter);

        vBox->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

        // Widget
        QWidget *widget_output = new QWidget();
        widget_output->setContentsMargins(0,0,0,0);
        widget_output->setLayout(vBox);

        // return
        return widget_output;
    }


    /**
     * @brief "로그인" 버튼 클릭 처리 - 로그인 다이얼로그 띄우기
     */
    void AbstractAppleSubWidget::slot_clickBtnLogin_toShowDialog(){

        // TODO::Apple Login system pendin... change source code Added 07.01.2022 by Jeon
        /*this->dlbLogin = new DialogLogin_forQobuz(this);
        connect(this->dlbLogin, &DialogLogin_forQobuz::successLogin, this, &AbstractAppleSubWidget::slot_acceptedDialogLogin);
        this->dlbLogin->exec();*/

        this->dlgNotice = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OK);
        this->dlgNotice->setTitle(tr("NOTICE"));
        this->dlgNotice->setText(tr("Please proceed with AppleMusic Login through the Connect app."));

        this->dlgNotice->exec();
    }


    // MARK : about Filter Option Box -----------------------------------------------------------------------------------------------------

    /**
     * @brief My Collection 에서 사용하는 Filter Option Data 목록 반환함
     * @return
     */
    QList<bugs::FilterOptionData> AbstractAppleSubWidget::get_list_filterOptData_forMyCollection(){

        QList<bugs::FilterOptionData> list_output;

        bugs::FilterOptionData filterOpt_date;
        filterOpt_date.opt_code = QVariant::fromValue(ProcCommon::DataOrderOption::DATE);
        filterOpt_date.opt_name = tr("Date added");
        list_output.append(filterOpt_date);

        bugs::FilterOptionData filterOpt_name;
        filterOpt_name.opt_code = QVariant::fromValue(ProcCommon::DataOrderOption::NAME);
        filterOpt_name.opt_name = tr("A-Z");
        list_output.append(filterOpt_name);

        return list_output;

    }
}
