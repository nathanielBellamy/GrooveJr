//
// Created by ns on 10/11/25.
//

#ifndef TRACKTABLEVIEW_H
#define TRACKTABLEVIEW_H

#include <QTableView>

#include "TrackQueryModel.h"

namespace Gj {
namespace Gui {

class TrackTableView final : public QTableView {
  TrackQueryModel* trackQueryModel;

  public:
    TrackTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , trackQueryModel(new TrackQueryModel(this)) {
      setModel(trackQueryModel);
    };
};

} // Gui
} // Gj

#endif //TRACKTABLEVIEW_H
