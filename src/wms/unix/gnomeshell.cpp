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

#include "wms/unix/gnomeshell.hpp"

#include <QProcess>
#include <QUrl>

using namespace Wally::WindowManagers;

void GnomeShell::showPhoto(const QString &sFileName)
{
  QProcess::execute(
    "gsettings",

    QStringList{
      "set", "org.gnome.desktop.background", "draw-background", "true"
    }
  );

  QProcess::execute(
    "gsettings",

    QStringList{
      "set", "org.gnome.desktop.background", "picture-options", "centered"
    }
  );

  QProcess::execute(
    "gsettings",

    QStringList{
      "set",
      "org.gnome.desktop.background",
      "picture-uri",
      QUrl::fromLocalFile(sFileName).toString()
    }
  );
}

::Wally::Image::Format GnomeShell::requestedFormat() const
{
  return ::Wally::Image::Format::PNG;
}
