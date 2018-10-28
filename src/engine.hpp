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

#include <QFileInfo>
#include <QObject>
#include <QWidget>
#include <QVariantMap>

constexpr const char *FILENAME = "fileName";
constexpr const char *TITLE = "title";
constexpr const char *SUBTITLE = "subtitle";
constexpr const char *DESCRIPTION = "description";
constexpr const char *LOCATION = "location";
constexpr const char *AUTHOR = "author";
constexpr const char *URL = "url";
constexpr const char *CREATION_DATE = "creationDate";
constexpr const char *WIDTH = "width";
constexpr const char *HEIGHT = "height";
constexpr const char *COPYRIGHT = "copyright";
constexpr const char *ENGINE = "engine";

namespace Wally
{
  namespace Engines
  {
    class SettingsWidget : public QWidget
    {
      Q_OBJECT

    public:
      explicit SettingsWidget(QWidget *parent = nullptr);

    public slots:
      virtual void accept() = 0;
    };

    class Base : public QObject
    {
      Q_OBJECT

    public:
      explicit Base(QObject *parent = nullptr);

      virtual SettingsWidget *settingsWidget(QWidget *parent) = 0;

      virtual QString name() const = 0;

    signals:
      void pictureAvailable(const QFileInfo &pictureFileName, const QVariantMap &pictureInfo);
      void failed();

    public slots:
      virtual void selectNext() = 0;
    };
  } // namespace Engines
} // namespace Wally
