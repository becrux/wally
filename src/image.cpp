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

#include "image.hpp"
#include "application.hpp"
#include "engine.hpp"
#include "exif.hpp"

#include <QApplication>
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QImage>
#include <QPainter>

using namespace Wally;

Image::Image(
    const QString &sFileName,
    bool bNormalizeImage,
    const QColor &cBackgroundColor,
    Qt::AlignmentFlag eTextAlignment
  ) :
  _fileName(sFileName),
  _normalizeImage(bNormalizeImage),
  _backgroundColor(cBackgroundColor),
  _textAlignment(eTextAlignment)
{

}

const char *Image::formatToString(Format eFormat)
{
  switch (eFormat)
  {
    case Format::BMP: return "bmp";
    case Format::JPG: return "jpg";
    case Format::PNG: return "png";
    case Format::PPM: return "ppm";
    case Format::XBM: return "xbm";
    case Format::XPM: return "xpm";
    default: return nullptr;
  }
}

QString Image::process(
  Format eOutputFormat,
  Position ePosition,
  const QVariantMap &cPictureInfo,
  const QRect &cDesktop) const
{
  QImage srcImage;

  if (!srcImage.load(_fileName))
    return QString();

  if (_normalizeImage)
    srcImage = Exif::Reader(_fileName).normalize(srcImage);

  Centers centers;

  QScopedPointer< QGraphicsScene > desktopScene(new QGraphicsScene(cDesktop));
  QImage outImage(cDesktop.width(), cDesktop.height(), QImage::Format_ARGB32);
  QPainter painter(&outImage);

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);

  switch (ePosition)
  {
    case Centered:
    case Tiled:
    case CenterTiled:
    default:
      break;

    case CenteredMaxpect:
    case TiledMaxpect:
      srcImage = srcImage.scaled(cDesktop.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
      break;

    case Scaled:
      srcImage = srcImage.scaled(cDesktop.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      break;

    case CenteredAutoFit:
      if ((srcImage.size().width() > cDesktop.width()) ||
          (srcImage.size().height() > cDesktop.height()))
        srcImage = srcImage.scaled(cDesktop.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
      break;

    case ScaleAndCrop:
      srcImage = srcImage.scaled(cDesktop.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
      break;
  }

  if (!_backgroundColor.isValid())
  {
    int x, y, r = 0, g = 0, b = 0;

    for (y = 0; y < srcImage.height(); ++y)
      for (x = 0; x < srcImage.width(); ++x)
      {
        QColor color = srcImage.pixel(x,y);

        r += color.red();
        g += color.green();
        b += color.blue();
      }

    int imageArea = srcImage.height() *  srcImage.width();

    desktopScene->setBackgroundBrush(
      QColor(r / imageArea, g / imageArea, b / imageArea)
    );
  }
  else
    desktopScene->setBackgroundBrush(_backgroundColor);

  calculateCenters(ePosition, cDesktop, srcImage.size(), centers);

  QHashIterator< QPoint, QPair< bool,bool > > center(centers);
  while (center.hasNext())
  {
    center.next();

    QPoint p(
      center.key().x() - srcImage.width() / 2 + cDesktop.left(),
      center.key().y() - srcImage.height() / 2 + cDesktop.top()
    );

    desktopScene->addPixmap(
      QPixmap::fromImage(
        srcImage.mirrored(
          ((ePosition == MirroredTiled) ||
           (ePosition == SymmetricalMirroredTiled)) && center.value().first,
          ((ePosition == MirroredTiled) ||
           (ePosition == SymmetricalMirroredTiled)) && center.value().second
        )
      )
    )->setPos(p);
  }

  int prevTop = cDesktop.top();

  QString value = cPictureInfo.value(TITLE).toString();
  if (!value.isEmpty())
    drawText(desktopScene.data(), value, _textAlignment, prevTop, true);

  value = cPictureInfo.value(AUTHOR).toString();
  if (!value.isEmpty())
    drawText(desktopScene.data(), QObject::tr("by: %1").arg(value), _textAlignment, prevTop);

  value = cPictureInfo.value(ENGINE).toString();
  if (!value.isEmpty())
    drawText(desktopScene.data(), QObject::tr("Engine: %1").arg(value), _textAlignment, prevTop);

  desktopScene->render(&painter);

  QString newFilename(
    QFileInfo(
      Application::dataDir(),
      QString("%1.%2")
        .arg(QDateTime::currentDateTime().toTime_t())
        .arg("png")
    ).absoluteFilePath()
  );
  outImage.save(newFilename, formatToString(eOutputFormat));

  return newFilename;
}

bool Image::isCenterInsideDesktop(const QPoint &cCenter, const QSize &cImageSize, const QRect &cDesktop)
{
  return (((cCenter.x() + cImageSize.width() / 2) >= cDesktop.left()) &&
          ((cCenter.y() + cImageSize.height() / 2) >= cDesktop.top()) &&
          ((cCenter.x() - cImageSize.width() / 2) <= cDesktop.right()) &&
          ((cCenter.y() - cImageSize.width() / 2) <= cDesktop.bottom()));
}

void Image::fillCenters(
  const QRect &cDesktop,
  const QSize &cImageSize,
  const QPoint &cCenter,
  bool bHorizontalFlip,
  bool bVerticalFlip,
  Centers &cCenters)
{
  if (!isCenterInsideDesktop(cCenter, cImageSize, cDesktop) || cCenters.contains(cCenter))
    return;

  cCenters.insert(cCenter, qMakePair(bHorizontalFlip, bVerticalFlip));

  fillCenters(
    cDesktop,
    cImageSize,
    QPoint(cCenter.x(), cCenter.y() - cImageSize.height()),
    bHorizontalFlip,
    !bVerticalFlip,
    cCenters
  );

  fillCenters(
    cDesktop,
    cImageSize,
    QPoint(cCenter.x(), cCenter.y() + cImageSize.height()),
    bHorizontalFlip,
    !bVerticalFlip,
    cCenters
  );

  fillCenters(
    cDesktop,
    cImageSize,
    QPoint(cCenter.x() - cImageSize.width(), cCenter.y()),
    !bHorizontalFlip,
    bVerticalFlip,
    cCenters
  );

  fillCenters(
    cDesktop,
    cImageSize,
    QPoint(cCenter.x() + cImageSize.width(), cCenter.y()),
    !bHorizontalFlip,
    bVerticalFlip,
    cCenters
  );
}

void Image::calculateCenters(
  Position ePosition,
  const QRect &cDesktop,
  QSize cImageSize,
  Centers &cCenters)
{
  switch (ePosition)
  {
    case Centered:
    case CenteredMaxpect:
    case Scaled:
    case CenteredAutoFit:
    case ScaleAndCrop:
      cCenters.insert(QPoint(cDesktop.width() / 2, cDesktop.height() / 2), qMakePair(false,false));
      break;

    case TiledMaxpect:
      cImageSize.scale(cDesktop.size(), Qt::KeepAspectRatio);

    case Tiled: /* or tiled maxpect */
    case MirroredTiled:
      fillCenters(
        cDesktop,
        cImageSize,
        QPoint(cImageSize.width() / 2, cImageSize.height() / 2),
        false,
        false,
        cCenters
      );
      break;

    case CenterTiled:
      fillCenters(
        cDesktop,
        cImageSize,
        QPoint(cImageSize.width() / 2, cImageSize.height() / 2),
        false,
        false,
        cCenters
      );
      break;

    case SymmetricalTiled:
    case SymmetricalMirroredTiled:
      fillCenters(
        cDesktop,
        cImageSize,
        QPoint(
          cDesktop.width() / 2 - cImageSize.width() / 2,
          cDesktop.height() / 2 - cImageSize.height() / 2
        ),
        false,
        false,
        cCenters
      );
      break;

    default:
      break;
  }
}

void Image::drawText(
  QGraphicsScene *scene,
  const QString &text,
  Qt::AlignmentFlag alignment,
  int &y,
  bool isTitle)
{
  qreal x;
  QGraphicsTextItem *textItem;
  QFont font(QApplication::font());

  font.setPointSize(10);
  font.setBold(isTitle);

  textItem = new QGraphicsTextItem;
  scene->addItem(textItem);
  textItem->setFont(font);
  textItem->setDefaultTextColor(Qt::white);
  if (isTitle)
    textItem->setHtml("\"" + text + "\"");
  else
    textItem->setHtml(text);

  switch (alignment)
  {
    case Qt::AlignRight:
    default:
      x = scene->width() - textItem->boundingRect().toRect().width();
      break;

    case Qt::AlignLeft:
      x = 2;
      break;
  }
  textItem->setZValue(1.0);
  textItem->setPos(x, y);

  textItem = new QGraphicsTextItem;
  scene->addItem(textItem);
  textItem->setFont(font);
  textItem->setDefaultTextColor(Qt::black);
  if (isTitle)
    textItem->setHtml("\"" + text + "\"");
  else
    textItem->setHtml(text);
  textItem->setPos(x+1, y+1);
  textItem->setZValue(0.5);

  y += textItem->boundingRect().toRect().height() + 1;
}

inline uint qHash(const QPoint &cKey, uint)
{
  return qHash((static_cast< quint64 >(cKey.x()) << 32) | static_cast< quint64 >(cKey.y()));
}
