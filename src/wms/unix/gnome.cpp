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

#include "wms/unix/gnome.hpp"

#include <QProcess>

using namespace Wally::WindowManagers;

void Gnome::showPhoto(const QString &sFileName)
{
  QProcess::execute(
    "gconftool-2",

    QStringList{
      "--type", "bool",
      "--set", "/desktop/gnome/background/draw_background", "true"
    }
  );

  QProcess::execute(
    "gconftool-2",

    QStringList{
      "--type", "string",
      "--set", "/desktop/gnome/background/picture_options", "centered"
    }
  );

  QProcess::execute(
    "gconftool-2",

    QStringList{
      "--type", "string",
      "--set", "/desktop/gnome/background/picture_filename", sFileName
    }
  );
}

::Wally::Image::Format Gnome::requestedFormat() const
{
  return ::Wally::Image::Format::PNG;
}
