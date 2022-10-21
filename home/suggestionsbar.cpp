#include "suggestionsbar.h"
#include "common/gscommon.h"
#include "common/global.h"//c220629
#include <QLabel>
#include <QJsonObject>
#include "common/sqlitehelper.h"//c220619-search
#include "common/ProcJsonEasy.h"//c220619-search
#include <QJsonDocument>//c220619-search


const int SEARCHLISTMAX_CNT = 100;//c220629

/**
 * @brief SuggestionsBar::SuggestionsBar 검색 제안어 목록
 * @param parent
 * @note Google suggestion search 결과 QWidget @n
 *      @sa Linker::signal_search() 를 통해 App에 검색 요청
 */
SuggestionsBar::SuggestionsBar(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief SuggestionsBar::setInit : 초기 세팅
 */
void SuggestionsBar::setInit(){
    linker = Linker::getInstance();
    delegate = new SuggestionDelegate(this);
}

/**
 * @brief SuggestionsBar::setUIControl : UI 세팅
 */
void SuggestionsBar::setUIControl(){
    // -------------------------------------------------
    //  검색 suggestions
    // -------------------------------------------------
    /*
    box_suggestions = new QVBoxLayout;
    box_suggestions->setSpacing(0);
    box_suggestions->setContentsMargins(15,15,15,15);
    box_suggestions->setAlignment(Qt::AlignTop);
*/
#if defined(Q_OS_WIN)//c220913_2
    listWidget = new QListWidget;
    listWidget->setItemDelegate(delegate);
    listWidget->setContentsMargins(0,0,0,0);
    listWidget->setFixedWidth(500);
    listWidget->setStyleSheet("background-color:#333333;border:none;border-radius:20px; " );
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //listWidget->setMouseTracking(true);//c220620
#endif
#if defined(Q_OS_MAC)//c220913_2
    listWidget = new QListWidget;
    listWidget->setItemDelegate(delegate);
    listWidget->setContentsMargins(0,0,0,0);
    listWidget->setFixedWidth(500);

    //listWidget->setStyleSheet("QListWidget::item:hover {background-color:#111111;}; " );//c221020_1
    listWidget->setStyleSheet("QListWidget{background-color:#333333;border:none;border-radius:20px;} QListWidget::item:hover {background-color:#ffffff;};" );//c221020_1
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setMouseTracking(true);//c220620
#endif



    QWidget *widget_suggestions = new QWidget;
    widget_suggestions->setContentsMargins(0,0,0,0);
    widget_suggestions->setStyleSheet("background-color:#333333;border:none;border-radius:20px;");
    widget_suggestions->setFixedHeight(500);


    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setContentsMargins(0,0,0,0);
    box_total->addWidget(listWidget);

    this->setLayout(box_total);

    // connect
    //connect(listWidget, &QListWidget::itemClicked, this, &SuggestionsBar::slot_suggestionClicked);//c220629
    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
    connect(linker, SIGNAL(signal_sugestionbarDown(QString)), this, SLOT(slot_suggestionBarDown(QString)));//c220904_1
    connect(linker, SIGNAL(signal_sugestionbarUp(QString)), this, SLOT(slot_suggestionBarUp(QString)));//c220904_1


}
//signal_clickListBtn()
void SuggestionsBar::slot_suggestionBarDown(QString text){//c220904_1
    print_debug();
    //global->downKeyFlag = true;
    //emit linker->signal_clickListBtn();

    this->show();
    qDebug() << "text=" << text;

    if(listWidget->count() > this->positionSel+1){
        this->positionSel++;
    }
    qDebug() << "this->positionSel=" << this->positionSel;
    if(this->positionSel > 0){
        QListWidgetItem *item0 = listWidget->item(this->positionSel-1);
        item0->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
        //item1->setBackground(QBrush("#FFFFFF"));
        QMap<QString,QVariant> map0 = item0->data(Qt::UserRole).toMap();
        QJsonObject json0;
        json0.insert("index", this->positionSel-1);
        json0.insert("old", true);
        json0.insert("suggestion", map0["suggestion"].toString());
        item0->setData(Qt::UserRole, json0);
        item0->setSelected(false);
    }


    QListWidgetItem *item1 = listWidget->item(this->positionSel);
    item1->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);

    QMap<QString,QVariant> map = item1->data(Qt::UserRole).toMap();
    QJsonObject json;
    json.insert("index", this->positionSel);
    json.insert("old", false);
    json.insert("suggestion", map["suggestion"].toString());
    item1->setData(Qt::UserRole, json);
    item1->setSelected(true);
    item1->setBackground(QBrush("#FFFFFF"));

    qDebug() << "map[suggestion].toString()= " << map["suggestion"].toString();
    if(this->positionSel == 0){
        global.savetypyingStr = text;
    }
    emit linker->signal_searchCopy(map["suggestion"].toString());
    listWidget->setCurrentRow(this->positionSel);

    qDebug() << "global.savetypyingStr=" << global.savetypyingStr;


}

