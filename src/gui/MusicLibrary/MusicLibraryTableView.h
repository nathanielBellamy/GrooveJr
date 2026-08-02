//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>

#include "caf/actor_system.hpp"

#include "../../enums/Result.h"
#include "../../state/Core.h"
#include "../../db/Dao.h"

#include "../QSql/SqlTableView.h"
#include "MusicLibraryQueryModel.h"
#include "MusicLibraryFilters.h"
#include "../Color.h"


namespace Gj {
namespace Gui {
using namespace caf;

class MusicLibraryTableView : public SqlTableView {
protected:
  QMenu* menu;
  MusicLibraryFilters* filters;

  void setMusicLibraryStyle() {
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
  }

public:
  MusicLibraryTableView(
    QWidget* parent,
    actor_system& actorSystem,
    Db::Dao* dao,
    Gj::State::Core* stateCore,
    SqlQueryModel* model,
    MusicLibraryFilters* filters
  )
  : SqlTableView(parent, actorSystem, dao, stateCore, model)
    , menu(nullptr)
    , filters(filters) {
    setEditTriggers(NoEditTriggers);
    setMusicLibraryStyle();
  };
};
} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
