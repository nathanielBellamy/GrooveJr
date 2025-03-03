//
// Created by ns on 3/2/25.
//

#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>

namespace Gj {
namespace Gui {

class Channel final : public QWidget {

  public:
    explicit Channel(QWidget* parent);

  private:
    void setStyle();
};

} // Gui
} // Gj



#endif //CHANNEL_H
