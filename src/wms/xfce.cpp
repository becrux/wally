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

#include "wms/xfce.hpp"

#include <QFile>
#include <QProcess>
#include <QTextStream>

using namespace Wally::WindowManagers;

void XFCE::showPhoto(const QString &sFileName)
{
  QProcess p;

  p.start(
    "xfconf-query",

    QStringList{
      "-c", "xfce4-desktop", "-p", "/backdrop", "-lv"
    },

    QIODevice::ReadOnly
  );
  p.waitForFinished();

  QStringList monitors, workspaces;

  if (p.exitStatus() == QProcess::NormalExit)
  {
    QTextStream r(&p);
    QString line;

    while (!(line = r.readLine()).isNull())
      if (line.contains("image-path"))
        monitors << QFileInfo(line.section(" ", 0, 0)).path();
      else if (line.contains("last-image"))
        workspaces << QFileInfo(line.section(" ", 0, 0)).path();
  }

  if (!monitors.isEmpty() || !workspaces.isEmpty())
  {
    QStringListIterator mIter(monitors);
    while (mIter.hasNext())
    {
      QString ms(mIter.next());

      QProcess::execute(
        "xfconf-query",

        QStringList{
          "-c", "xfce4-desktop", "-p", QString("%1/image-show").arg(ms), "-s", "true"
        }
      );

      QProcess::execute(
        "xfconf-query",

        QStringList{
          "-c", "xfce4-desktop", "-p", QString("%1/image-style").arg(ms), "-s", "1"
        }
      );

      QProcess::execute(
        "xfconf-query",

        QStringList{
          "-c", "xfce4-desktop", "-p", QString("%1/image-path").arg(ms), "-s", sFileName
        }
      );
    }

    QStringListIterator wIter(workspaces);
    while (wIter.hasNext())
    {
      QString ws(wIter.next());

      QProcess::execute(
        "xfconf-query",

        QStringList{
          "-c", "xfce4-desktop", "-p", QString("%1/color-style").arg(ws), "-s", "3"
        }
      );

      QProcess::execute(
        "xfconf-query",

        QStringList{
          "-c", "xfce4-desktop", "-p", QString("%1/image-style").arg(ws), "-s", "1"
        }
      );

      QProcess::execute(
        "xfconf-query",

        QStringList{
          "-c", "xfce4-desktop", "-p", QString("%1/last-image").arg(ws), "-s", sFileName
        }
      );
    }
  }
  else
  {
    {
      QFile f(QFileInfo(QDir::homePath(), ".config/xfce4/desktop/backdrops.list").absoluteFilePath());

      if (!f.open(QIODevice::WriteOnly))
        return;

      QTextStream ts(&f);

      ts << "# xfce backdrop list" << endl;
      ts << sFileName << endl;
    }

    QProcess::execute("xfdesktop", QStringList{ "--reload" });
  }
}

::Wally::FileFormats XFCE::requestedFormat() const
{
  return ::Wally::FileFormats::PNG;
}
