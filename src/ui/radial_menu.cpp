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

#include "radial_menu.hpp"
#include <fmt/core.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgraphicsitem.h>
#include <qmath.h>
#include <qpainter.h>
#include <qpainterpath.h>

namespace sketchy::ui {
auto max_rect(const QRectF& r) -> QRectF
{
    const auto dim = std::max(r.height(), r.width());
    return QRectF{r.topLeft(), QSizeF{dim, dim}};
}
auto min_rect(const QRectF& r) -> QRectF
{
    const auto dim = std::min(r.height(), r.width());
    return QRectF{r.topLeft(), QSizeF{dim, dim}};
}
radial_menu_segment::radial_menu_segment(QAction* act) : act_{act}
{
    auto* lbl = new QGraphicsTextItem{act->text(), this};
    const auto margin = 10;
    bounds_ = max_rect(
        lbl->boundingRect().adjusted(-margin, -margin, margin, margin));
    auto* boundry = new QGraphicsEllipseItem{bounds_, this};
    lbl->setPos(bounds_.center() - lbl->boundingRect().center());
}
void radial_menu_segment::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
    if (act_) {
        act_->trigger();
    }
}

radial_menu::radial_menu() : viewport_{new QGraphicsView{this}}
{
    auto* layout = new QVBoxLayout{this};
    layout->addWidget(viewport_);

    viewport_->setScene(&scene_);
}

void radial_menu::add_action(QAction* act)
{
    auto* seg = new radial_menu_segment{act};
    scene_.addItem(seg);
    parts_.emplace_back(seg);
    relayout();
}
void radial_menu::relayout() const
{
    for (auto i = 0; i != parts_.size(); ++i) {
        parts_.at(i)->setPos(calc_segment_pos(i));
    }
}

auto radial_menu::calc_segment_pos(int num) const -> QPointF
{
    const auto dims = min_rect(rect());
    QLineF baseline{dims.center(),
                    dims.center() + QPointF{dims.width() / 2, 0}};
    const float per_seg = 360.0 / parts_.size();
    baseline.setAngle(per_seg * num);
    return baseline.p2();
}
} // namespace sketchy::ui
