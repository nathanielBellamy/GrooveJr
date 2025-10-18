//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"

namespace Gj {
namespace Gui {

class AudioFileTableView final : public MusicLibraryTableView {

  public:
    AudioFileTableView(QWidget* parent, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, filters, AUDIO_FILE)
        {};
};

} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
