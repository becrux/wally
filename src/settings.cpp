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
#include "engine.hpp"
#include "settings.hpp"
#include "ui/colorbutton.hpp"

#include <QComboBox>
#include <QFontMetrics>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImageReader>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QStackedLayout>
#include <QVBoxLayout>

using namespace Wally::Settings;

Toolbar::Model::Model(QAbstractItemModel *pSourceModel, QObject *pParent) :
  QAbstractProxyModel(pParent)
{
  setSourceModel(pSourceModel);
}

int Toolbar::Model::columnCount(const QModelIndex &cParent) const
{
  return sourceModel()->columnCount(cParent);
}

QVariant Toolbar::Model::data(const QModelIndex &index, int role) const
{
  if ((index.row() < 0) || (index.row() >= rowCount()))
    return QVariant();

  if ((index.row() >= 0) && (index.row() < (rowCount() - 1)))
    return sourceModel()->data(index, role);

  if (role != Qt::DisplayRole)
    return QVariant();

  return tr("Settings");
}

Qt::ItemFlags Toolbar::Model::flags(const QModelIndex &) const
{
  return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
}

QModelIndex Toolbar::Model::index(int iRow, int iColumn, const QModelIndex &) const
{
  return createIndex(iRow, iColumn);
}

QModelIndex Toolbar::Model::mapFromSource(const QModelIndex &sourceIndex) const
{
  return sourceIndex;
}

QModelIndex Toolbar::Model::mapToSource(const QModelIndex &proxyIndex) const
{
  return (proxyIndex.row() >= sourceModel()->rowCount())? QModelIndex() : proxyIndex;
}

QModelIndex Toolbar::Model::parent(const QModelIndex &cIndex) const
{
  return sourceModel()->parent(cIndex);
}

int Toolbar::Model::rowCount(const QModelIndex &cParent) const
{
  return sourceModel()->rowCount(cParent) + 1;
}

Toolbar::ItemDelegate::ItemDelegate(QObject *pParent) :
  QStyledItemDelegate(pParent)
{

}

void Toolbar::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (option.state.testFlag(QStyle::State_Selected))
    painter->fillRect(option.rect, option.palette.highlight());

  QPixmap pm = index.data(Qt::DecorationRole).value< QPixmap >();

  if (pm.isNull())
  {
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(option.rect, Qt::AlignCenter, index.data().toString());
    return;
  }

  QRect imgRect = pm.rect();
  imgRect.moveCenter(option.rect.center());
  painter->drawPixmap(imgRect.topLeft(), index.data(Qt::DecorationRole).value< QPixmap >());
}

QSize Toolbar::ItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
  return QSize(150, 50);
}

Toolbar::View::View(QWidget *pParent) :
  QListView(pParent)
{
  setItemDelegate(new Toolbar::ItemDelegate(this));

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  setSelectionMode(QAbstractItemView::SingleSelection);
}

QSize Toolbar::View::sizeHint() const
{
  return QSize(150, QListView::sizeHint().height());
}

GeneralWidget::GeneralWidget(QWidget *pParent) :
  QWidget(pParent),
  _sbTime(new QSpinBox(this)),
  _cbxTimeUnits(new QComboBox(this)),
  _clbBackground(new Ui::ColorButton(this)),
  _pbPosition(new QPushButton(this))
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  QGroupBox *gbGeneral = new QGroupBox(tr("General"), this);

  QGridLayout *generalLayout = new QGridLayout;

  generalLayout->addWidget(new QLabel(tr("Interval:"), this), 0, 0, 1, 2);
  generalLayout->addWidget(new QLabel(tr("Border:"), this), 0, 2);
  generalLayout->addWidget(new QLabel(tr("Position:"), this), 0, 3);

  generalLayout->addWidget(_sbTime, 1, 0);
  generalLayout->addWidget(_cbxTimeUnits, 1, 1);
  generalLayout->addWidget(_clbBackground, 1, 2);

  gbGeneral->setLayout(generalLayout);

  setLayout(mainLayout);
}

Dialog::Dialog(const Application *pApp, QWidget *pParent) :
  QDialog(pParent)
{
  QHBoxLayout *mainLayout = new QHBoxLayout;

  QAbstractItemView *view = new Toolbar::View(this);

  QAbstractItemModel *mdlEngines = new Toolbar::Model(new ::Wally::Engines::Model(pApp->engines(), this), this);
  view->setModel(mdlEngines);

  QItemSelectionModel *ismEngines = new QItemSelectionModel(mdlEngines);
  view->setSelectionModel(ismEngines);
  ismEngines->select(mdlEngines->index(0, 0), QItemSelectionModel::Select);

  mainLayout->addWidget(view);

  QVBoxLayout *rightLayout = new QVBoxLayout;

  QGroupBox *gbGeneral = new QGroupBox(this);
  gbGeneral->setTitle(tr("General"));
  rightLayout->addWidget(gbGeneral);

  QGroupBox *gbEngines = new QGroupBox(this);
  gbEngines->setTitle(tr("Engines"));
  rightLayout->addWidget(gbEngines);

  QStackedLayout *enginesLayout = new QStackedLayout;

  for (auto item : pApp->engines())
    enginesLayout->addWidget(item->settingsWidget(this));

  enginesLayout->addWidget(new GeneralWidget(this));

  gbEngines->setLayout(enginesLayout);

  connect(ismEngines, &QItemSelectionModel::selectionChanged, this,
    [enginesLayout] (const QItemSelection &selected, const QItemSelection &)
      {
        QModelIndexList mil = selected.indexes();
        if (mil.isEmpty())
          return;

        enginesLayout->setCurrentIndex(mil.at(0).row());
      }
  );

  mainLayout->addLayout(rightLayout);

  setLayout(mainLayout);
}
