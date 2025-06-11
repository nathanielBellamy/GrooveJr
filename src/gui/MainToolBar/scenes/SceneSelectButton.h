//
// Created by ns on 6/1/25.
//

#ifndef SCENESELECTBUTTON_H
#define SCENESELECTBUTTON_H

#include <QPushButton>

#include "../../../AppState.h"
#include "../../Color.h"

namespace Gj {
namespace Gui {

class SceneSelectButton final : public QPushButton {
  public:
    SceneSelectButton(QWidget* parent, QAction* action, int sceneIndex);
    void hydrateState(const AppStatePacket& appState);

  private:
    int sceneIndex;
    QAction* action;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;
};

} // Gui
} // Gj

#endif //SCENESELECTBUTTON_H
