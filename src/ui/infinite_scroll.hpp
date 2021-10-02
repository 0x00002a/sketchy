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

#include <qabstractscrollarea.h>

namespace sketchy::ui {

class infinite_scroller : public QAbstractScrollArea {
public:
    infinite_scroller();
    void widget(QWidget* w);

protected:
    void scrollContentsBy(int dx, int dy) override;

private:
    QWidget* inner_{nullptr};
};

} // namespace sketchy::ui