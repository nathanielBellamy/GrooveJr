//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILEQUERYMODEL_H
#define AUDIOFILEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class AudioFileQueryModel final : public QSqlQueryModel {
  QString query = QString("select filePath from audioFiles");

public:
  explicit AudioFileQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Path"));
  }
  Result hydrateState(const AppStatePacket& appStatePacket);

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //AUDIOFILEQUERYMODEL_H
