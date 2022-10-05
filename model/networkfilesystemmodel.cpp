#include "networkfilesystemmodel.h"
#include <QJsonObject>

NetworkFileSystemModel::NetworkFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
    list_fileInfoStruct = new QList<FileInfoStruct>();

}

QVariant NetworkFileSystemModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && list_fileInfoStruct->count() > 0 ){
        if(index.isValid() && role == QFileSystemModel::FileIconRole){
            if(!list_fileInfoStruct->at(index.row()).flagReqDrawThumb
                    && list_fileInfoStruct->at(index.row()).isFile){
                FileInfoStruct tmp_struct = list_fileInfoStruct->takeAt(index.row());
                tmp_struct.flagReqDrawThumb = true;
                list_fileInfoStruct->insert(index.row(), tmp_struct);

                // 썸네일 Drwa 시그널 호출
                emit signal_reqThumbnail(tmp_struct.index,tmp_struct.filePath,tmp_struct.fileName);
            }
            return list_fileInfoStruct->at(index.row()).pixmapIcon;

        }else if(index.isValid() && role == Qt::UserRole){

            QJsonObject json;
            int tmp_index = index.row();

            if(tmp_index >=0 && tmp_index < list_fileInfoStruct->count()){

                if(list_fileInfoStruct->at(tmp_index).flagSetFileInfo == false){
                    FileInfoStruct fileInfoStruct = list_fileInfoStruct->takeAt(tmp_index);
                    fileInfoStruct.index = tmp_index;
                    fileInfoStruct.filePath = this->filePath(index);
                    fileInfoStruct.fileName = this->fileName(index);
                    fileInfoStruct.title = this->fileName(index);
                    fileInfoStruct.isFile = !this->isDir(index);
                    fileInfoStruct.pixmapIcon = fileInfoStruct.isFile ?  QPixmap(":images/def_mus_60.png") : QPixmap(":images/icon_folder_bic.png");
                    fileInfoStruct.flagReqDrawThumb = false;
                    fileInfoStruct.flagSetFileInfo = true;

                    list_fileInfoStruct->insert(tmp_index, fileInfoStruct);

                    // 파일인 경우 favorite 요청
                    if(fileInfoStruct.isFile){
                        emit signal_reqFavorites(tmp_index);
                    }
                }

                json.insert("index", tmp_index);
                json.insert("filePath", this->list_fileInfoStruct->at(tmp_index).filePath);
                json.insert("fileName", this->list_fileInfoStruct->at(tmp_index).fileName);
                json.insert("duration", this->list_fileInfoStruct->at(tmp_index).duration);
                json.insert("author", this->list_fileInfoStruct->at(tmp_index).author);
                json.insert("albumTitle", this->list_fileInfoStruct->at(tmp_index).albumTitle);
                json.insert("title", this->list_fileInfoStruct->at(tmp_index).fileName);
                json.insert("isFile", this->list_fileInfoStruct->at(tmp_index).isFile);
                json.insert("favorites", this->list_fileInfoStruct->at(tmp_index).favorites);
            }

            return json;
        }else if(index.isValid() && role == Qt::UserRole+4){
            return lastPressBtn;
        }
    }

    return QFileSystemModel::data(index, role);
}

bool NetworkFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role){

    if(role == Qt::UserRole+4){
        lastPressBtn = value.toInt();
        return true;
    }
    else if(role == Qt::UserRole){
        QJsonObject json = value.toJsonObject();

        int tmp_rowIndex = index.row();
        if(tmp_rowIndex >=0 && tmp_rowIndex < list_fileInfoStruct->count()){
            FileInfoStruct fileInfoStruct = list_fileInfoStruct->takeAt(tmp_rowIndex);
            fileInfoStruct.filePath = json["filePath"].toString();
            fileInfoStruct.fileName = json["fileName"].toString();
            fileInfoStruct.duration = json["duration"].toString();
            fileInfoStruct.author = json["author"].toString();
            fileInfoStruct.albumTitle = json["albumTitle"].toString();
            fileInfoStruct.title = json["title"].toString();
            fileInfoStruct.isFile = json["isFile"].toBool();
            fileInfoStruct.favorites = json["favorites"].toBool();

            list_fileInfoStruct->insert(tmp_rowIndex, fileInfoStruct);

            emit dataChanged(index, index);
        }
    }
    else if(role == Qt::DecorationRole){
        int tmp_rowIndex = index.row();
        if(tmp_rowIndex >=0 && tmp_rowIndex < list_fileInfoStruct->count()){
            FileInfoStruct fileInfoStruct = list_fileInfoStruct->takeAt(tmp_rowIndex);
            fileInfoStruct.pixmapIcon = value.value<QPixmap>();

            list_fileInfoStruct->insert(tmp_rowIndex, fileInfoStruct);

            emit dataChanged(index, index);
        }
    }

    return QFileSystemModel::setData(index, value, role);
}

int NetworkFileSystemModel::rowCount(const QModelIndex &parent) const
{
    return QFileSystemModel::rowCount(parent);
}

int NetworkFileSystemModel::columnCount(const QModelIndex &parent) const
{
    return QFileSystemModel::columnCount(parent);
}

void NetworkFileSystemModel::clearData(){
    lastPressBtn = 0;
    list_fileInfoStruct->clear();
}

void NetworkFileSystemModel::setFileInfoStruct(int p_size){
    for(int i = 0 ; i < p_size; i++){
        FileInfoStruct tmp_struct;
        tmp_struct.flagSetFileInfo = false;
        list_fileInfoStruct->append(tmp_struct);
    }
}
