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

#include <qaction.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qwidget.h>

namespace sketchy::ui {

class radial_menu_segment : public QGraphicsItem {
public:
    explicit radial_menu_segment(QAction* act);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override {}
    auto boundingRect() const -> QRectF override { return bounds_; }

private:
    QAction* act_;
    QRectF bounds_;
};
class radial_menu : public QWidget {
    struct segment {
        QAction* act;
    };

public:
    radial_menu();

    void add_action(QAction* act);

    void set_radius(float r) { radius_ = r; }

private:
    auto calc_segment_pos(int num) const -> QPointF;
    void relayout() const;

    float radius_{0};
    QGraphicsScene scene_;
    QGraphicsView* viewport_;
    std::vector<radial_menu_segment*> parts_;
};

} // namespace sketchy::ui