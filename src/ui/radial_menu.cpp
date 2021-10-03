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

radial_menu::radial_menu() : viewport_{new QGraphicsView{this}}
{
    auto* layout = new QVBoxLayout{this};
    layout->addWidget(viewport_);
    QBrush b;
    b.setColor(QColor{"#00000000"});
    scene_.setBackgroundBrush(b);
    viewport_->setBackgroundBrush(b);
    setAutoFillBackground(false);

    viewport_->setScene(&scene_);
    connect(this, &QMenu::aboutToShow, this, &radial_menu::show_menu);
    connect(this, &QMenu::aboutToHide, this, &radial_menu::hide_menu);
    connect(&scene_, &QGraphicsScene::sceneRectChanged, this,
            [this](const QRectF& r) {
                setMinimumSize(r.size().toSize() + QSize{40, 40});
            });
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
void radial_menu::relayout() const
{
    for (auto i = 0; i != parts_.size(); ++i) {
        parts_.at(i)->setPos(calc_segment_pos(i));
    }
}
void radial_menu::show_menu()
{
    const auto dims = min_rect(rect());
    for (auto i = 0; i != parts_.size(); ++i) {
        auto* ani = new QPropertyAnimation{parts_.at(i), "pos", this};
        ani->setStartValue(dims.center());
        ani->setEndValue(calc_segment_pos(i));
        ani->setDuration(200);
        ani->start();
    }
}
void radial_menu::hide_menu() {}

auto radial_menu::calc_segment_pos(int num) const -> QPointF
{
    const auto dims = min_rect(rect());
    QLineF baseline{dims.center(), dims.center() + QPointF{70, 0}};
    const float per_seg = 360.0 / parts_.size();
    baseline.setAngle(per_seg * num);
    return baseline.p2();
}

void radial_menu::paintEvent(QPaintEvent* ev) {}
} // namespace sketchy::ui
