//
// Created by ns on 3/2/25.
//

#ifndef CHANNEL_H
#define CHANNEL_H

#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class Channel final : public QWidget {

  public:
    Channel(QWidget* parent, int index);

  private:
    QLabel title;
    int index;
    void setStyle();
};

} // Gui
} // Gj



#endif //CHANNEL_H
