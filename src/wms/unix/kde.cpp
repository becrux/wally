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

#include "wms/unix/kde.hpp"

#include <QProcess>
#include <QUrl>

using namespace Wally::WindowManagers;

void KDE::showPhoto(const QString &sFileName)
{
  QProcess::execute(
    "qdbus",

    QStringList{
      "--session",
      "--dest=org.kde.plasmashell",
      "--type=method_call", "/PlasmaShell",
      "org.kde.PlasmaShell.evaluateScript",

      QString(
        "'string:"
          "var Desktops = desktops();"
          "for (i=0;i<Desktops.length;i++) {"
            "d = Desktops[i];"
            "d.wallpaperPlugin = \"org.kde.image\";"
            "d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");"
            "d.writeConfig(\"Image\", \"%1\");"
          "}"
        "'"

      ).arg(QUrl::fromLocalFile(sFileName).toString())
    }
  );
}

::Wally::Image::Format KDE::requestedFormat() const
{
  return ::Wally::Image::Format::PNG;
}
