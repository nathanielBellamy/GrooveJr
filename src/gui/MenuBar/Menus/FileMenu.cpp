//
// Created by ns on 2/23/25.
//

#include "FileMenu.h"

#include <iostream>

namespace Gj {
namespace Gui {

FileMenu::~FileMenu() {
  delete renderAction;
  delete this;
}

FileMenu::FileMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&File", parent)
    , actorSystem(actorSystem)
    , renderAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Render"), this))
    {

  renderAction->setStatusTip(tr("Render Audio"));
  connect(renderAction, &QAction::triggered, [&]() {
    // TODO
    std::cout << " kOffline Render Audio " << std::endl;
  });
  addAction(renderAction);

}

} // Gui
} // Gj