//
// Created by ns on 10/11/25.
//

#include "AudioFileQueryModel.h"

namespace Gj {
namespace Gui {

Result AudioFileQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()

  Logging::write(
      Info,
      "Gui::AudioFileQueryModel::hydrateState",
      "Setting AudioFile Query: ===== newQuery ====="
  );
  return OK;
}

QVariant AudioFileQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result AudioFileQueryModel::refresh() {
  std::string queryStr;
  switch (filters->filterBy) {
    case TRACK:
      queryStr = " select filePath from audioFiles"
                 " where audioFiles.trackId in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select filePath from audioFiles";
  }

  query = QString(queryStr.c_str());
  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Path"));

  return OK;
}


} // Gui
} // Gj