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

#include "wms/macosx/native.hpp"
#include "application.hpp"

#include <QFile>
#include <QProcess>
#include <QTextStream>

using namespace Wally::WindowManagers;

void Native::showPhoto(const QString &sFileName)
{
  QString wallyScript = QFileInfo(Application::dataDir(), "wally.scpt").absoluteFilePath();

  {
    QFile f(wallyScript);
    QTextStream ts(&f);

    if (!f.open(QIODevice::WriteOnly))
      return;

    if (QSysInfo::MacintoshVersion == QSysInfo::MV_10_6)
    {
      ts << "tell application \"System Events\"" << endl;
      ts << "set picture of every desktop to POSIX file \"" << sFileName << "\"" << endl;
      ts << "end tell" << endl;
    }
    else
    {
      ts << "tell application \"Finder\"" << endl;
      ts << "set desktop picture to POSIX file \"" << sFileName << "\"" << endl;
      ts << "end tell" << endl;
    }
  }

  QProcess::execute("osascript", QStringList{ wallyScript });
}

::Wally::Image::Format Native::requestedFormat() const
{
  return ::Wally::Image::Format::PNG;
}
