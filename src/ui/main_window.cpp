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
#include "ui/radial_menu.hpp"

#include <QHBoxLayout>
#include <fstream>
#include <qapplication.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qkeysequence.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qscreen.h>
#include <qscrollarea.h>
#include <qstackedwidget.h>
#include <qsvggenerator.h>
#include <qtoolbar.h>

#include <spdlog/spdlog.h>

namespace sketchy::ui {
main_window::main_window(logger_t logger)
    : logger_{std::move(logger)},
      center_container_{new QStackedWidget},
      canvas_{new canvas{logger_->clone("canvas")}}
{
    auto* w = new QWidget;
    auto* layout = new QHBoxLayout{w};
    layout->addWidget(center_container_);
    setCentralWidget(w);
    const auto aval = QApplication::primaryScreen()->availableSize();
    setMinimumSize(aval.width() / 3 * 2, aval.height() / 3 * 2);

    center_container_->addWidget(canvas_);
    center_container_->setCurrentWidget(canvas_);
    connect(canvas_, &canvas::content_menu_wanted, this,
            &main_window::on_radial_menu_wanted);

    auto* tbar = addToolBar(tr("tools"));

    auto* move_act = new QAction{tr("Move"), this};
    auto* draw_act = new QAction{tr("Draw"), this};
    auto* erase_act = new QAction{tr("Erase"), this};

    move_act->setShortcut(QKeySequence::fromString("S"));
    draw_act->setShortcut(QKeySequence::fromString("D"));
    erase_act->setShortcut(QKeySequence::fromString("E"));
    connect(move_act, &QAction::triggered, this,
            &main_window::switch_to_move_mode);
    connect(draw_act, &QAction::triggered, this,
            &main_window::switch_to_draw_mode);
    connect(erase_act, &QAction::triggered, this,
            &main_window::switch_to_erase_mode);
    tbar->addAction(move_act);
    tbar->addAction(draw_act);
    tbar->addAction(erase_act);
    tools_acts_.emplace_back(move_act);
    tools_acts_.emplace_back(draw_act);
    tools_acts_.emplace_back(erase_act);

    auto* save_act = new QAction{tr("Save"), this};
    save_act->setShortcut(QKeySequence::Save);
    connect(save_act, &QAction::triggered, this, &main_window::on_save);

    auto* save_as_act = new QAction{tr("Save As..."), this};
    save_as_act->setShortcut(QKeySequence::SaveAs);
    connect(save_as_act, &QAction::triggered, this,
            &main_window::on_save_as_clicked);

    auto* load_act = new QAction{tr("Open..."), this};
    load_act->setShortcut(QKeySequence::Open);
    connect(load_act, &QAction::triggered, this,
            &main_window::on_load_from_clicked);

    auto* export_act = new QAction{tr("Export"), this};
    export_act->setShortcut(QKeySequence::fromString("Ctrl-e"));
    connect(export_act, &QAction::triggered, this,
            &main_window::on_export_all_svg);

    auto* mfile = menuBar()->addMenu("&File");
    mfile->addAction(save_act);
    mfile->addAction(save_as_act);
    mfile->addSeparator();
    mfile->addAction(load_act);
    mfile->addSeparator();
    mfile->addAction(export_act);
}

void on_radial_menu_wanted(const QPointF&) {}
void main_window::export_all_svg_to(const QString& path) const
{
    QSvgGenerator gen;
    gen.setFileName(path);
    gen.setSize(canvas_->scene_size().toSize());
    QFile out{path};
    out.open(QFile::WriteOnly);
    gen.setOutputDevice(&out);

    QPainter p{&gen};
    canvas_->print_area(p, QRectF{QPointF{0, 0}, canvas_->scene_size()});
    p.end();
}

void main_window::on_export_all_svg()
{
    auto* dialog = new QFileDialog{this};
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setFileMode(QFileDialog::FileMode::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this,
            &main_window::export_all_svg_to);
    dialog->open();
}
void main_window::on_save_as(const QString& p)
{
    save_path_ = p;
    const auto json = to_json(canvas_->strokes());
    spdlog::debug("saving document as: {}", json);
    QFile f{p};
    f.open(QFile::WriteOnly);
    f.write(json.c_str(), json.size());
}
void main_window::on_save_as_clicked()
{
    auto* dialog = new QFileDialog{this};
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setFileMode(QFileDialog::FileMode::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, &main_window::on_save_as);
    dialog->open();
}
void main_window::on_save()
{
    if (save_path_.isEmpty()) {
        on_save_as_clicked();
    }
    else {
        on_save_as(save_path_);
    }
}
void main_window::on_load_from(const QString& p)
{
    QFile f{p};
    f.open(QFile::ReadOnly);
    canvas_->set_strokes(from_json(f.readAll().toStdString()));
    save_path_ = p;
}
void main_window::on_load_from_clicked()
{
    auto* dialog = new QFileDialog{this};
    dialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    connect(dialog, &QFileDialog::fileSelected, this,
            &main_window::on_load_from);
    dialog->open();
}

void main_window::switch_to_draw_mode()
{
    logger_->debug("switch mode: draw");
    canvas_->curr_mode(canvas::mode::draw);
}
void main_window::switch_to_move_mode()
{
    logger_->debug("switch mode: move");
    canvas_->curr_mode(canvas::mode::move);
}
void main_window::switch_to_erase_mode()
{
    logger_->debug("switch mode: erase");
    canvas_->curr_mode(canvas::mode::erase);
}
void main_window::on_radial_menu_wanted(const QPointF& at)
{
    logger_->debug("radial menu requested");
    if (!tools_menu_) {
        tools_menu_ = new radial_menu;
        std::for_each(tools_acts_.begin(), tools_acts_.end(),
                      [this](auto* act) { tools_menu_->add_action(act); });
        tools_menu_->setMinimumSize(tools_menu_->sizeHint());
    }
    tools_menu_->popup(at.toPoint());
}

auto main_window::make_action(const QString& txt,
                              const std::function<void()>& act) -> QAction*
{
    auto* obj = new QAction{txt, this};
    connect(obj, &QAction::triggered, this, act);
    return obj;
}
} // namespace sketchy::ui
