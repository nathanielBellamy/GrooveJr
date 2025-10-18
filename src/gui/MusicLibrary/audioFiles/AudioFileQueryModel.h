//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILEQUERYMODEL_H
#define AUDIOFILEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class AudioFileQueryModel final : public MusicLibraryQueryModel {
  bool isSelected(const QModelIndex& item) const;

public:
  explicit AudioFileQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, filters, AUDIO_FILE)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //AUDIOFILEQUERYMODEL_H
