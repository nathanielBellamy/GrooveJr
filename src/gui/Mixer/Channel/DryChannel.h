//
// Created by ns on 3/2/25.
//

#ifndef DRYCHANNEL_H
#define DRYCHANNEL_H

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>

namespace Gj {
namespace Gui {

class DryChannel final : public QWidget {

  public:
    DryChannel(QWidget* parent);

  private:
    QGridLayout grid;
    QLabel title;
    QSlider slider;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //DRYCHANNEL_H
