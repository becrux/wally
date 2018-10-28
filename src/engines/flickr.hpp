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
    namespace Flickr
    {
      struct Item
      {
        enum TagsCondition
        {
          And = 0,
          Or = 1,
          TagsConditionSize = 2
        };

        enum SearchOrder
        {
          DatePostedAsc = 0,
          DatePostedDesc = 1,
          DateTakenAsc = 2,
          DateTakenDesc = 3,
          InterestingnessAsc = 4,
          InterestingnessDesc = 5,
          Relevance = 6,
          SearchOrderSize = 7
        };

        TagsCondition tagsCondition;
        SearchOrder searchOrder;
        QStringList tags;
        QString text;
        int currentPage;
        int pagesCount;
      };

      class Engine : public ::Wally::Engines::Base
      {
        Q_OBJECT

        int _currentItem;
        QList< Item > _items;
        QNetworkAccessManager *_networkAccessManager;

      private slots:
        void processSearchResult();
        void processSizeQueryResult();
        void savePhoto();
        void processPhotoInfo();
        void updateStorage();

      public:
        explicit Engine(QObject *parent = nullptr);

        QString name() const override;

        ::Wally::Engines::SettingsWidget *settingsWidget(QWidget *parent) override;

      public slots:
        void selectNext() override;
      };

      class SettingsWidget : public ::Wally::Engines::SettingsWidget
      {
      public:
        explicit SettingsWidget(QWidget *parent = nullptr);

        void accept() override;
      };
    } // namespace Flickr
  } // namespace Engines
} // namespace Wally
