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

namespace {
void fill_with_transparent(QPixmap& m)
{
    QColor c{Qt::white};
    c.setAlpha(0);
    m.fill(c);
}
} // namespace
namespace sketchy::ui {

canvas::canvas(logger_t logger)
    : logger_{std::move(logger)}, curr_pen_{Qt::black}
{
}

void canvas::paintEvent(QPaintEvent* e)
{
    const auto aval = e->region();
    QPainter p{this};
    p.fillRect(aval.boundingRect(), Qt::white);

    std::for_each(strokes_.begin(), strokes_.end(), [&](const stroke& s) {
        if (aval.contains(s.bounds.toRect())) {
            logger_->trace("draw at: [{}, {}]", s.bounds.topLeft().x(),
                           s.bounds.topLeft().y());
            p.drawPixmap(s.bounds.topLeft(), s.img);
        }
    });
    if (!active_stroke_.img.isNull()) {
        p.drawPixmap(0, 0, active_stroke_.img);
    }
}

bool canvas::event(QEvent* e)
{
    if (e->isPointerEvent()) {
        auto* pe = static_cast<QPointerEvent*>(e);
        for (const auto& pt : pe->points()) {
            switch (pt.state()) {
            case QEventPoint::State::Pressed:
                prime_stroke(pt.position());
                last_pt = pt.position();
                pen_down_ = true;
                break;
            case QEventPoint::State::Released:
                if (pen_down_) {
                    finish_stroke(pt.position());
                }
                pen_down_ = false;
            case QEventPoint::State::Updated:
                if (pen_down_) {
                    add_stroke(pt.position());
                    last_pt = pt.position();
                }
                break;
            default:
                break;
            }
        }
        return true;
    }
    else {
        return QWidget::event(e);
    }
}
void canvas::prime_stroke(const QPointF& at)
{

    const auto dpr = devicePixelRatioF();
    active_stroke_.img =
        QPixmap{qRound(rect().width() * dpr), qRound(rect().height() * dpr)};

    fill_with_transparent(active_stroke_.img);
}
template<typename T>
constexpr auto diff(T lhs, T rhs) -> T
{
    return lhs > rhs ? lhs - rhs : rhs - lhs;
}

void canvas::stroke::update_bounds(const QPointF& at)
{
    auto& b = bounds;

    b.setTopLeft(QPointF{std::min(at.x(), b.topLeft().x()),
                         std::min(at.y(), b.topLeft().y())});
    b.setBottomRight(QPointF{std::max(at.x(), b.bottomRight().x()),
                             std::max(at.y(), b.bottomRight().y())});
}
void canvas::finish_stroke(const QPointF& at)
{
    active_stroke_.update_bounds(at);

    strokes_.emplace_back(std::move(active_stroke_));
    active_stroke_ = {};
}
void canvas::add_stroke(const QPointF& at)
{
    auto max_pen_radius = 20;

    const auto dpr = devicePixelRatioF();
    assert(!active_stroke_.img.isNull());
    QPainter with{&active_stroke_.img};
    with.setPen(curr_pen_);
    with.drawLine(last_pt, at);
    with.end();

    active_stroke_.update_bounds(at);

    /*update(QRect{last_pt.toPoint(), at.toPoint()}.normalized().adjusted(
        -max_pen_radius, -max_pen_radius, max_pen_radius, max_pen_radius));*/
    update();
}

} // namespace sketchy::ui
