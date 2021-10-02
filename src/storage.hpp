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

#include <variant>

#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>

namespace sketchy {
namespace detail {
class stroke {
public:
    struct line {
        QPointF start;
        QPointF end;
        float weight{0};
        QColor colour;

        auto operator==(const line& l) const -> bool
        {
            return l.start == start && l.end == end && l.weight == weight &&
                   l.colour == colour;
        }
    };

    QRectF bounds;
    QPointF offset;

    void update_bounds(const QPointF& at);

    void paint(QPainter& to, bool use_cache = true) const;
    void append(const line& l);

    auto parts() const -> const std::vector<line>& { return parts_; }
    void set_parts(const std::vector<line>& p) { parts_ = p; }

    auto operator==(const stroke& s) const -> bool
    {
        return s.parts_ == parts_;
    }

private:
    std::vector<line> parts_;
    bool img_updated_{false};
    mutable QPixmap cached_img_;
};
} // namespace detail


auto to_json(const std::vector<detail::stroke>& obj) -> std::string;

auto from_json(const std::string& j) -> std::vector<detail::stroke>;

} // namespace sketchy
