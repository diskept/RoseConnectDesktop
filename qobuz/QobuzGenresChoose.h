#ifndef QOBUZGENRESCHOOSE_H
#define QOBUZGENRESCHOOSE_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "AbstractQobuzSubWidget.h"
#include "qobuz_struct.h"


#define print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief "QOBUZ > 탐색 > Genre View as List" 화면.
     */
    class QobuzGenresChoose : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzGenresChoose(QWidget *parent = nullptr);
        ~QobuzGenresChoose();

        void setActivePage() override;

    private slots:
        void slot_clickGenreBtn();

    private:
        // 데이터를 화면에 적용
        void applyUIControl_chooseGenres_fromlistData();
        QWidget* get_genrePushButton(const int p_index, const qobuz::GenreInfo& p_genreInfo);

    };
};

#endif // QOBUZGENRESCHOOSE_H
