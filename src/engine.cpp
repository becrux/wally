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

#include <QLabel>

#include "engine.hpp"

using namespace Wally::Engines;

Base::Base(QObject *pParent) :
  QObject(pParent)
{

}

SettingsWidget::SettingsWidget(QWidget *pParent) :
  QWidget(pParent)
{

}

Model::Model(QList< Base * > lEngines, QObject *pParent) :
  QAbstractListModel(pParent),
  _engines(lEngines)
{

}

QVariant Model::data(const QModelIndex &cIndex, int iRole) const
{
  if ((cIndex.column() != 0) || (cIndex.row() < 0) || (cIndex.row() >= _engines.size()))
    return QVariant();

  switch (iRole)
  {
    case Qt::DisplayRole:
      return _engines.at(cIndex.row())->name();

    case Qt::DecorationRole:
      return _engines.at(cIndex.row())->icon();

    default:
      return QVariant();
  }
}

Qt::ItemFlags Model::flags(const QModelIndex &) const
{
  return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
}

QModelIndex Model::index(int iRow, int iColumn, const QModelIndex &) const
{
  return createIndex(iRow, iColumn);
}

QModelIndex Model::parent(const QModelIndex &) const
{
  return QModelIndex();
}

int Model::rowCount(const QModelIndex &) const
{
  return _engines.size();
}
