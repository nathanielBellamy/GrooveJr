//
// Created by ns on 3/2/25.
//

#ifndef MUSICLIBRARYWINDOW_H
#define MUSICLIBRARYWINDOW_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <QTableView>

#include "../Color.h"
#include "../../enums/Result.h"
#include "../../Logging.h"

#include "tracks/TrackTableView.h"


using namespace caf;

namespace Gj {
namespace Gui {

class MusicLibraryWindow final : public QWidget {
  public:
    explicit MusicLibraryWindow(QWidget *parent, actor_system& actorSystem);
    ~MusicLibraryWindow();

  private:
    actor_system& actorSystem;
    QGridLayout grid;
    QLabel title;
    TrackTableView* trackTableView;
    Result connectToDb();
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //MUSICLIBRARYWINDOW_H
