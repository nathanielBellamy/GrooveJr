//
// Created by ns on 10/10/25.
//

#ifndef DIRECTORYSELECT_H
#define DIRECTORYSELECT_H

#include "../../Logging.h"

#include <QFileDialog>
#include <QUrl>

namespace Gj {
namespace Gui {

class DirectorySelect final : public QFileDialog {
  QUrl defaultUrl;

public:
  explicit DirectorySelect(QWidget *parent);
  ~DirectorySelect() override;
};

} // Gui
} // GJ




#endif //DIRECTORYSELECT_H
