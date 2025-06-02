//
// Created by ns on 6/1/25.
//

#ifndef SCENESELECTBUTTON_H
#define SCENESELECTBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {

class SceneSelectButton final : public QPushButton {
  public:
    SceneSelectButton(QWidget* parent, QAction* action, int sceneIndex);

  private:
    int sceneIndex;
    bool selected;
    QAction* action;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj

#endif //SCENESELECTBUTTON_H
