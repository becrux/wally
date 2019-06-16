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
#include <QMenu>
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

  // QMetaObject::invokeMethod(_engines.at(3), "selectNext", Qt::QueuedConnection);
}

void Application::init()
{
  _engines << new Engines::Flickr::Engine(this)
    << new Engines::Ipernity::Engine(this)
    << new Engines::DeviantArt::Engine(this)
    << new Engines::Imgur::Engine(this);

  initTrayIcon();

  _trayIcon->setIcon(QIcon(":/images/off"));
  _trayIcon->setToolTip(QString("%1 %2").arg(applicationName()).arg(applicationVersion()));

  QMetaObject::invokeMethod(_trayIcon, "show", Qt::QueuedConnection);
}

void Application::initTrayIcon()
{
  connect(this, SIGNAL(aboutToQuit()), _trayIcon, SLOT(hide()));
  connect(_trayIcon, &QSystemTrayIcon::activated, this,
    [] (QSystemTrayIcon::ActivationReason ar)
      {
        if (ar == QSystemTrayIcon::DoubleClick)
        {

        }
      }
  );

  QMenu *mnuTrayIcon = new QMenu;
  connect(_trayIcon, SIGNAL(destroyed()), mnuTrayIcon, SLOT(deleteLater()));

  QAction *actPlay = new QAction(QIcon(":/images/control_play"), tr("Play"), this);
  connect(actPlay, SIGNAL(triggered()), this, SLOT(play()));
  mnuTrayIcon->addAction(actPlay);

  QAction *actNextPhoto = new QAction(QIcon(":/images/move_all_right"), tr("Next photo"), this);
  connect(actNextPhoto, SIGNAL(triggered()), this, SLOT(nextPhoto()));
  mnuTrayIcon->addAction(actNextPhoto);

  QAction *actSavePhoto = new QAction(QIcon(":/images/photo_save"), tr("Save photo"), this);
  connect(actSavePhoto, SIGNAL(triggered()), this, SLOT(savePhoto()));
  mnuTrayIcon->addAction(actSavePhoto);

  QAction *actShowExifInfo = new QAction(QIcon(":/images/exif_info"), tr("Show EXIF info"), this);
  connect(actShowExifInfo, SIGNAL(triggered()), this, SLOT(showExifInfo()));
  mnuTrayIcon->addAction(actShowExifInfo);

  QAction *actViewPhotoSource = new QAction(QIcon(":/images/source_url"), tr("View photo source"), this);
  connect(actViewPhotoSource, SIGNAL(triggered()), this, SLOT(viewPhotoSource()));
  mnuTrayIcon->addAction(actViewPhotoSource);

  QAction *actShowHistory = new QAction(QIcon(":/images/history"), tr("Show history..."), this);
  connect(actShowHistory, SIGNAL(triggered()), this, SLOT(showHistory()));
  mnuTrayIcon->addAction(actShowHistory);

  mnuTrayIcon->addSeparator();

  QAction *actConfigure = new QAction(QIcon(":/images/configure"), tr("Settings..."), this);
  connect(actConfigure, SIGNAL(triggered()), this, SLOT(configure()));
  mnuTrayIcon->addAction(actConfigure);

  mnuTrayIcon->addSeparator();

  QAction *actShowAbout = new QAction(QIcon(":/images/about"), tr("About..."), this);
  connect(actShowAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
  mnuTrayIcon->addAction(actShowAbout);

  QAction *actQuit = new QAction(QIcon(":/images/quit"), tr("Quit"), this);
  connect(actQuit, SIGNAL(triggered()), this, SLOT(quit()));
  mnuTrayIcon->addAction(actQuit);

  _trayIcon->setContextMenu(mnuTrayIcon);
}

void Application::play()
{

}

void Application::nextPhoto()
{

}

void Application::savePhoto()
{

}

void Application::showExifInfo()
{

}

void Application::viewPhotoSource()
{

}

void Application::configure()
{

}

void Application::showHistory()
{

}

void Application::showAbout()
{

}

QDir Application::dataDir()
{
  static QDir dataDir = createDataDir();

  return dataDir;
}

const QList< Engines::Base * > &Application::engines() const
{
  return _engines;
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
