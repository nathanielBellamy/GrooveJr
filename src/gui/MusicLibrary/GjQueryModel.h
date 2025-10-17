//
// Created by ns on 10/16/25.
//

#ifndef GJQUERYMODEL_H
#define GJQUERYMODEL_H

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

class GjQueryModel : public QSqlQueryModel {
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
      return type == filters->type
        && std::find(
          filters->ids.begin(),
          filters->ids.end(),
          index(item.row(), idCol).data()
          ) != filters->ids.end();
    };

  public:
    GjQueryModel(QObject* parent, MusicLibraryFilters* filters, const MusicLibraryType type)
      : QSqlQueryModel(parent)
      , type(type)
      , filters(filters)
      {}
};

} // Gui
} // Gj

#endif //GJQUERYMODEL_H
