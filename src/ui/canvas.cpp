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

#include "canvas.hpp"

#include <QMouseEvent>

#include <iostream>

namespace sketchy::ui {

canvas::canvas() : curr_pen_{Qt::black} {}

void canvas::paintEvent(QPaintEvent* e)
{
    const auto aval = e->region();
    QPainter p{this};
    p.fillRect(aval.boundingRect(), Qt::white);
    std::for_each(strokes_.begin(), strokes_.end(), [&](const auto& s) {
        if (aval.contains(s.bounds.toRect())) {
            p.drawPixmap(s.bounds.toRect(), s.img);
        }
    });
}

void canvas::mouseMoveEvent(QMouseEvent* e)
{
    add_stroke(e->posF());
    last_pt = e->posF();
}

void canvas::add_stroke(const QPointF& at)
{
    auto max_pen_radius = 20;
    const QPointF top_left{std::min(at.x(), last_pt.x()),
                           std::min(at.y(), last_pt.y())};
    const QPointF bottom_right{std::max(at.x(), last_pt.x()),
                               std::max(at.y(), last_pt.y())};
    const QRectF draw_size{top_left, bottom_right};
    if (!draw_size.isValid()) {
        std::cout << "invalid pt: [" << at.x() << ", " << at.y() << "] vs ["
                  << last_pt.x() << ", " << last_pt.y() << "]\n";
        return;
    }

    const auto dpr = devicePixelRatioF();
    stroke s{QPixmap{qRound(draw_size.width() * dpr),
                     qRound(draw_size.height() * dpr)},
             draw_size};
    s.img.setDevicePixelRatio(dpr);
    s.img.fill(Qt::white);
    assert(!s.img.isNull());
    QPainter with{&s.img};
    with.setPen(curr_pen_);
    with.drawLine(last_pt, at);
    with.end();

    strokes_.emplace_back(std::move(s));

    /*update(QRect{last_pt.toPoint(), at.toPoint()}.normalized().adjusted(
        -max_pen_radius, -max_pen_radius, max_pen_radius, max_pen_radius));*/
    update();
}

} // namespace sketchy::ui
