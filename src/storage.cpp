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
CRONCH_META_TYPE(stroke,
                 (cm::field("s", &stroke::start), cm::field("e", &stroke::end),
                  cm::field("w", &stroke::weight),
                  cm::field("c", &stroke::colour)));

namespace sketchy {
namespace detail {

} // namespace detail

auto to_json(const std::vector<detail::stroke>& obj) -> std::string
{
    return cronch::serialize<cronch::json::boost>(obj);
}

auto from_json(const std::string& j) -> std::vector<detail::stroke>
{
    return cronch::deserialize<std::vector<detail::stroke>>(
        cronch::json::boost{j});
}
} // namespace sketchy
