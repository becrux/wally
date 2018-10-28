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

#include "wms/os2/native.hpp"

#include <QDir>

#define INCL_WINWORKPLACE
#include <os2.h>
#include <os2emx.h>

using namespace Wally::WindowManagers;

void Native::showPhoto(const QString &sFileName)
{
  HOBJECT hobj;
  const char *objName = "<WP_DESKTOP>";

  if (hobj = WinQueryObject(reinterpret_cast< const unsigned char * >(objName)))
    WinSetObjectData(
      hobj,
      reinterpret_cast< const unsigned char * >(
        QString("BACKGROUND=%1,S,1,I;").arg(
          QDir::toNativeSeparators(sFileName)
        ).toUtf8().constData()
      )
    );
}

::Wally::Image::Format Native::requestedFormat() const
{
  return ::Wally::Image::Format::BMP;
}
