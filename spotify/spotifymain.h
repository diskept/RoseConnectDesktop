#ifndef SPOTIFYMAIN_H
#define SPOTIFYMAIN_H

#include <QWidget>

#include <home/abstractmaincontent.h>


class SpotifyMain : public AbstractMainContent
{
    Q_OBJECT
public:
    explicit SpotifyMain(QWidget *parent = nullptr);

signals:

protected:
    void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

private :
    // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요

    // 메뉴 관련 페이지  ----------------------


    void setUIControl();
};

#endif // SPOTIFYMAIN_H
