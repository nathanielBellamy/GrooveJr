//
// Created by ns on 11/16/25.
//

#ifndef SCENESTABLEVIEW_H
#define SCENESTABLEVIEW_H

#include <QHeaderView>
#include <QTimer>

#include "../../QSql/SqlTableView.h"
#include "../../Color.h"
#include "ScenesQueryModel.h"

namespace Gj {
namespace Gui {
class ScenesTableView final : public SqlTableView {
  QAction* sceneLoadAction;

  Result setStyle() {
    setMaximumHeight(80);

    setShowGrid(false);
    setAlternatingRowColors(true);

    setSelectionBehavior(SelectRows);
    setSelectionMode(SingleSelection);

    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(28);

    setFocusPolicy(Qt::StrongFocus);

    const std::string dark500 = Color::toHex(GjC::DARK_500);
    const std::string dark400 = Color::toHex(GjC::DARK_400);
    const std::string dark300 = Color::toHex(GjC::DARK_300);
    const std::string light100 = Color::toHex(GjC::LIGHT_100);
    const std::string light400 = Color::toHex(GjC::LIGHT_400);
    const std::string light300 = Color::toHex(GjC::LIGHT_300);

    setStyleSheet(QString::fromStdString(
      "QTableView {"
      "  background-color: " + dark500 + ";"
      "  alternate-background-color: " + dark400 + ";"
      "  color: " + light100 + ";"
      "  border: none;"
      "  font-size: 12px;"
      "  font-weight: 400;"
      "  selection-background-color: " + light400 + ";"
      "  selection-color: #FFFFFF;"
      "  outline: 0;"
      "}"

      "QTableView::item {"
      "  padding: 4px 8px;"
      "  border: none;"
      "}"
      "QTableView::item:hover {"
      "  background-color: " + dark300 + ";"
      "}"
      "QTableView::item:selected {"
      "  background-color: " + light400 + ";"
      "  color: #FFFFFF;"
      "}"

      "QHeaderView::section {"
      "  background-color: " + dark400 + ";"
      "  color: " + light300 + ";"
      "  border: none;"
      "  border-bottom: 1px solid " + dark300 + ";"
      "  padding: 6px 8px;"
      "  font-size: 11px;"
      "  font-weight: 600;"
      "  text-transform: uppercase;"
      "}"

      "QScrollBar:vertical {"
      "  background: " + dark500 + ";"
      "  width: 8px;"
      "  margin: 0;"
      "}"
      "QScrollBar::handle:vertical {"
      "  background: " + dark300 + ";"
      "  min-height: 24px;"
      "  border-radius: 4px;"
      "}"
      "QScrollBar::handle:vertical:hover {"
      "  background: " + light400 + ";"
      "}"
      "QScrollBar::add-line:vertical,"
      "QScrollBar::sub-line:vertical {"
      "  height: 0px;"
      "}"

      "QScrollBar:horizontal {"
      "  background: " + dark500 + ";"
      "  height: 8px;"
      "  margin: 0;"
      "}"
      "QScrollBar::handle:horizontal {"
      "  background: " + dark300 + ";"
      "  min-width: 24px;"
      "  border-radius: 4px;"
      "}"
      "QScrollBar::handle:horizontal:hover {"
      "  background: " + light400 + ";"
      "}"
      "QScrollBar::add-line:horizontal,"
      "QScrollBar::sub-line:horizontal {"
      "  width: 0px;"
      "}"

      "QTableCornerButton::section {"
      "  background-color: " + dark400 + ";"
      "  border: none;"
      "}"
    ));

    return OK;
  }

  void mouseDoubleClickEvent(QMouseEvent* event) override {
    if (const QModelIndex clickedIndex = indexAt(event->pos()); clickedIndex.isValid()) {
      const SqlQueryModel* model = getModel();
      const ID sceneDbId = model->index(clickedIndex.row(), SCENES_COL_ID).data().toULongLong();
      sceneLoadAction->setData(sceneDbId);
      sceneLoadAction->trigger();
    }
  }

public:
  ScenesTableView(
    QWidget* parent,
    actor_system& actorSystem,
    Db::Dao* dao,
    Gj::State::Core* stateCore,
    SqlWorkerPool* workerPool,
    QAction* sceneLoadAction
  )
  : SqlTableView(
      parent,
      actorSystem,
      dao,
      stateCore,
      new ScenesQueryModel(parent, stateCore, QString("ScenesQueryModel"), workerPool)
    )
    , sceneLoadAction(sceneLoadAction) {
    workerPool->connectClient(model);
    setStyle();
    connect(model, &QAbstractItemModel::modelReset, this, [this]() {
      QTimer::singleShot(0, this, [this]() {
        setColumnHidden(SCENES_COL_ID, true);
      });
    });
    refresh(true);
  };
};
} // Gui
} // Gj

#endif //SCENESTABLEVIEW_H

