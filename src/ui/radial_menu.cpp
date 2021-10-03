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
#include <qpropertyanimation.h>

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

radial_menu::radial_menu() : viewport_{std::make_unique<QGraphicsView>()}
{
    viewport_->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    viewport_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport_->setScene(&scene_);
    viewport_->setAttribute(Qt::WA_TranslucentBackground);
    viewport_->setStyleSheet(" QGraphicsView { background: transparent; } ");
}

auto radial_menu::sizeHint() const -> QSize
{
    const QSize base{40, 40};
    if (parts_.empty()) {
        return base;
    }
    else {
        const int dim = parts_.at(0)->diameter();
        return base + QSize{dim, dim};
    }
}
void radial_menu::add_action(QAction* act)
{
    auto* seg = new radial_menu_segment{act};
    scene_.addItem(seg);
    parts_.emplace_back(seg);
}

void radial_menu::show_menu(const QPointF& at)
{
    const auto dims = min_rect(bounds());
    const auto center = bounds().center();
    const auto sdims = scene_.sceneRect();
    viewport_->setGeometry(QRect{QPointF{at.x() - sdims.size().width() / 2,
                                         at.y() - sdims.size().height() / 2}
                                     .toPoint(),
                                 QSize{sdims.size().toSize()}});
    viewport_->show();
    viewport_->activateWindow();
    for (auto i = 0; i != parts_.size(); ++i) {
        auto* ani = new QPropertyAnimation{parts_.at(i), "pos", this};
        const auto start =
            viewport_->mapToScene(viewport_->mapFromGlobal(at).toPoint());
        ani->setStartValue(start);
        ani->setEndValue(calc_segment_pos(i));
        ani->setDuration(200);
        ani->start();
    }
}
void radial_menu::hide_menu() {}

auto radial_menu::calc_segment_pos(int num) const -> QPointF
{
    const auto count = parts_.size();
    return QPointF{radius_ * cos(num * 2 * M_PI / count - M_PI / 2),
                   radius_ * sin(num * 2 * M_PI / count - M_PI / 2)};
}

void radial_menu::paintEvent(QPaintEvent* ev) {}
} // namespace sketchy::ui
