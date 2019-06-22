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

#include <QAbstractProxyModel>
#include <QDialog>
#include <QListView>
#include <QStyledItemDelegate>

class QComboBox;
class QPainter;
class QSpinBox;
class QStackedLayout;

namespace Wally
{
  namespace Ui
  {
    class ColorButton;
  } // namespace Wally;

  class Application;

  namespace Engines
  {
    class Base;
  }

  namespace Settings
  {
    namespace Toolbar
    {
      class Model : public QAbstractProxyModel
      {
        Q_OBJECT

      public:
        explicit Model(QAbstractItemModel *pSourceModel, QObject *pParent = nullptr);

        int columnCount(const QModelIndex &cParent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
        QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
        QModelIndex parent(const QModelIndex &index) const override;
        int rowCount(const QModelIndex &cParent = QModelIndex()) const override;
      };

      class ItemDelegate : public QStyledItemDelegate
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

        QSize sizeHint() const override;
      };
    }

    class GeneralWidget : public QWidget
    {
      QSpinBox *_sbTime;
      QComboBox *_cbxTimeUnits;
      Ui::ColorButton *_clbBackground;
      QPushButton *_pbPosition;

    public:
      explicit GeneralWidget(QWidget *pParent = nullptr);
    };

    class Dialog : public QDialog
    {
      Q_OBJECT

    public:
      explicit Dialog(const Application *pApp, QWidget *pParent = nullptr);
    };
  } // namespace Settings
} // namespace Wally
