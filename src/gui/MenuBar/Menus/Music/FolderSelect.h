//
// Created by ns on 2/26/25.
//

#ifndef FOLDERSELECT_H
#define FOLDERSELECT_H

#include <QFileDialog>
#include <QWidget>

namespace Gj {
namespace Gui {

class FolderSelect final : public QFileDialog {

  public:
    explicit FolderSelect(QWidget* parent);
    ~FolderSelect() override;
};

} // Gui
} // Gj





#endif //FOLDERSELECT_H
