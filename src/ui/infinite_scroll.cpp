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

#include "infinite_scroll.hpp"

#include <limits>
#include <qboxlayout.h>
#include <qlayout.h>
#include <qscrollbar.h>
#include <type_traits>

namespace {
void init_scrollbar(QScrollBar* bar)
{
    bar->setPageStep(10);
    bar->setRange(0, std::numeric_limits<int>::max());
}
} // namespace
namespace sketchy::ui {

infinite_scroller::infinite_scroller()
{
    new QHBoxLayout{viewport()};

    auto* vbar = verticalScrollBar();
    auto* hbar = horizontalScrollBar();

    init_scrollbar(vbar);
    init_scrollbar(hbar);
}

void infinite_scroller::scrollContentsBy(int dx, int dy)
{
    if (!inner_) {
        return;
    }
    inner_->move(dx, dy);
}

void infinite_scroller::widget(QWidget* w)
{
    auto* layout = viewport()->layout();
    assert(layout);
    if (inner_) {
        layout->removeWidget(inner_);
        delete inner_;
    }
    inner_ = w;
    layout->addWidget(inner_);
}

} // namespace sketchy::ui
