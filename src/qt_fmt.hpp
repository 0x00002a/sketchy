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

#include <fmt/format.h>
#include <qpoint.h>

#include <concepts>

namespace sketchy::detail {
template<typename T>
concept point = requires(const T& pt)
{
    {pt.x()};
    {pt.y()};
};
} // namespace sketchy::detail

template<sketchy::detail::point P>
struct fmt::formatter<P> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.end();
    }
    template<typename FormatContext>
    auto format(const P& p, FormatContext& ctx) -> decltype(ctx.out())
    {
        // ctx.out() is an output iterator to write to.
        return format_to(ctx.out(), "{}, {}", p.x(), p.y());
    }
};
