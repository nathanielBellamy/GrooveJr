//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AudioFileQueryModel.h"

namespace Gj {
namespace Gui {

class AudioFileTableView final : public MusicLibraryTableView {

  public:
    AudioFileTableView(QWidget* parent, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, dao, AUDIO_FILE, AUDIO_FILE_COL_ID, filters, new AudioFileQueryModel(parent, filters))
        {};
};

} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
