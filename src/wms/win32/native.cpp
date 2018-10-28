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

#include "wms/win32/native.hpp"

#include <QDir>
#include <QSettings>

#include <windows.h>
#include <wininet.h>
#include <shlobj.h>

using namespace Wally::WindowManagers;

void Native::showPhoto(const QString &sFileName)
{
  SHELLFLAGSTATE shfs;

  SHGetSettings(&shfs, SSF_DESKTOPHTML);
  if (shfs.fDesktopHTML)
    return;

  QString win32FN = QDir::toNativeSeparators(sFileName);

  QSettings regSettings("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
  regSettings.setValue("Wallpaper", win32FN);
  regSettings.setValue("WallpaperStyle","0");
  regSettings.setValue("TileWallpaper","0");

  #ifdef UNICODE
    WCHAR path[MAX_PATH];

    memset(path, 0, MAX_PATH * sizeof(WCHAR));
    QDir::toNativeSeparators(sFileName).toWCharArray(path);

    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, reinterpret_cast< void * >(path), SPIF_SENDWININICHANGE);
  #else
    SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, reinterpret_cast< void * >(win32FN.toUtf8().data()), SPIF_SENDWININICHANGE);
  #endif
}

::Wally::Image::Format Native::requestedFormat() const
{
  return ::Wally::Image::Format::BMP;
}
