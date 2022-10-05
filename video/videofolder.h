#ifndef VIDEOFOLDER_H
#define VIDEOFOLDER_H

#include <home/abstractpage.h>
#include <home/topmenubar.h>

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

/**
 * @brief 비디오  > 폴더
 * @note 음악-폴더와 공통
 */
class VideoFolder : public AbstractPage
{
    Q_OBJECT
public:
    explicit VideoFolder(QWidget *parent = nullptr);
    void setDataABSPage(QJsonObject p_data) override;

signals:

public slots:
    void slot_changedSubTabUI(const QJsonObject &p_data) override;

private :
    virtual QJsonObject getDataJson() override;
};

#endif // VIDEOFOLDER_H
