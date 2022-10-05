#include "dirfilelistpage.h"
#include "common/global.h"
#include <QFileInfoList>
#include <QTime>
#include <QScrollArea>
#include <QSplitter>
#include <QJsonDocument>

#include <common/gscommon.h>

const int ALBUMIMG_SIZE = 60;
/**
 * @brief ClickableDirRow::ClickableDirRow 생성자
 * @param p_iconPath
 * @param p_name
 * @param p_path
 * @param parent
 */


ClickableDirRow::ClickableDirRow(QString p_iconPath, QString p_name, QString p_path, QWidget *parent) : QWidget(parent)//c220609
{
    this->dirName = p_name;
    this->dirPath = p_path;
    this->setCursor(Qt::PointingHandCursor);

    QLabel *lb_icon = GSCommon::getUILabelImg(p_iconPath);
    QLabel *lb_text = new QLabel();
    lb_text->setText(p_name);
    lb_icon->setStyleSheet("background-color:transparent;");
    lb_text->setStyleSheet("font-size:16px;color:#E5E5E4;background-color:transparent;");
    QHBoxLayout *hl_row = new QHBoxLayout();
    hl_row->setContentsMargins(0,0,0,0);
    hl_row->setSpacing(0);
    hl_row->setAlignment(Qt::AlignLeft);
    hl_row->addWidget(lb_icon);
    hl_row->addSpacing(15);
    hl_row->addWidget(lb_text);

    QWidget *widget_row = new QWidget();
    widget_row->setObjectName("widget_row");
    widget_row->setStyleSheet("#widget_row { border-bottom:2px solid #282828; } #widget_row:hover { background-color:#333333; } ");
    widget_row->setLayout(hl_row);
    QVBoxLayout *vl_row = new QVBoxLayout();
    vl_row->setContentsMargins(33,0,38,0);
    vl_row->setSpacing(0);
    vl_row->addWidget(widget_row);

    this->setLayout(vl_row);
}

void ClickableDirRow::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);        // by sunnyfish
    emit signal_clicked(this->dirName, this->dirPath);
}


ListFileView::ListFileView(QWidget *parent) : QListView(parent){
}

void ListFileView::setRootIndex(const QModelIndex &index){
    QListView::setRootIndex(index);

    // 동일한 root 클릭할때도 갱신할 수 있게
    emit signal_setUI(index);
}

/**
 * @brief LocalFileRow::LocalFileRow 생성자
 * @param p_fileInfo
 * @param parent
 */
