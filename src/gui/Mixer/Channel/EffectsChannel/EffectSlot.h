//
// Created by ns on 3/8/25.
//

#ifndef EFFECTSLOT_H
#define EFFECTSLOT_H

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class EffectSlot final : public QWidget {

  public:
    EffectSlot(QWidget* parent);

  private:
    QLabel title;
    void setStyle();

};

} // Gui
} // Gj



#endif //EFFECTSLOT_H
