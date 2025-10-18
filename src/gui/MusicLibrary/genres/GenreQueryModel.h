//
// Created by ns on 10/11/25.
//

#ifndef GENREQUERYMODEL_H
#define GENREQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../../AppState.h"
#include "../../../enums/Result.h"
#include "../../../Logging.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"


namespace Gj {
namespace Gui {

constexpr size_t GENRE_COL_NAME = 0;
constexpr size_t GENRE_COL_ID = 1;

class GenreQueryModel final : public MusicLibraryQueryModel {

public:
  explicit GenreQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, filters, GENRE)
    {
    refresh();
  }

  Result refresh() override;
  Result hydrateState(const AppStatePacket &appStatePacket) override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //GENREQUERYMODEL_H