void SuggestionsBar::slot_suggestionBarUp(QString text){//c220904_1
    print_debug();

    qDebug() << "global.savetypyingStr=" << global.savetypyingStr;
    qDebug() << "this->positionSel=" << this->positionSel;
    //global->downKeyFlag = true;

    if(0 <= this->positionSel){

        QListWidgetItem *item0 = listWidget->item(this->positionSel);
        item0->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);

        QMap<QString,QVariant> map0 = item0->data(Qt::UserRole).toMap();
        QJsonObject json0;
        json0.insert("index", this->positionSel);
        json0.insert("old", true);
        json0.insert("suggestion", map0["suggestion"].toString());
        item0->setData(Qt::UserRole, json0);
        item0->setSelected(false);
        this->positionSel--;
    }

    qDebug() << "this->positionSel=" << this->positionSel;

    if(this->positionSel >= 0){
        print_debug();
        qDebug() << "this->positionSel=" << this->positionSel;
        QListWidgetItem *item1 = listWidget->item(this->positionSel);
        item1->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
        QMap<QString,QVariant> map = item1->data(Qt::UserRole).toMap();
        QJsonObject json;
        json.insert("index", this->positionSel);
        json.insert("old", false);
        json.insert("suggestion", map["suggestion"].toString());
        item1->setData(Qt::UserRole, json);
        item1->setSelected(true);

        qDebug() << "map[suggestion].toString()= " << map["suggestion"].toString();

        emit linker->signal_searchCopy(map["suggestion"].toString());
        listWidget->setCurrentRow(this->positionSel);
    }else{
        listWidget->setCurrentRow(-1);
        emit linker->signal_searchCopy(global.savetypyingStr);
        return;
    }
}


void SuggestionsBar::proc_selOldDBDel(int idx_del){//c220629
    print_debug();
    qDebug() << "idx_del=" << idx_del;

    QString strQuery_delete = "DELETE ";
    strQuery_delete += " FROM ";
    strQuery_delete += " searchPreList";
    strQuery_delete += QString(" WHERE (idx == '%1'").arg(idx_del);
    //strQuery_delete += " WHERE (idx == idx";
    strQuery_delete += ")";
    //qDebug() << "strQuery_delete=" << strQuery_delete;

    SqliteHelper *sqliteHelperDel = new SqliteHelper();
    QSqlError err = sqliteHelperDel->addConnectionLocal();
    QVariantList datad = QVariantList();
    if(err.type() == QSqlError::NoError){
        datad = QVariantList();
        sqliteHelperDel->exec(strQuery_delete, datad);
        //qDebug() << "datad.size()=" << datad.size();
       // print_debug();
        //qDebug() << datad;
    }
    sqliteHelperDel->close();
    delete sqliteHelperDel;

}



