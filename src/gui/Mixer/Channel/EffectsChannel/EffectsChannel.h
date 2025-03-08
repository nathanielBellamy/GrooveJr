//
// Created by ns on 3/7/25.
//

#ifndef GUIEFFECTSCHANNEL_H
#define GUIEFFECTSCHANNEL_H

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class EffectsChannel final : public QWidget {

  public:
    EffectsChannel(QWidget* parent, int index);

  private:
    QLabel title;
    int index;
    void setStyle();
};


} // Gui
} // Gj

#endif //EFFECTSCHANNEL_H