LocalFileRow::LocalFileRow(QFileInfo p_fileInfo, QWidget *parent) : QWidget(parent)//c220609
{
    this->fileInfo = p_fileInfo;

    m_player = new QMediaPlayer();
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));

    // 앨범이미지
    lb_albumImg = new QLabel();
    lb_albumImg->setFixedSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE);
    lb_title = new QLabel();
    lb_title->setObjectName("lb_title");
    lb_title->setStyleSheet("#lb_title { font-size:16px;color:#FFFFFF;background-color:transparent; }");
    lb_title->setWordWrap(true);
    //lb_title->setCursor(Qt::PointingHandCursor);
    lb_artist = new QLabel();
    lb_artist->setObjectName("lb_artist");
    lb_artist->setStyleSheet("#lb_artist { font-size:16px;color:#999999;background-color:transparent; }");
    lb_artist->setContentsMargins(0,5,0,0);
    lb_albumName = new QLabel();
    lb_albumName->setObjectName("lb_albumName");
    lb_albumName->setStyleSheet("#lb_albumName { font-size:16px;color:#FFFFFF;background-color:transparent; }");
    lb_duration = new QLabel();
    lb_duration->setObjectName("lb_duration");
    lb_duration->setStyleSheet("#lb_duration { font-size:16px;color:#FFFFFF;background-color:transparent; }");

    QVBoxLayout *vl_title = new QVBoxLayout();
    vl_title->setContentsMargins(0,0,0,0);
    vl_title->setSpacing(0);
    vl_title->setAlignment(Qt::AlignVCenter);
    vl_title->addWidget(this->lb_title);
    //vl_title->addWidget(this->lb_artist);

    // 버튼 3개
    QPushButton *btn_icon_menu = GSCommon::getUIBtnImg("btn_icon_menu", ":/images/icon_menu.png");
    btn_icon_menu->setFixedSize(60,60);
    btn_icon_menu->setCursor(Qt::PointingHandCursor);
    btn_icon_menu->setProperty("code", "more");

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(38,0,0,0);
    hl_total->setSpacing(0);
    hl_total->setAlignment(Qt::AlignVCenter);
    hl_total->addWidget(lb_albumImg);
    hl_total->addSpacing(20);
    hl_total->addLayout(vl_title, 8);
    hl_total->addWidget(lb_artist, 5);
    hl_total->addWidget(lb_duration, 2);
    //hl_total->addSpacing(118);
    hl_total->addWidget(btn_icon_menu);

    QWidget *widget = new QWidget();
    widget->setObjectName("trackInfo");
    widget->setCursor(Qt::PointingHandCursor);
    widget->setLayout(hl_total);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget);

    this->setStyleSheet("#trackInfo { border-bottom:1px solid #2A2A2A; }");
    this->setFixedHeight(70);
    this->setLayout(vl_total);

    // 커넥트
    connect(btn_icon_menu, SIGNAL(clicked()), this, SLOT(slot_showOptionPopup()));

    m_player->setMedia(QUrl::fromLocalFile(this->fileInfo.filePath()));
    /*
    //this->fileInfo_filePath = this->fileInfo.filePath();
    //qDebug()<< "this->fileInfo.filePath() = " << this->fileInfo.filePath();
    //this->lb_title->setText(this->fileInfo.fileName());
    */

}

void LocalFileRow::slot_showOptionPopup(){
    print_debug();
}

/**
 * @brief LocalFileRow::onMediaStatusChanged 메타정보를 이용하여 데이터 세팅한다.
 * @param status
 */
void LocalFileRow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
   // print_debug();
    if (status == QMediaPlayer::LoadedMedia){


        QTime tmp_duration = QTime::fromMSecsSinceStartOfDay(m_player->metaData("Duration").toInt()); // QTime("00:03:27.700")
      //  qDebug() << "tmp_duration = " << tmp_duration;
        //this->lb_title->setText(this->fileInfo.fileName());
        this->lb_title->setText(m_player->metaData("Title").toString()+" ("+ m_player->metaData("AlbumTitle").toString()+")");
        if(m_player->metaData("Title").toString()==nullptr){
            this->lb_title->setText(this->fileInfo.fileName());
        }
        this->lb_artist->setText(m_player->metaData("Author").toString());

        this->lb_albumName->setText(m_player->metaData("AlbumTitle").toString());
        //qDebug()<< "m_player->metaData(Author).toString()=" << m_player->metaData("Author").toString();
        //qDebug()<< "m_player->metaData(AlbumTitle).toString()=" << m_player->metaData("AlbumTitle").toString();
        this->lb_duration->setText(tmp_duration.toString("hh:mm:ss"));
        QImage image = m_player->metaData("ThumbnailImage").value<QImage>();
        if(image.isNull()==true){
            this->lb_albumImg->setPixmap(*GSCommon::getUIPixmapImg(":/images/def_mus_60.png"));
        }else{
            QPixmap pixmap_icon = QPixmap::fromImage(image);
            pixmap_icon = pixmap_icon.scaled(ALBUMIMG_SIZE, ALBUMIMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            this->lb_albumImg->setPixmap(pixmap_icon);
        }

    }
    m_player->destroyed();
    //QSplitter *tt = new QSplitter();
    //tt->refresh();
}

void LocalFileRow::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);        // by sunnyfish

    QString p_index = this->property("index").toString();
    emit signal_clicked(p_index);
}


/**
 * @brief DirFileListPage::DirFileListPage 생성자
 * @param p_fileType
 * @param parent
 */
