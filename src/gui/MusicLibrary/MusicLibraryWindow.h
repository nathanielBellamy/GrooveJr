//
// Created by ns on 3/2/25.
//

#ifndef MUSICLIBRARYWINDOW_H
#define MUSICLIBRARYWINDOW_H

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class MusicLibraryWindow final : public QWidget {
  public:
    explicit MusicLibraryWindow(QWidget *parent);

  private:
    QGridLayout grid;
    QLabel title;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //MUSICLIBRARYWINDOW_H
