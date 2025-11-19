//
// Created by ns on 6/1/25.
//

#ifndef SCENESAVEBUTTON_H
#define SCENESAVEBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {

class SceneButton final : public QPushButton {
  public:
    SceneButton(QWidget* parent, QAction* action, const QString& title);

  private:
    QAction* action;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;

};

} // Gui
} // Gj

#endif //SCENESAVEBUTTON_H
