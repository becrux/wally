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
#include "engines/ipernity.hpp"
#include "engines/deviantart.hpp"
#include "engines/imgur.hpp"

#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QSystemTrayIcon>

using namespace Wally;

Application *Application::_app = nullptr;

Application::Application(int &argc, char **argv) :
  QApplication(argc, argv),
  _settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, "BeCrux", "Wally", this)),
  _trayIcon(new TrayIcon(this))
{
  _app = this;
  setApplicationName("Wally");
  setOrganizationName("BeCrux");
  setApplicationVersion(WALLY_VERSION);

  _engines << new Engines::Flickr::Engine(this)
    << new Engines::Ipernity::Engine(this)
    << new Engines::DeviantArt::Engine(this)
    << new Engines::Imgur::Engine(this);

  QMetaObject::invokeMethod(_engines.at(3), "selectNext", Qt::QueuedConnection);
}

QDir Application::dataDir()
{
  static QDir dataDir = createDataDir();

  return dataDir;
}

QDir Application::createDataDir()
{
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  QDir::root().mkpath(path);

  return path;
}

QSettings &Application::storage()
{
  return *(_app->_settings);
}

SettingsGroupScope::SettingsGroupScope(QSettings &settings, const QString &scope) :
  _settings(settings)
{
  _settings.beginGroup(scope);
}

SettingsGroupScope::~SettingsGroupScope()
{
  _settings.endGroup();
}

SettingsArrayScope::SettingsArrayScope(QSettings &settings) :
  _settings(settings)
{

}

SettingsArrayScope::~SettingsArrayScope()
{
  _settings.endArray();
}

SettingsReadArrayScope::SettingsReadArrayScope(QSettings &settings, const QString &arrayName) :
  SettingsArrayScope(settings),
  _size(settings.beginReadArray(arrayName))
{

}

int SettingsReadArrayScope::size() const
{
  return _size;
}

SettingsWriteArrayScope::SettingsWriteArrayScope(QSettings &settings, const QString &arrayName, int size) :
  SettingsArrayScope(settings)
{
  settings.beginWriteArray(arrayName, size);
}
