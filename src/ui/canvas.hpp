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

namespace sketchy::ui {

class canvas : public QWidget {
    struct stroke {
        QPixmap img;
        QPointF start;
        QPointF end;
        QRectF bounds;
    };

public:
    explicit canvas(logger_t logger);

protected:
    bool event(QEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    void add_stroke(const QPointF& at);
    void prime_stroke(const QPointF& at);
    void finish_stroke(const QPointF& at);

    logger_t logger_;
    QPointF last_pt;
    QPen curr_pen_;
    bool pen_down_{false};
    stroke active_stroke_;
    std::vector<stroke> strokes_;
};
} // namespace sketchy::ui
