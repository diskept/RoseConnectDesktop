#ifndef NETWORKFILESYSTEMMODEL_H
#define NETWORKFILESYSTEMMODEL_H

#include <QFileSystemModel>

/**
 * @brief 네트워크파일시스템 모델 @n
 * @note QFileSystemoModel 상속받아 필요한 데이터 struct로 사용
 *
 */
class NetworkFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

    struct FileInfoStruct{
        int index=0;
        QString filePath = "";
        QString fileName = "";
        QString duration = "";
        QString author = "";
        QString albumTitle = "";
        QString title = "";
        bool favorites = false;
        bool isFile = false;
        bool flagSetFileInfo = false;
        bool flagReqDrawThumb = false;
        QPixmap pixmapIcon;
    };

public:
    NetworkFileSystemModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void setFileInfoStruct(int p_size);
    void clearData();

signals:
    void signal_reqThumbnail(const int &p_index, const QString &p_filePath, const QString &p_fileName) const;
    void signal_reqFavorites(const int &p_idnex) const;

private:

    int lastPressBtn=0;
    QList<FileInfoStruct> *list_fileInfoStruct;



protected:

};

#endif // NETWORKFILESYSTEMMODEL_H
