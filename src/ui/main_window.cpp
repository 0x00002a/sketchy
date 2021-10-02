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
#include "infinite_scroll.hpp"

#include <QHBoxLayout>
#include <qscrollarea.h>
#include <qstackedwidget.h>

#include <spdlog/spdlog.h>

namespace sketchy::ui {
main_window::main_window()
    : center_container_{new QStackedWidget}, canvas_{new canvas{spdlog::default_logger()}}
{
    auto* w = new QWidget;
    auto* layout = new QHBoxLayout{w};
    layout->addWidget(center_container_);
    setCentralWidget(w);

    center_container_->addWidget(canvas_);
    center_container_->setCurrentWidget(canvas_);
}
} // namespace sketchy::ui