DirFileListPage::DirFileListPage(ValiedFileType p_fileType, QWidget *parent) : QWidget(parent)
{
    linker = Linker::getInstance();
    this->treeView = new QTreeView();
    this->listView = new ListFileView();

    QStringList filterToApply;
    if(p_fileType==ValiedFileType::Audio){
        filterToApply.append("*.mp3"); // 테스트 ok
        filterToApply.append("*.wav");
        filterToApply.append("*.wma");
        filterToApply.append("*.flac");
    }else if(p_fileType==ValiedFileType::Video){
        filterToApply.append("*.mp4"); // 테스트 ok
        filterToApply.append("*.avi");
        filterToApply.append("*.mov");
        filterToApply.append("*.mpg");
        filterToApply.append("*.mkv");
        filterToApply.append("*.wmv");
    }

    QString sPath = "";
    //sPath = "F:/";
    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    filemodel = new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    filemodel->setNameFilters(filterToApply);
    filemodel->setNameFilterDisables(false);

    dirmodel->setRootPath(sPath);
    filemodel->setRootPath(sPath);
    treeView->setModel(dirmodel);
    listView->setModel(filemodel);
    treeView->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    vl_list = new QVBoxLayout();
    vl_list->setContentsMargins(0,0,0,30);
    vl_list->setSpacing(0);
    vl_list->setAlignment(Qt::AlignTop);

    QWidget *wg_init = new QWidget();
    wg_init->setLayout(vl_list);


    // 스크롤
    QScrollArea *page_init = new QScrollArea();
    page_init->setObjectName("scrollArea");
    page_init->setWidget(wg_init);
    page_init->setWidgetResizable(true);
    page_init->setStyleSheet("#scrollArea { border:0px; }");
    page_init->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vl_total= new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(page_init);


    this->setLayout(vl_total);

    //connect(linker, SIGNAL(signal_playlist_reflesh()), this, SLOT(slot_playlist_reflesh()));
    // 커넥션
    connect(filemodel, SIGNAL(directoryLoaded(QString)), this, SLOT(slot_directoryLoadedFile(QString)));
    connect(listView, SIGNAL(signal_setUI(QModelIndex)), this, SLOT(slot_setUI(QModelIndex)));
}


void DirFileListPage::slot_playlist_reflesh(){
    print_debug();
    if(global.desktopPlayerMode){
        if(this->pre_playIndex >=0){
            QLayoutItem *tt1 = this->vl_list->itemAt(this->pre_playIndex);
            tt1->widget()->setStyleSheet("background-color:transparent; ");
        }
        curr_playIndex = global.music_playlist->currentIndex();
        QLayoutItem *tt1 = this->vl_list->itemAt(curr_playIndex);
        tt1->widget()->setStyleSheet("background-color:#aaaaaa; ");

        pre_playIndex = curr_playIndex;
    }

}
/**
 * @brief DirFileListPage::setInitDriver 로컬 드라이버 세팅
 */
void DirFileListPage::setInitDriver(){

    GSCommon::clearLayout(this->vl_list);

    QFileInfoList tmp_drives = QDir::drives();
    print_debug();
    for(int i=0; i<tmp_drives.count(); i++){
      //  print_debug();
        QFileInfo tmp_drive = tmp_drives.at(i);
      //  qDebug() << "tmp_drive=" << tmp_drive;


        ClickableDirRow *clickableRow_folder = new ClickableDirRow(":/images/icon_folder_bic.png", tmp_drive.filePath(), tmp_drive.filePath());
        this->vl_list->addWidget(clickableRow_folder);
        connect(clickableRow_folder, SIGNAL(signal_clicked(QString, QString)), this, SLOT(slot_clickedDirName(QString, QString)));
    }
}

/**
 * @brief DirFileListPage::setInitNetworkDriver : 루트 파일 경로 세팅
 * @details 세팅한 파일 경로부터 파일 트리 시작
 * @param p_hostName QString smb 호스트명
 * @param p_path Qstring smb share path
 */
