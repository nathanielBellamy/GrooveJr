//
// Created by ns on 5/18/25.
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <sndfile.hh>
#include <QWidget>

namespace Gj {
namespace Gui {

class ProgressBar : public QWidget {

  public:
    ProgressBar(QWidget* parent, sf_count_t frame);

  private:
    sf_count_t frame;
    void setStyle();

};

} // Gui
} // Gj



#endif //PROGRESSBAR_H
