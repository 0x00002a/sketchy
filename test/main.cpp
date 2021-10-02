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

#include <doctest/doctest.h>

#include <qapplication.h>
#include <vector>

#include "storage.hpp"

using namespace sketchy;

TEST_CASE("serialization and deserialization works")
{
    detail::stroke::line l{.start = {0.2, 0.5},
                           .end = {0.6, 0.8},
                           .weight = 0.3,
                           .colour = QColor{"#1b1b1b"}};
    std::vector<detail::stroke> strokes{{}};
    strokes.at(0).append(l);

    const auto rs = to_json(strokes);
    const auto actual = from_json(to_json(strokes));

    REQUIRE(actual == strokes);
}

int main(int argc, char** argv)
{
    QApplication app{argc, argv};
    doctest::Context ctx;
    ctx.applyCommandLine(argc, argv);
    return ctx.run();
}