void SuggestionsBar::proc_selOldDBSel(int idx_sel){//c220629

    //this->searchList = new QList<search_textData>();

    SqliteHelper *sqliteHelperSel = new SqliteHelper();
    QSqlError err = sqliteHelperSel->addConnectionLocal();
    QVariantList data = QVariantList();
    if(err.type() == QSqlError::NoError){
        print_debug();
        QString strQuery_select = "SELECT  *";
        strQuery_select += " FROM ";
        strQuery_select += " searchPreList";

        sqliteHelperSel->exec(strQuery_select, data);

        if(data.size() > 0){
            //qDebug() << "data.size()=" << data.size();
            //qDebug() << "data=" << data;

            //print_debug();
            this->searchList.clear();
            for(int i = data.size()-1 ; i >= 0  ; i--){
                //print_debug();
                QJsonObject tmp_data = data.at(i).toJsonObject();
                //print_debug();
                //QJsonDocument doc(tmp_data);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSuggestionsBar::proc_selOldDBSel=" << strJson  << "\n";
                QMap<QString, QVariant> map = data.at(i).toMap();

                search_textData data_output;
                data_output.index = i;
                data_output.idx = map["idx"].toInt();
                data_output.search_text = map["search_text"].toString();

                //qDebug() << "data_output.idx=" << data_output.idx;
                //qDebug() << "data_output.search_text=" << data_output.search_text;

                this->searchList.append(data_output);


            }
            //setSuggestions_old();//c220710
           // print_debug();


        }else{

        }
    }
    sqliteHelperSel->close();
    delete sqliteHelperSel;
    //print_debug();
    //qDebug() << "idx_sel=" << idx_sel;
    //qDebug() << "this->searchList->count()=" << this->searchList.count();
    //qDebug() << "this->searchList->at(idx_sel).idx=" << this->searchList.at(idx_sel).idx;
    if(idx_sel >= 0 && idx_sel < this->searchList.count() ){//c220710
        proc_selOldDBDel(this->searchList.at(idx_sel).idx);
        emit linker->signal_searchBarFocusChanged(true);
    }
    print_debug();

}

void SuggestionsBar::slot_delegateClicked(const int &p_index, const int &p_btnType){//c220703

    if(p_btnType == SuggestionDelegate::BtnType::etc){//index = 1
        print_debug();
        qDebug() << "p_index=" << p_index;
        QListWidgetItem *item1 = listWidget->item(p_index);
        QMap<QString,QVariant> map = item1->data(Qt::UserRole).toMap();
        emit linker->signal_search(map["suggestion"].toString());
        emit linker->signal_sugestionbarOld_sel(map["suggestion"].toString());
    }
    else if(p_btnType == SuggestionDelegate::BtnType::del){//index = 0
        print_debug();
        qDebug() << "p_index=" << p_index;
        //QListWidgetItem *item1 = listWidget->item(p_index);
       // listWidget->removeItemWidget(item1);
        listWidget->takeItem(p_index);
        //linker->signal_searchBarFocusChanged(false);//c220703
        //linker->signal_searchBarFocusChanged(true);//c220703
        proc_selOldDBSel(p_index);

    }


}


/**
 * @brief SuggestionsBar::setSuggestions : 검색 제안 데이터 세팅
 * @param p_jsonArray
 */
void SuggestionsBar::setSuggestions(QJsonArray p_jsonArray){//c220620

    //GSCommon::clearLayout(box_suggestions);
    listWidget->clear();
    print_debug();



    for(int i = 0 ; i < p_jsonArray.size(); i++){
        //print_debug();
        QJsonObject json;
        json.insert("index", i);
        json.insert("old", true);//c220904_1
        json.insert("suggestion", p_jsonArray.at(i).toString());
        QListWidgetItem *item1 = new QListWidgetItem;
        item1->setData(Qt::UserRole, json);
        listWidget->addItem(item1);
    }


}
/**
 * @brief SuggestionsBar::setSuggestions : 검색 제안 데이터 세팅
 * @param p_jsonArray
 */
