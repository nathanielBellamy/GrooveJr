//
// Created by ns on 10/16/25.
//

#ifndef MUSICLIBRARYQUERYMODEL_H
#define MUSICLIBRARYQUERYMODEL_H

#include <vector>

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../AppState.h"
#include "../../Logging.h"
#include "../../enums/Result.h"

#include "MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class MusicLibraryQueryModel : public QSqlQueryModel {
  protected:
    MusicLibraryType type;
    MusicLibraryFilters* filters;
    QString queryString;
    Result setQueryString(const std::string& newQueryString) {
      queryString = QString(newQueryString.c_str());
      setQuery(queryString);
      return OK;
    };

    bool isSelected(const QModelIndex& item, const size_t idCol) const {
      std::vector<Db::ID> ids = filters->filters.at(type).ids;
      return std::find(
          ids.begin(),
          ids.end(),
          index(item.row(), idCol).data()
          ) != ids.end();
    };

  public:
    MusicLibraryQueryModel(QObject* parent, MusicLibraryFilters* filters, const MusicLibraryType type)
      : QSqlQueryModel(parent)
      , type(type)
      , filters(filters)
      {}

    virtual Result hydrateState(const AppStatePacket& appStatePacket) = 0;
    virtual Result refresh() = 0;
};

} // Gui
} // Gj

#endif //MUSICLIBRARYQUERYMODEL_H