void DirFileListPage::setInitNetworkDriver(const QString &p_hostName, const QString &p_path){
    Q_UNUSED(p_hostName);       // by sunnyfish

    GSCommon::clearLayout(this->vl_list);

    listView->setRootIndex(filemodel->setRootPath(p_path));
}




/**
 * @brief DirFileListPage::slot_directoryLoadedFile [SLOT]
 * @note QDirModel(구식)과 달리 QFileSystemModel은 별도의 스레드를 사용하여 자체를 채우므로 파일 시스템을 쿼리 할 때 기본 스레드가 중단되지 않는다...
 *       모델이 디렉토리를 채울 때 까지 기다렸다가 채워야한다.
 * @param path
 */
void DirFileListPage::slot_directoryLoadedFile(const QString &path){
print_debug();
    QModelIndex parentIndex = filemodel->index(path);
    //this->currentQModelIndex = parentIndex;//c220704
    this->slot_setUI(parentIndex);
}

/**
 * @brief DirFileListPage::slot_setUI 동일한 path를 열때는 QFileSystemModel 재Loaded하지 않으므로 강제로 다시 세팅해야한다.
 * @param p_index
 */
void DirFileListPage::slot_setUI(const QModelIndex &p_index){//c220704
print_debug();
    // 레이아웃 클리어
    GSCommon::clearLayout(this->vl_list);

    QList<QString> path_list;
    int numRows = filemodel->rowCount(p_index);
    this->currentQModelIndex = p_index;//c220704
    qDebug()<<"numRows = "<< numRows;
    global.musicPlaylistMediaContent.clear();
    fileList.clear();
    for (int row = 0; row < numRows; ++row) {
        QModelIndex childIndex = filemodel->index(row, 0, p_index);
        QString tmp_fileName = filemodel->fileName(childIndex);
        QFileInfo tmp_fileInfo = filemodel->fileInfo(childIndex);
        QString tmp_filePath = filemodel->fileInfo(childIndex).absoluteFilePath();

        if(tmp_fileInfo.isDir()){
            ClickableDirRow *clickableRow_folder = new ClickableDirRow(":/images/icon_folder_bic.png", tmp_fileName, tmp_filePath);
            connect(clickableRow_folder, SIGNAL(signal_clicked(QString, QString)), this, SLOT(slot_clickedDirName(QString, QString)));
            this->vl_list->addWidget(clickableRow_folder);
        }else if(tmp_fileInfo.isFile()){
           // print_debug();
            //global.musicPlaylistMediaContent.append(QUrl::fromLocalFile(tmp_filePath));

            fileList.append(tmp_filePath);
            LocalFileRow *localFileRow = new LocalFileRow(tmp_fileInfo);
            localFileRow->setProperty("index", row);
            connect(localFileRow, SIGNAL(signal_clicked(QString)), this, SLOT(slot_clickedFileName(QString)));//c220609
            this->vl_list->addWidget(localFileRow);
        }
    }
}
/*
void DirFileListPage::slot_clickedFileName(QString index){
    print_debug();
    qDebug()<<"index = "<<index;

    if(this->pre_playIndex >=0){
        QLayoutItem *tt1 = this->vl_list->itemAt(this->pre_playIndex);
        tt1->widget()->setStyleSheet("background-color:transparent; ");
    }


    this->curr_playIndex = index.toInt();

    QLayoutItem *tt1 = this->vl_list->itemAt(index.toInt());
    tt1->widget()->setStyleSheet("background-color:#aaaaaa; ");

    this->pre_playIndex = this->curr_playIndex;

    emit signal_fileInfo_filePath_play(index);

}
*/