void SuggestionsBar::setSuggestions_old(){//c220620

    //GSCommon::clearLayout(box_suggestions);
    listWidget->clear();
    print_debug();

    SqliteHelper *sqliteHelperSel = new SqliteHelper();
    QSqlError err = sqliteHelperSel->addConnectionLocal();
    QVariantList data = QVariantList();
    if(err.type() == QSqlError::NoError){
        print_debug();
        QString strQuery_select = "SELECT  *";
        strQuery_select += " FROM ";
        strQuery_select += " searchPreList";

        sqliteHelperSel->exec(strQuery_select, data);
        if(data.size() > 0){
            qDebug() << "data.size()=" << data.size();



        }else{

        }

    }
    sqliteHelperSel->close();
    delete sqliteHelperSel;


    int itemp = 0;
    int cnt =0;
    //qDebug() << "data.size()=" << data.size();
    //this->setGeometry(global.LmtCnt_W, -577, 70, 500, global.LmtCnt_H-70-115);
    for(int i = data.size()-1 ; i >= 0; i--){
        //print_debug();
        if(cnt++ < SEARCHLISTMAX_CNT){
            QJsonObject json;
            json.insert("index", itemp++);
            json.insert("old", true);
            QJsonObject jsonData = data.at(i).toJsonObject();
            //print_debug();
            //QJsonDocument doc(jsonData);    QString strJson(doc.toJson(QJsonDocument::Compact));    qDebug() << "SuggestionsBar::setSuggestions = " << strJson;

            json.insert("suggestion", ProcJsonEasy::getString(jsonData, "search_text"));
            QListWidgetItem *item1 = new QListWidgetItem;
            item1->setData(Qt::UserRole, json);

            item1->setBackground(QColor(255, 0, 0, 50));
            listWidget->addItem(item1);
        }else{
            //print_debug();
            //qDebug() << data;


            QJsonObject jsonData = data.at(i).toJsonObject();
            int  idx_del = ProcJsonEasy::getInt(jsonData, "idx");
            //qDebug() << "idx_del=" << idx_del;
            QString tt = QString("%1").arg(idx_del);
            QString strQuery_delete = "DELETE ";
            strQuery_delete += " FROM ";
            strQuery_delete += " searchPreList";
            strQuery_delete += QString(" WHERE (idx == '%1'").arg(idx_del);
            //strQuery_delete += " WHERE (idx == idx";
            strQuery_delete += ")";
            //qDebug() << "strQuery_delete=" << strQuery_delete;

            SqliteHelper *sqliteHelperDel = new SqliteHelper();
            QSqlError err = sqliteHelperDel->addConnectionLocal();
            QVariantList datad = QVariantList();
            if(err.type() == QSqlError::NoError){
                datad = QVariantList();
                sqliteHelperDel->exec(strQuery_delete, datad);
               // print_debug();
                //qDebug() << datad;
            }
            sqliteHelperDel->close();
            delete sqliteHelperDel;

            SqliteHelper *sqliteHelperSel = new SqliteHelper();
            QSqlError err2 = sqliteHelperSel->addConnectionLocal();
            QVariantList datas = QVariantList();
            if(err2.type() == QSqlError::NoError){
                //print_debug();
                QString strQuery_select = "SELECT  *";
                strQuery_select += " FROM ";
                strQuery_select += " searchPreList";


                sqliteHelperSel->exec(strQuery_select, datas);
                if(datas.size() > 0){
                   // qDebug() << "data.size()=" << datas.size();
                    //qDebug() << datas;


                }else{

                }

            }
            sqliteHelperSel->close();
            delete sqliteHelperSel;

        }
    }

}

/**
 * @brief SuggestionsBar::slot_suggestionClicked :[슬롯] 제시어 클릭
 * @param p_item QListWidgetItem
 * @note 검색 요청 시그널 발생
 */
void SuggestionsBar::slot_suggestionClicked(QListWidgetItem *p_item){
print_debug();
    QMap<QString,QVariant> map = p_item->data(Qt::UserRole).toMap();
    emit linker->signal_search(map["suggestion"].toString());
}
