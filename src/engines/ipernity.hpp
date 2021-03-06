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

#include "engine.hpp"

#include <QWidget>

class QNetworkAccessManager;

namespace Wally
{
  namespace Engines
  {
    namespace Ipernity
    {
      class Engine : public ::Wally::Engines::Base
      {
        Q_OBJECT

        QNetworkAccessManager *_networkAccessManager;

      private slots:
        void processSearchResult();
        void processInfoQueryResult();
        void savePhoto();

      public:
        explicit Engine(QObject *pParent = nullptr);

        QString name() const override;
        QPixmap icon() const override;

        ::Wally::Engines::SettingsWidget *settingsWidget(QWidget *parent) override;

      public slots:
        void selectNext() override;
      };

      class SettingsWidget : public ::Wally::Engines::SettingsWidget
      {
      public:
        explicit SettingsWidget(QWidget *pParent = nullptr);

        void accept() override;
      };
    } // namespace Ipernity
  } // namespace Engines
} // namespace Wally
