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

#pragma once

#include <qmainwindow.h>

class QStackedWidget;

namespace sketchy::ui {
class canvas;

class main_window : public QMainWindow {
    Q_OBJECT
public:
    main_window();

private slots:
    void switch_to_draw_mode();
    void switch_to_move_mode();

private:
    QStackedWidget* center_container_;
    canvas* canvas_;
};

} // namespace sketchy::ui
