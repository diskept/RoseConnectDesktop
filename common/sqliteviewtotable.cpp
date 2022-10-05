#include "sqliteviewtotable.h"

#include <QJsonObject>
#include <common/sqlitehelper.h>
#include <QDateTime>

/**
 * @brief SqliteViewToTable::SqliteViewToTable : rose.db View를 Table로 저장
 * @details rose기기 DB 데이터중 View를 전부 Table화
 * 메모리 DB 미사용::DB 가상테이블 쿼리속도 낮음.
 * @param parent
 */
SqliteViewToTable::SqliteViewToTable(QObject *parent)
    :QObject(parent)
{
    setInit();
}

/**
 * @brief SqliteViewToTable::~SqliteViewToTable : 소멸자
 */
SqliteViewToTable::~SqliteViewToTable(){
}

/**
 * @brief SqliteViewToTable::setInit 초기 세팅
 * @details
 */
void SqliteViewToTable::setInit(){

    this->moveToThread(&thread);
    thread.start();

    connect(this, SIGNAL(signal_viewToTable()), SLOT(viewToTable()));
    QObject::connect(this, &SqliteViewToTable::signal_finished, [&](){
        thread.quit();
        thread.wait();
    });
}

/**
 * @brief SqliteViewToTable::viewToTable : rose db view to table 진행
 */
void SqliteViewToTable::viewToTable(){

    SqliteHelper *helper = new SqliteHelper;
    QSqlError err = helper->addConnectionRoseOld();

    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery += " SELECT * FROM sqlite_master WHERE type='view'";

        QVariantList dataList;
        helper->exec(strQuery, dataList);
        if(dataList.size() > 0){
            for(int i = 0 ; i < dataList.size(); i++){

                QJsonObject jsonData = dataList.at(i).toJsonObject();


                if(jsonData.contains("name") && jsonData.contains("sql")){
                    QString tmp_name = jsonData["name"].toString();
                    QString tmp_sql = jsonData["sql"].toString();


                    strQuery = "DROP VIEW IF EXISTS "+tmp_name;
                    helper->exec(strQuery);

                    helper->exec(tmp_sql.replace("CREATE VIEW","CREATE TABLE", Qt::CaseInsensitive));

                    QString tmp_createIndex;
                    if(tmp_name=="artist_info"){
                        tmp_createIndex = "CREATE INDEX idx_ai_id ON artist_info(_id)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_ai_artist ON artist_info(artist)";
                        helper->exec(tmp_createIndex);
                    }
                    else if(tmp_name=="audio"){
                        tmp_createIndex = "CREATE INDEX idx_audio_id ON audio(_id)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_audio_album_id ON audio(album_id)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_audio_artist_id ON audio(artist_id)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_audio_data ON audio(_data)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_audio_title ON audio(title)";
                        helper->exec(tmp_createIndex);
                    }
                    else if(tmp_name=="audio_genres_map_noid"){
                        tmp_createIndex = "CREATE INDEX idx_g_m_n_id ON audio_genres_map_noid(audio_id)";
                        helper->exec(tmp_createIndex);
                    }
                    else if(tmp_name=="audio_meta"){
                        tmp_createIndex = "CREATE INDEX idx_audio_meta_id ON audio_meta(_id)";
                        helper->exec(tmp_createIndex);
                    }
                    else if(tmp_name=="composer"){
                        tmp_createIndex = "CREATE INDEX idx_c_a_m_n_composer ON composer_albums_map_noid(composer)";
                        helper->exec(tmp_createIndex);
                    }
                    else if(tmp_name=="search"){
                        /*
                        tmp_createIndex = "CREATE INDEX idx_search_id ON search(_id)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_search_artist ON search(artist)";
                        helper->exec(tmp_createIndex);
                        */
                    }
                    else if(tmp_name=="searchhelpertitle"){

                    }
                    else if(tmp_name=="video"){
                        tmp_createIndex = "CREATE INDEX idx_video_id ON video(_id)";
                        helper->exec(tmp_createIndex);
                        tmp_createIndex = "CREATE INDEX idx_video_data ON search(_data)";
                        helper->exec(tmp_createIndex);
                    }
                }
            }
        }
    }

    QSqlDatabase::removeDatabase(helper->getDBName_Rose_Old());
    helper->close();
    delete helper;

    emit signal_finished();
}
