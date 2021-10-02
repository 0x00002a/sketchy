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

#include <iterator>
#include <qapplication.h>
#include <qevent.h>
#include <qnamespace.h>

#include <qpixmap.h>
#include <spdlog/spdlog.h>

namespace {
void fill_with_transparent(QPixmap& m)
{
    QColor c{Qt::white};
    c.setAlpha(0);
    m.fill(c);
}
auto rasterise(const sketchy::detail::stroke& s) -> QPixmap
{
    assert(s.bounds.isValid());
    QPixmap stroke_raster{s.bounds.size().toSize()};
    fill_with_transparent(stroke_raster);
    QPainter p{&stroke_raster};
    s.paint(p);
    p.end();
    return stroke_raster;
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

    std::size_t i{0};
    std::for_each(strokes_.begin(), strokes_.end(),
                  [&, this](const stroke& s) mutable {
                      logger_->trace("draw at: [{}]", s.bounds);

                      p.drawPixmap(s.bounds.topLeft() - move_offset_,
                                   raster_strokes_.at(i));
                      ++i;
                  });
    if (pen_down_) {
        p.translate(-move_offset_);
        active_stroke_.paint(p);
    }
}

void canvas::set_strokes(const std::vector<stroke>& s)
{
    strokes_ = s;
    raster_strokes_.clear();
    raster_strokes_.reserve(s.size());
    std::transform(strokes_.begin(), strokes_.end(),
                   std::back_inserter(raster_strokes_), rasterise);
    update();
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
            if (pen_down_) {
                curr_weight_ = pt.pressure();
                logger_->debug("recorded pressure: {}", curr_weight_);
            }
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
    active_stroke_.offset = move_offset_;
}
template<typename T>
constexpr auto diff(T lhs, T rhs) -> T
{
    return lhs > rhs ? lhs - rhs : rhs - lhs;
}

void canvas::finish_stroke(const QPointF& at)
{
    active_stroke_.update_bounds(at);
    raster_strokes_.emplace_back(rasterise(active_stroke_));
    strokes_.emplace_back(std::move(active_stroke_));
    active_stroke_ = {};
}
void canvas::add_stroke(const QPointF& at)
{

    const auto dpr = devicePixelRatioF();
    active_stroke_.update_bounds(at);
    active_stroke_.append({
        .start = last_pt,
        .end = at,
        .weight = curr_weight_,
        .colour = Qt::black,
    });

    update(active_stroke_.bounds.toRect().normalized());
}

} // namespace sketchy::ui
