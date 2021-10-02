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

#pragma once

#include <boost/system/detail/errc.hpp>
#include <qevent.h>
#include <qgraphicsview.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qwidget.h>

#include "logger.hpp"
#include "storage.hpp"

class QGraphicsView;
namespace sketchy::ui {

class canvas_view : public QGraphicsView {
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;

protected:
    bool event(QEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
signals:
    void on_pointer_event(QPointerEvent* ev) const;
};
class canvas_scene : public QGraphicsScene {
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;

protected:
    bool event(QEvent* e) override;
signals:
    void on_pointer_event(QPointerEvent* ev) const;
};
class canvas : public QWidget {
    Q_OBJECT
    using stroke = detail::stroke;

public:
    enum class mode {
        move,
        draw,
    };
    explicit canvas(logger_t logger);

    void curr_mode(mode m);

    auto strokes() const -> const std::vector<stroke>& { return {}; }
    void set_strokes(const std::vector<stroke>&);

private slots:
    void on_canvas_event(QPointerEvent* e);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void add_stroke(const QPointF& at);
    void prime_stroke(const QPointF& at);
    void finish_stroke(const QPointF& at);

    void handle_pen_down(const QPointF& at);
    void handle_pen_up(const QPointF& at);
    void handle_pen_move(const QPointF& at);

    mode curr_mode_{mode::draw};
    float zoom_{1};
    logger_t logger_;
    QPointF last_pt;
    QPen curr_pen_;
    bool pen_down_{false};
    stroke* active_stroke_{nullptr};
    std::vector<stroke*> strokes_;
    std::vector<QPixmap> raster_strokes_;
    QPointF move_offset_;
    canvas_scene scene_;
    canvas_view* viewport_;
    float curr_weight_{0};
};
} // namespace sketchy::ui
