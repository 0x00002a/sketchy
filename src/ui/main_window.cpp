// Copyright (C) 2021 Natasha England-Elbro
//
// This file is part of sketchy.
//
// sketchy is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// sketchy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sketchy.  If not, see <http://www.gnu.org/licenses/>.

#include "main_window.hpp"
#include "canvas.hpp"
#include "storage.hpp"

#include <QHBoxLayout>
#include <fstream>
#include <qevent.h>
#include <qfile.h>
#include <qkeysequence.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qscrollarea.h>
#include <qstackedwidget.h>
#include <qtoolbar.h>

#include <spdlog/spdlog.h>

namespace sketchy::ui {
main_window::main_window()
    : center_container_{new QStackedWidget},
      canvas_{new canvas{spdlog::default_logger()}}
{
    auto* w = new QWidget;
    auto* layout = new QHBoxLayout{w};
    layout->addWidget(center_container_);
    setCentralWidget(w);

    center_container_->addWidget(canvas_);
    center_container_->setCurrentWidget(canvas_);

    auto* mbar = menuBar()->addMenu("tools");
    auto* tbar = addToolBar(tr("tools"));

    auto* move_act = new QAction{tr("move"), this};
    auto* draw_act = new QAction{tr("draw"), this};

    move_act->setShortcut(QKeySequence::fromString("S"));
    draw_act->setShortcut(QKeySequence::fromString("D"));
    connect(move_act, &QAction::triggered, this,
            &main_window::switch_to_move_mode);
    connect(draw_act, &QAction::triggered, this,
            &main_window::switch_to_draw_mode);
    tbar->addAction(move_act);
    tbar->addAction(draw_act);
    mbar->addAction(move_act);
    mbar->addAction(draw_act);

    auto* save_act = new QAction{tr("save"), this};
    save_act->setShortcut(QKeySequence::Save);
    connect(save_act, &QAction::triggered, this, &main_window::on_save);

    auto* load_act = new QAction{tr("open"), this};
    load_act->setShortcut(QKeySequence::Open);
    connect(load_act, &QAction::triggered, [this] { on_load_from(""); });

    auto* mfile = menuBar()->addMenu("&File");
    mfile->addAction(save_act);
    mfile->addAction(load_act);
}
void main_window::on_save_as(const QString&) {}
void main_window::on_save()
{
    const auto json = to_json(canvas_->strokes());
    spdlog::info("json: {}", json);
    std::ofstream f{"./output.json"};
    f.write(json.c_str(), json.size());
}
void main_window::on_load_from(const QString&)
{
    QFile f{"./output.json"};
    f.open(QFile::ReadOnly);
    canvas_->set_strokes(from_json(f.readAll().toStdString()));
}

void main_window::switch_to_draw_mode()
{
    spdlog::debug("switch mode: draw");
    canvas_->curr_mode(canvas::mode::draw);
}
void main_window::switch_to_move_mode()
{
    spdlog::debug("switch mode: move");
    canvas_->curr_mode(canvas::mode::move);
}

} // namespace sketchy::ui
