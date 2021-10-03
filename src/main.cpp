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

#include "ui/main_window.hpp"

#include <qapplication.h>

#include <spdlog/spdlog.h>

using namespace sketchy;

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::debug);

    QApplication app{argc, argv};

    ui::main_window win{spdlog::default_logger()->clone("window")};
    win.show();

    return app.exec();
}
