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

#include "application.hpp"
#include "trayicon.hpp"

#include "engines/flickr.hpp"

#include <QSystemTrayIcon>

using namespace Wally;

Application::Application(int &argc, char **argv) :
  QApplication(argc, argv),
  _trayIcon(new TrayIcon(this))
{
  _engines << new Engines::Flickr::Engine(this);

  QMetaObject::invokeMethod(_engines.at(0), "selectNext", Qt::QueuedConnection);
}
