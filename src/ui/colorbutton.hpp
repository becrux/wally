/*
  Wally
  Copyright (C) 2018 BeCrux

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QColor>
#include <QObject>
#include <QPushButton>

namespace Wally
{
  namespace Ui
  {
    class ColorButton : public QPushButton
    {
      Q_OBJECT

      QColor _color;

    protected:
      void paintEvent(QPaintEvent *event) override;
      void nextCheckState() override;

    public:
      explicit ColorButton(QWidget *parent = nullptr);

      QColor color() const;
      void setColor(const QColor &color);

      // QSize sizeHint() const override;

    signals:
      void colorChanged(const QColor &cNewColor);
    };
  }
}
