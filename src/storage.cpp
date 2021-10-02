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

#include "storage.hpp"

#include <qpainter.h>

namespace sketchy {
namespace detail {
void stroke::update_bounds(const QPointF& at)
{
    auto& b = bounds;

    b.setTopLeft(QPointF{std::min(at.x(), b.topLeft().x()),
                         std::min(at.y(), b.topLeft().y())});
    b.setBottomRight(QPointF{std::max(at.x(), b.bottomRight().x()),
                             std::max(at.y(), b.bottomRight().y())});
}

void stroke::paint(QPainter& to, bool use_cache) const
{
    to.save();
    std::for_each(parts.begin(), parts.end(), [&, this](const line& part) {
        QPen p;
        p.setColor(part.colour);
        p.setWidthF(part.weight);
        to.setPen(p);
        to.drawLine(part.start, part.end);
    });
    to.restore();
}
void stroke::append(const line& l)
{
    img_updated_ = true;
    parts.emplace_back(l);
}

} // namespace detail

} // namespace sketchy