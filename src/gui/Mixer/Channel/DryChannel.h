//
// Created by ns on 3/2/25.
//

#ifndef DRYCHANNEL_H
#define DRYCHANNEL_H

#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class DryChannel final : public QWidget {

  public:
    DryChannel(QWidget* parent);

  private:
    QLabel title;
    void setStyle();
};

} // Gui
} // Gj



#endif //DRYCHANNEL_H
