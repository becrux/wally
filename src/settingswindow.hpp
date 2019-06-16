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

#include <QAbstractItemDelegate>
#include <QAbstractListModel>
#include <QDialog>
#include <QListView>

class QPainter;
class QStackedLayout;

namespace Wally
{
  class Application;

  namespace Engines
  {
    class Base;
  }

  namespace Settings
  {
    namespace EnginesToolbar
    {
      class ItemDelegate : public QAbstractItemDelegate
      {
      public:
        explicit ItemDelegate(QObject *pParent = nullptr);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
      };

      class View : public QListView
      {
      public:
        explicit View(QWidget *pParent = nullptr);
      };
    }

    class Window : public QDialog
    {
      Q_OBJECT

    public:
      explicit Window(const Application *pApp, QWidget *pParent = nullptr);
    };
  } // namespace Settings
} // namespace Wally
