//
// Created by ns on 10/11/25.
//

#ifndef ALBUMQUERYMODEL_H
#define ALBUMQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"

namespace Gj {
namespace Gui {

constexpr size_t ALBUM_COL_TITLE = 0;
constexpr size_t ALBUM_COL_YEAR = 1;
constexpr size_t ALBUM_COL_ID = 2;

class AlbumQueryModel final : public MusicLibraryQueryModel {

public:
  Result refresh();
  explicit AlbumQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, filters, ALBUM)
    {
    refresh();
  }
  Result hydrateState(const AppStatePacket& appStatePacket);
  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //ALBUMQUERYMODEL_H
