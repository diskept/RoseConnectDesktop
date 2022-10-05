#ifndef MUSICFOLDER_COLUD_H
#define MUSICFOLDER_COLUD_H

#include <QWidget>
#include <QVBoxLayout>
#include "data/datacloud.h"

/**
 * @brief 음악 > 폴더 > 클라우드 @n
 *      클라우드 추가, 등록된 클라우드 목록으로 구성
 *
 * @note
 *      클라우드 상세 미구현 ==> API 확인 필요(Android App에서 음원재생 불가, 폴더 클릭시 같은 폴더 무한 이동)
 */
class MusicFolder_Colud : public QWidget
{
    Q_OBJECT
public:
    explicit MusicFolder_Colud(QWidget *parent = nullptr);
    ~MusicFolder_Colud();

    void requestCloudlist(bool flagRefresh = false);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedRow();
    void slot_clickedRemove();

private:

    QVBoxLayout *box_cloudList;
    QList<DataCloud*> *listCloud;

    void setInit();
    void setUIControl();
    void clearCloudList();
    void appendCloudUI(const int &p_index);
    void setResultOfCloudList(const QJsonObject &p_json);
    void showDlgOfSelectCloudType();
    void showDlgOfAddCloud(bool p_flagIsFTP = true);
    void requestRemoveCloud(const int &p_index);


};

#endif // MUSICFOLDER_COLUD_H
