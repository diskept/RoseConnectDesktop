#ifndef MUSICFOLDER_EDITFAVOR_H
#define MUSICFOLDER_EDITFAVOR_H

#include <QWidget>
#include <QVBoxLayout>
#include "common/linker.h"

/**
 * @brief > 폴더 메뉴에서의 즐겨찾기 편집 화면 @n
 *      즐겨찾기 삭제 가능
 *
 * @note 즐겨찾기명, 순서 변경 미구현 ==> API 미지원
 */
class MusicFolder_EditFavor : public QWidget
{
    Q_OBJECT
public:
    explicit MusicFolder_EditFavor(QWidget *parent = nullptr);
    void setFavorData(const QJsonArray &p_jsonArrFavor);

signals:

private slots:
    void slot_clickedRemoveFavorFolder();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    QVBoxLayout *boxFavorGroup;

    void setInit();
    void setUIControl();
    void appendFavoriteUI(const QJsonObject &p_jsonData);
    void requestFavorList();
    void requestRemoveFavorFolder(const QString &p_name, const QString &p_path, const QString &p_pathKey);
    void setResultOfFavorList(const QJsonObject &p_jsonObject);
    void setResultOfRemoveFavorFolder(const QJsonObject &p_jsonObject);

};

#endif // MUSICFOLDER_EDITFAVOR_H
