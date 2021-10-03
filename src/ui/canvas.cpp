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

#include <fmt/core.h>
#include <iterator>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qnamespace.h>
#include <qscrollbar.h>

#include <qpixmap.h>
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
    : logger_{std::move(logger)},
      curr_pen_{Qt::black},
      viewport_{new canvas_view{&scene_}}
{
    logger_->trace("canvas::canvas()");
    scene_.setBackgroundBrush(QBrush{Qt::white});
    (new QVBoxLayout{this})->addWidget(viewport_);
    connect(&scene_, &canvas_scene::on_pointer_event, this,
            &canvas::on_canvas_event);
    connect(&scene_, &canvas_scene::on_mouse_enter, this,
            &canvas::on_mouse_enter);
    connect(&scene_, &canvas_scene::on_mouse_leave, this,
            &canvas::on_mouse_leave);
    viewport_->setMouseTracking(true);
    viewport_->setTabletTracking(true);
}

void canvas::print_area(QPainter& to, const QRectF& area) const
{
    viewport_->render(&to, area);
}
auto canvas::scene_size() const -> QSizeF { return scene_.sceneRect().size(); }

void canvas::set_strokes(const std::vector<detail::stroke>& s)
{
    scene_.clear();
    std::for_each(s.begin(), s.end(),
                  [this](const auto& ds) { scene_.addItem(new stroke{ds}); });
    scene_.update();
}
void canvas::curr_mode(mode m) { curr_mode_ = m; }
void canvas::handle_pen_down(const QPointF& at)
{
    logger_->trace("handle_pen_down()");
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
    case mode::erase:
        QApplication::setOverrideCursor(eraser_cursor());
        handle_erase(at);
        break;
    }
    last_pt = at;
}
void canvas::handle_pen_up(const QPointF& at)
{
    logger_->trace("handle_pen_up()");
    switch (curr_mode_) {
    case mode::draw:
        if (pen_down_) {
            finish_stroke(at);
        }
        break;
    }
    pen_down_ = false;
}
void canvas::handle_pen_move(const QPointF& at)
{
    logger_->trace("handle_pen_move()");
    if (pen_down_) {
        switch (curr_mode_) {
        case mode::draw:
            add_stroke(at);
            break;
        case mode::erase:
            handle_erase(at);
            break;
        case mode::move:
            const auto diff = last_pt - at;
            logger_->trace("move: [{}]", diff);
            QRectF new_size{scene_.sceneRect().topLeft(),
                            scene_.sceneRect().size() +
                                QSizeF{diff.x(), diff.y()}};
            if (!scene_.sceneRect().contains(new_size)) {
                scene_.setSceneRect(new_size);
            }
            viewport_->verticalScrollBar()->setValue(
                viewport_->verticalScrollBar()->value() + diff.y());
            viewport_->horizontalScrollBar()->setValue(
                viewport_->horizontalScrollBar()->value() + diff.x());
            break;
        }

        last_pt = at;
    }
}
auto canvas::eraser_cursor() const -> QCursor
{
    return QCursor{erasor_cursor_bitmap()};
}
auto canvas::erasor_cursor_bitmap() const -> QPixmap
{
    const auto size = QSizeF{curr_weight_ * 2, curr_weight_ * 2};
    QPixmap img{size.toSize()};
    fill_with_transparent(img);
    const auto path =
        eraser_bounds(QPointF(size.width() / 2, size.height() / 2));
    QPainter p{&img};
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidthF(1);
    p.setPen(pen);
    p.drawPath(path);
    p.end();
    return img;
}

