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

#include <qevent.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qwidget.h>

#include "logger.hpp"
#include "storage.hpp"

namespace sketchy::ui {

class canvas : public QWidget {
    using stroke = detail::stroke;

public:
    enum class mode {
        move,
        draw,

    };
    explicit canvas(logger_t logger);

    void curr_mode(mode m);

    auto strokes() const -> const std::vector<stroke>& { return strokes_; }
    void set_strokes(const std::vector<stroke>&);

protected:
    bool event(QEvent* e) override;
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
    stroke active_stroke_;
    std::vector<stroke> strokes_;
    std::vector<QPixmap> raster_strokes_;
    QPointF move_offset_;
    float curr_weight_{0};
};
} // namespace sketchy::ui
