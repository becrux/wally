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

#ifndef WALLY_APPLICATION_HPP
#define WALLY_APPLICATION_HPP

#include <QApplication>

namespace Wally
{
  class TrayIcon;

  namespace Engines
  {
    class Base;
  }

  class Application : public QApplication
  {
    Q_OBJECT

    TrayIcon *_trayIcon;
    QList< Engines::Base * > _engines;

  public:
    explicit Application(int &argc, char **argv);
  };
} // namespace Wally

#endif // APPLICATION_HPP