void DirFileListPage::slot_clickedFileName(QString index){
    print_debug();
    qDebug()<<"index = "<<index;

    if(this->pre_playIndex >=0){
        QLayoutItem *tt1 = this->vl_list->itemAt(this->pre_playIndex);
        tt1->widget()->setStyleSheet("background-color:transparent; ");
    }


    this->curr_playIndex = index.toInt();

    QLayoutItem *tt1 = this->vl_list->itemAt(index.toInt());
    tt1->widget()->setStyleSheet("background-color:#aaaaaa; ");

    //int numRows = filemodel->rowCount( this->currentQModelIndex);
    //qDebug()<<"numRows = "<< numRows;

     int pre_cnt = this->select_table("dtPlayList_table");
     qDebug() << "pre_cnt=" << pre_cnt;
     int cnt = pre_cnt;
//
    for (int row = 0; row < fileList.count(); row++) {
        qDebug() << "row= " << row;

        QString tmp_filePath = fileList.at(row);
        if(row >= index.toInt()){
            qDebug()<<"tmp_filePath = "<< tmp_filePath;
            dt_playlist_insertDB(row, tmp_filePath);


            global.music_playlist->insertMedia(cnt++, QUrl::fromLocalFile(tmp_filePath));
       }



    }


    int next_cnt = this->select_table("dtPlayList_table");
    qDebug() << "next_cnt=" << next_cnt;

    this->pre_playIndex = this->curr_playIndex;

    qDebug() << "global.music_playlist->mediaCount()=" << global.music_playlist->mediaCount();

    emit signal_fileInfo_filePath_play(QString("%1").arg(pre_cnt));



}


void DirFileListPage::insert_table(int row, QString tmp_filePath, QString str_table){

    SqliteHelper *sqliteHelper = new SqliteHelper();
    QSqlError err = sqliteHelper->addConnectionLocal();
    QVariantList datad = QVariantList();
    if(err.type() == QSqlError::NoError){
        datad = QVariantList();


        if(datad.size() > 0){
            qDebug() << "datad.size()=" << datad.size();
        }else{
            print_debug();
            if(tmp_filePath.size() > 0){//c220628
                print_debug();
                // SqliteHelper *sqliteHelper = new SqliteHelper();
                QString strQuery_insert = QString("INSERT INTO %1 (row, filePath) ").arg(str_table);
                strQuery_insert += " VALUES ";
                strQuery_insert += " ('" + QString::number(row)+"','"+ tmp_filePath + "'";
                strQuery_insert += ") ";
                qDebug() << "strQuery_insert="<<strQuery_insert;
                sqliteHelper->exec(strQuery_insert);
                //c220619-search
            }else{
                print_debug();
            }

        }

    }else{
        print_debug();
    }
    sqliteHelper->close();
    delete sqliteHelper;
}





int DirFileListPage::select_table(QString str_table){
    print_debug();
    qDebug() << "str_table=" << str_table;
    SqliteHelper *sqliteHelperSel = new SqliteHelper();
    QSqlError err = sqliteHelperSel->addConnectionLocal();
    QVariantList data = QVariantList();
    if(err.type() == QSqlError::NoError){
        print_debug();
        QString strQuery_select = "SELECT  *";
        strQuery_select += " FROM ";
        strQuery_select += QString(" %1").arg(str_table);

        qDebug() << "strQuery_insert="<<strQuery_select;
        sqliteHelperSel->exec(strQuery_select, data);
        qDebug() << str_table << " table:data.size()=" << data.size();

        if(data.size() > 0){
            for(int i =0; i < data.size(); i++){
               // QJsonObject jsonData = data.at(i).toJsonObject();
               // QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " DirFileListPage::select_table---jsonData_obj = " << strJson;
            }
            print_debug();

        }else{
            print_debug();

        }

    }
    sqliteHelperSel->close();
    delete sqliteHelperSel;
    return data.size();
}

void DirFileListPage::dt_playlist_insertDB(int row, QString tmp_filePath){//c220704
    print_debug();

    insert_table(row, tmp_filePath, "dtPlayList_table");


}


/**
 * @brief DirFileListPage::slot_clickedDirName [SLOT] 디렉토리명 click시
 * @param p_dirName
 * @param p_dirPath
 */
void DirFileListPage::slot_clickedDirName(QString p_dirName, QString p_dirPath){
print_debug();

    // 상단 타이틀 디렉토리명 변경 요청
    emit signal_clickedDirName(p_dirName, p_dirPath);

    listView->setRootIndex(filemodel->setRootPath(p_dirPath));
}
