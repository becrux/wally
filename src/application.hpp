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

#pragma once

#include <QApplication>
#include <QDir>

class QSettings;

namespace Wally
{
  class TrayIcon;

  enum class FileFormats
  {
    BMP,
    JPG,
    PNG,
    PPM,
    XBM,
    XPM
  };

  namespace Engines
  {
    class Base;
  }

  class SettingsGroupScope
  {
    QSettings &_settings;

  public:
    explicit SettingsGroupScope(QSettings &settings, const QString &scope);
    ~SettingsGroupScope();
  };

  class SettingsArrayScope
  {
    QSettings &_settings;

  protected:
    explicit SettingsArrayScope(QSettings &settings);

  public:
    ~SettingsArrayScope();
  };

  class SettingsReadArrayScope : public SettingsArrayScope
  {
    int _size;

  public:
    explicit SettingsReadArrayScope(QSettings &settings, const QString &arrayName);

    int size() const;
  };

  class SettingsWriteArrayScope : public SettingsArrayScope
  {
  public:
    explicit SettingsWriteArrayScope(QSettings &settings, const QString &arrayName, int size);
  };

  class Application : public QApplication
  {
    Q_OBJECT

    static Application *_app;

    QSettings *_settings;
    TrayIcon *_trayIcon;
    QList< Engines::Base * > _engines;

    static QDir createDataDir();

  public:
    explicit Application(int &argc, char **argv);

    static QDir dataDir();

    static QSettings &storage();
  };
} // namespace Wally
