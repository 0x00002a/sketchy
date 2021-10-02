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
#include "cronch/deserialize.hpp"
#include "cronch/metadata.hpp"
#include "cronch/serialize.hpp"

#include <qpainter.h>
#include <qpainterpath.h>

#include <cronch/json/boost.hpp>
#include <cronch/meta.hpp>

using namespace sketchy::detail;
namespace cm = cronch::meta;

CRONCH_META_TYPE(QPointF, (cm::property("x", &QPointF::x, &QPointF::setX),
                           cm::property("y", &QPointF::y, &QPointF::setY)));
CRONCH_META_TYPE(QRectF,
                 (cm::property("tl", &QRectF::topLeft, &QRectF::setTopLeft),
                  cm::property("br", &QRectF::bottomRight,
                               &QRectF::setBottomRight)));

template<>
struct cronch::json::boost::converter<QColor> {
    static void to_json(::boost::json::value& v, const QColor& c)
    {
        v = ::boost::json::string(c.name().toStdString().c_str());
    }
    static void from_json(const ::boost::json::value& v, QColor& c)
    {
        c = QColor{v.as_string().c_str()};
    }
};
CRONCH_META_TYPE(stroke::line, (cm::field("s", &stroke::line::start),
                                cm::field("e", &stroke::line::end),
                                cm::field("w", &stroke::line::weight),
                                cm::field("c", &stroke::line::colour)));
CRONCH_META_TYPE(stroke,
                 (cm::field("off", &stroke::offset),
                  // cm::field("boun", &stroke::bounds_),
                  cm::property("pts", &stroke::parts, &stroke::set_parts)));

namespace sketchy {
namespace detail {
void stroke::update_bounds(const QPointF& at)
{
    auto& b = bounds_;

    b.setTopLeft(QPointF{std::min(at.x(), b.topLeft().x()),
                         std::min(at.y(), b.topLeft().y())});
    b.setBottomRight(QPointF{std::max(at.x(), b.bottomRight().x()),
                             std::max(at.y(), b.bottomRight().y())});
}

void stroke::paint(QPainter* to, const QStyleOptionGraphicsItem* option,
                   QWidget* w)
{
    to->save();
    /*std::for_each(parts_.begin(), parts_.end(), [&, this](const line& part) {
        QPen p;
        p.setColor(part.colour);
        p.setWidthF(part.weight);
        to.setPen(p);
        to.drawLine(part.start, part.end);
    });*/
}
void stroke::append(line* l)
{
    l->setParentItem(this);
    parts_.emplace_back(l);
}
void stroke::line::paint(QPainter* to, const QStyleOptionGraphicsItem* option,
                         QWidget* w)
{
    to->save();
    QPen p{colour};
    p.setWidthF(weight);

    to->setPen(p);
    to->drawLine(start, end);
    to->restore();
}

auto stroke::handle_erase(const QPainterPath& area) -> bool { return false; }
} // namespace detail

auto to_json(const std::vector<detail::stroke>& obj) -> std::string
{
    // return cronch::serialize<cronch::json::boost>(obj);
}

auto from_json(const std::string& j) -> std::vector<detail::stroke>
{
    /*return cronch::deserialize<std::vector<detail::stroke>>(
        cronch::json::boost{j});*/
}
} // namespace sketchy
