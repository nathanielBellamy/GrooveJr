//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILEQUERYMODEL_H
#define AUDIOFILEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QThread>

#include "../../QSql/SqlWorker.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {

class AudioFileQueryModel final : public MusicLibraryQueryModel {
  Q_OBJECT

  QThread sqlWorkerThread;
  SqlWorker* sqlWorker;
  Result connectActions();
  QString id;

public:
  explicit AudioFileQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, gAppState, filters, AUDIO_FILE)
    , id(QString("AudioFileQueryModel"))
    {

    sqlWorker = new SqlWorker(this);
    sqlWorker->moveToThread(&sqlWorkerThread);
    sqlWorkerThread.start();
    connectActions();
    emit initSqlWorker(id);
    refresh();
  }
  ~AudioFileQueryModel() {
    sqlWorkerThread.quit();
    sqlWorkerThread.wait();
  }
  QVariant data(const QModelIndex& item, int role) const override;

  public slots:
    Result hydrateState(const AppStatePacket& appStatePacket) override;
    Result refresh() override;

};

} // Gui
} // Gj

#endif //AUDIOFILEQUERYMODEL_H
