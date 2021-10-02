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
#include "qt_fmt.hpp"

#include <QMouseEvent>

#include <qapplication.h>
#include <qevent.h>
#include <qnamespace.h>

#include <spdlog/spdlog.h>

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
    const auto bounds = e->region().boundingRect();
    const auto aval = QRectF{bounds.topLeft() + move_offset_,
                             bounds.bottomRight() + move_offset_};
    QPainter p{this};
    p.fillRect(bounds, Qt::white);

    std::for_each(strokes_.begin(), strokes_.end(), [&, this](const stroke& s) {
        if (aval.contains(s.bounds.topLeft()) ||
            aval.contains(s.bounds.bottomRight())) {
            logger_->trace("draw at: [{}]", s.bounds.topLeft());
            p.drawPixmap(s.bounds.topLeft() - (move_offset_ - s.offset), s.img);
        }
        else {
            logger_->trace("[{}, {}, {}, {}] out of bounds of [{}, {}, {}, {}]",
                           s.bounds.topLeft().x(), s.bounds.topLeft().y(),
                           s.bounds.bottomRight().x(),
                           s.bounds.bottomRight().x(), aval.topLeft().x(),
                           aval.topLeft().y(), aval.bottomRight().x(),
                           aval.bottomRight().y());
        }
    });
    if (!active_stroke_.img.isNull()) {
        p.drawPixmap(0, 0, active_stroke_.img);
    }
}

void canvas::curr_mode(mode m) { curr_mode_ = m; }
void canvas::handle_pen_down(const QPointF& at)
{
    pen_down_ = true;
    switch (curr_mode_) {
    case mode::draw:
        QApplication::setOverrideCursor(QCursor{Qt::CursorShape::CrossCursor});
        prime_stroke(at);
        break;
    case mode::move:
        QApplication::setOverrideCursor(
            QCursor{Qt::CursorShape::DragMoveCursor});
        break;
    }
    last_pt = at;
}
void canvas::handle_pen_up(const QPointF& at)
{
    switch (curr_mode_) {
    case mode::draw:
        if (pen_down_) {
            finish_stroke(at);
        }
        break;
    }
    QApplication::setOverrideCursor(QCursor{});
    pen_down_ = false;
}
void canvas::handle_pen_move(const QPointF& at)
{
    if (pen_down_) {
        switch (curr_mode_) {
        case mode::draw:
            add_stroke(at);
            break;
        case mode::move:
            const auto diff = last_pt - at;
            move_offset_ += diff;
            if (move_offset_.x() < 0) {
                move_offset_.setX(0);
            }
            if (move_offset_.y() < 0) {
                move_offset_.setY(0);
            }
            logger_->debug("move: [{}]", diff);
            update();
            break;
        }

        last_pt = at;
    }
}

bool canvas::event(QEvent* e)
{
    if (e->isPointerEvent()) {
        auto* pe = static_cast<QPointerEvent*>(e);
        for (const auto& pt : pe->points()) {
            const auto pos = pt.position() + move_offset_;
            switch (pt.state()) {
            case QEventPoint::State::Pressed:
                handle_pen_down(pos);
                break;
            case QEventPoint::State::Released:
                handle_pen_up(pos);
                break;
            case QEventPoint::State::Updated:
                handle_pen_move(pos);
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
    active_stroke_.offset = move_offset_;
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
    with.drawLine(last_pt - move_offset_, at - move_offset_);
    with.end();

    active_stroke_.update_bounds(at);

    /*update(QRect{last_pt.toPoint(), at.toPoint()}.normalized().adjusted(
        -max_pen_radius, -max_pen_radius, max_pen_radius, max_pen_radius));*/
    update();
}

} // namespace sketchy::ui
