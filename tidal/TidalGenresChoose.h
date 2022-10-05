#ifndef TIDALGENRESCHOOSE_H
#define TIDALGENRESCHOOSE_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "AbstractTidalSubWidget.h"
#include "tidal_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief "TIDAL > 탐색 > Genre View as List" 화면.
     */
    class TidalGenresChoose : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalGenresChoose(QWidget *parent = nullptr);
        ~TidalGenresChoose();

        void setActivePage() override;

    private slots:
        void slot_clickGenreBtn();

    private:
        // 데이터를 화면에 적용
        void applyUIControl_chooseGenres_fromlistData();
        QWidget* get_genrePushButton(const int p_index, const tidal::GenreInfo& p_genreInfo);

    };
};

#endif // TIDALGENRESCHOOSE_H
