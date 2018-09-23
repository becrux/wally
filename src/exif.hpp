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

#ifndef WALLY_EXIF_HPP
#define WALLY_EXIF_HPP

extern "C"
{
  #include <exif-content.h>
}

#include <QByteArray>
#include <QFileInfo>
#include <QImage>
#include <QString>
#include <QVariant>

namespace Wally
{
  namespace Exif
  {
    class Reader
    {
      QMap< int, QString > _exifData;

      void readExifData(const QByteArray &data);

      static void readEntry(ExifEntry *entry, void *data);
      static void readContent(ExifContent *content, void *data);

    public:
      explicit Reader(const QFileInfo &fileInfo);
      explicit Reader(const QByteArray &data);

      QString owner() const;
      QString title() const;
      QString description() const;

      QImage normalize(const QImage &image) const;
    };
  }
}

#endif
