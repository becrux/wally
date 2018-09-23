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

#include "src/exif.hpp"

#include <QSharedPointer>
#include <QTextCodec>
#include <QVector>

using namespace Wally::Exif;

void Reader::readEntry(ExifEntry *entry, void *data)
{
  QVector< char > buf(1024, '\0');

  Reader *me = reinterpret_cast< Reader * >(data);

  exif_entry_get_value(entry, buf.data(), buf.size());
  me->_exifData[entry->tag] = QString::fromUtf8(buf.constData());
}

void Reader::readContent(ExifContent *content, void *data)
{
  exif_content_foreach_entry(content, readEntry, data);
}

Reader::Reader(const QFileInfo &fileInfo)
{
  QFile f(fileInfo.absoluteFilePath());

  if (!f.open(QIODevice::ReadOnly))
    return;

  readExifData(f.readAll());
}

Reader::Reader(const QByteArray &data)
{
  readExifData(data);
}

void Reader::readExifData(const QByteArray &data)
{
  if (data.isEmpty())
    return;

  QSharedPointer< ExifData > ed(exif_data_new(), exif_data_free);
  exif_data_load_data(ed.get(), reinterpret_cast< const unsigned char * >(data.constData()), data.size());

  exif_data_foreach_content(ed.get(), readContent, reinterpret_cast< void * >(this));
}

QString Reader::owner() const
{
  QString s = _exifData.value(EXIF_TAG_XP_AUTHOR);

  return (s.isEmpty())? _exifData.value(EXIF_TAG_ARTIST) : s;
}

QString Reader::title() const
{
  QString s = _exifData.value(EXIF_TAG_XP_TITLE);

  return (s.isEmpty())? _exifData.value(EXIF_TAG_DOCUMENT_NAME) : s;
}

QString Reader::description() const
{
  return _exifData.value(EXIF_TAG_IMAGE_DESCRIPTION);
}

QImage Reader::normalize(const QImage &image) const
{
  switch (_exifData.value(EXIF_TAG_ORIENTATION).toInt())
  {
    case 2:
      return image.mirrored(true, false);

    case 3:
      return image.mirrored(true, true);

    case 4:
      return image.mirrored(false, true);

    case 5:
      return image.transformed(QTransform().rotate(90), Qt::SmoothTransformation).mirrored(true, false);

    case 6:
      return image.transformed(QTransform().rotate(90), Qt::SmoothTransformation);

    case 7:
      return image.transformed(QTransform().rotate(270), Qt::SmoothTransformation).mirrored(true, false);

    case 8:
      return image.transformed(QTransform().rotate(270),Qt::SmoothTransformation);

    default:
      return image;
  }
}
