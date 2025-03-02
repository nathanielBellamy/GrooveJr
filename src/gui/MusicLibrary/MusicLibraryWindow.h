//
// Created by ns on 3/2/25.
//

#ifndef MUSICLIBRARYWINDOW_H
#define MUSICLIBRARYWINDOW_H

#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class MusicLibraryWindow final : public QWidget {
  public:
    explicit MusicLibraryWindow(QWidget *parent);

  private:
    QLabel title;
    void setStyle();
};

} // Gui
} // Gj



#endif //MUSICLIBRARYWINDOW_H
