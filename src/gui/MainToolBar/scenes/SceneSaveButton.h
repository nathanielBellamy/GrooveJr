//
// Created by ns on 6/1/25.
//

#ifndef SCENESAVEBUTTON_H
#define SCENESAVEBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {

class SceneSaveButton final : public QPushButton {
  public:
    SceneSaveButton(QWidget* parent, QAction* action);

  private:
    QAction* action;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;

};

} // Gui
} // Gj

#endif //SCENESAVEBUTTON_H
