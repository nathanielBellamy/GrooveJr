//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "AudioFileQueryModel.h"
#include "../../../AppState.h"

namespace Gj {
namespace Gui {

class AudioFileTableView final : public QTableView {
  AudioFileQueryModel* audioFileQueryModel;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    AudioFileTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , audioFileQueryModel(new AudioFileQueryModel(this)) {
      setModel(audioFileQueryModel);
      setStyle();
    };

    ~AudioFileTableView() {
      delete audioFileQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      audioFileQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