auto canvas::eraser_bounds(const QPointF& center) const -> QPainterPath
{
    QPainterPath erase_circle;
    const auto r = curr_weight_;
    erase_circle.addEllipse(center, r, r);
    return erase_circle;
}
void canvas::on_mouse_enter() const
{
    switch (curr_mode_) {
    case mode::draw:
        QApplication::setOverrideCursor(QCursor{Qt::CursorShape::CrossCursor});
        break;
    case mode::move:
        QApplication::setOverrideCursor(
            QCursor{Qt::CursorShape::DragMoveCursor});
        break;
    case mode::erase:
        QApplication::setOverrideCursor(eraser_cursor());
        break;
    }
}
void canvas::on_mouse_leave() const
{
    QApplication::setOverrideCursor(QCursor{});
}

void canvas::handle_erase(const QPointF& at)
{
    logger_->trace("handle_erase()");
    const auto area = eraser_bounds(at);
    const auto to_remove = scene_.items(area);
    if (!to_remove.empty()) {
        std::for_each(to_remove.begin(), to_remove.end(), [this](auto* item) {
            scene_.removeItem(item);
            delete item;
        });
        logger_->debug("erased {} items", to_remove.size());
    }
    else {
        logger_->trace("nothing to remove at: [{}]", at);
    }
}

void canvas_view::mouseReleaseEvent(QMouseEvent* e)
{
    QApplication::sendEvent(scene(), e);
}
void canvas_view::mouseMoveEvent(QMouseEvent* e)
{
    if (dragMode() == DragMode::ScrollHandDrag) {
        QGraphicsView::mouseMoveEvent(e);
    }
    else {
        QApplication::sendEvent(scene(), e);
    }
}
void canvas_view::mousePressEvent(QMouseEvent* e)
{
    if (dragMode() == DragMode::ScrollHandDrag) {
        QGraphicsView::mousePressEvent(e);
    }
    else {
        QApplication::sendEvent(scene(), e);
    }
}

bool canvas_scene::event(QEvent* e)
{
    if (e->type() == QEvent::Enter) {
        emit on_mouse_enter();
    }
    else if (e->type() == QEvent::Leave) {
        emit on_mouse_leave();
    }

    if (e->isPointerEvent()) {
        auto* pe = static_cast<QPointerEvent*>(e);
        emit on_pointer_event(pe);
        return true;
    }

    return QGraphicsScene::event(e);
}

bool canvas_view::event(QEvent* e)
{
    if (e->type() == QEvent::Leave || e->isPointerEvent()) {
        return QApplication::sendEvent(scene(), e);
    }

    return QGraphicsView::event(e);
}
void canvas::on_canvas_event(QPointerEvent* pe)
{
    for (const auto& pt : pe->points()) {
        const auto pos = viewport_->mapToScene(pt.position().toPoint());
        if (pen_down_) {
            curr_weight_ = pt.pressure() * weight_scaling_;
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
}
void canvas::prime_stroke(const QPointF& at) {}
template<typename T>
constexpr auto diff(T lhs, T rhs) -> T
{
    return lhs > rhs ? lhs - rhs : rhs - lhs;
}

void canvas::finish_stroke(const QPointF& at) {}
void canvas::add_stroke(const QPointF& at)
{
    auto* s = new stroke{{
        last_pt,
        at,
        curr_weight_,
        Qt::black,
    }};

    scene_.addItem(s);
    logger_->trace("add line: [{}] -> [{}]", last_pt, at);
}

auto canvas::strokes() const -> std::vector<detail::stroke>
{
    std::vector<detail::stroke> strokes;
    for (const auto* w : scene_.items()) {
        if (auto* s = dynamic_cast<const stroke*>(w)) {
            strokes.emplace_back(s->underlying());
        }
    }
    return strokes;
}

canvas::stroke::stroke(detail::stroke data) : data_{std::move(data)}
{
    setLine(QLineF{data_.start, data_.end});
    QPen p;
    p.setColor(data_.colour);
    p.setWidthF(data_.weight);
    p.setMiterLimit(8);
    p.setCapStyle(Qt::PenCapStyle::RoundCap);
    p.setStyle(Qt::PenStyle::SolidLine);
    p.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
    setPen(p);
}
} // namespace sketchy::ui
