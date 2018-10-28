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
#include <QHash>
#include <QPair>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QVariantMap>

class QGraphicsScene;

namespace Wally
{
  class Image
  {
  public:
    enum class Format
    {
      BMP,
      JPG,
      PNG,
      PPM,
      XBM,
      XPM
    };

    enum Position
    {
      Centered = 0,
      Tiled = 1,
      CenterTiled = 2,
      CenteredMaxpect = 3,
      TiledMaxpect = 4,
      Scaled = 5,
      CenteredAutoFit = 6,
      ScaleAndCrop = 7,
      SymmetricalTiled = 8,
      MirroredTiled = 9,
      SymmetricalMirroredTiled = 10
    };

    using Centers = QHash< QPoint, QPair< bool,bool > >;

  private:
    QString _fileName;
    bool _normalizeImage;
    QColor _backgroundColor;
    Qt::AlignmentFlag _textAlignment;

    static bool isCenterInsideDesktop(const QPoint &cCenter, const QSize &cImageSize, const QRect &cDesktop);

    static void fillCenters(
      const QRect &cDesktop,
      const QSize &cImageSize,
      const QPoint &center,
      bool bHorizontalFlip,
      bool bVerticalFlip,
      Centers &cCenters
    );

    static void calculateCenters(Position ePosition, const QRect &cDesktop, QSize cImageSize, Centers &cCenters);

    static void drawText(
      QGraphicsScene *scene,
      const QString &text,
      Qt::AlignmentFlag alignment,
      int &y,
      bool isTitle = false
    );

    static const char *formatToString(Format eFormat);

  public:
    explicit Image(
      const QString &sFileName,
      bool bNormalizeImage,
      const QColor &cBackgroundColor,
      Qt::AlignmentFlag eTextAlignment);

    QString process(
      Format eOutputFormat,
      Position ePosition,
      const QVariantMap &cPictureInfo,
      const QRect &cDesktop
    ) const;
  };
}
