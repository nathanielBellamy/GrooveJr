//
// Created by ns on 3/16/25.
//

#ifndef EFFECTSCONTAINER_H
#define EFFECTSCONTAINER_H

#include <QLabel>
#include <QWidget>
#include <QGridLayout>

namespace Gj {
namespace Gui {

class EffectsContainer : public QWidget {

  public:
     EffectsContainer(QWidget* parent);

  private:
    QGridLayout grid;
    QLabel title;
    void setupGrid();
    void setStyle();
};

} // Gui
} // Gj


#endif //EFFECTSCONTAINER_H
