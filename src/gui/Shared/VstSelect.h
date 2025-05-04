//
// Created by ns on 2/27/25.
//

#ifndef VSTSELECT_H
#define VSTSELECT_H

#include "../../Logging.h"

#include <QFileDialog>
#include <QUrl>

namespace Gj {
namespace Gui {

class VstSelect final : public QFileDialog {
  QUrl defaultUrl;

public:
  explicit VstSelect(QWidget *parent);
  ~VstSelect() override;
};

} // Gui
} // GJ

#endif //VSTSELECT_H
