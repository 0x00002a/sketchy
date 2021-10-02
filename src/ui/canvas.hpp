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

namespace sketchy::ui {

class canvas : public QWidget {
    struct stroke {
        QPixmap img;
        QRectF bounds;
    };

public:
    canvas();

protected:
    void mouseMoveEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    void add_stroke(const QPointF& at);
    QPointF last_pt;
    QPen curr_pen_;
    std::vector<stroke> strokes_;
};
} // namespace sketchy::ui
