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

#include <QFontMetrics>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QPainter>
#include <QStackedLayout>
#include <QVBoxLayout>

#include "application.hpp"
#include "engine.hpp"
#include "settingswindow.hpp"

using namespace Wally::Settings;

EnginesToolbar::ItemDelegate::ItemDelegate(QObject *pParent) :
  QAbstractItemDelegate(pParent)
{

}

void EnginesToolbar::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QPixmap pm = index.data(Qt::DecorationRole).value< QPixmap >();

  if (pm.isNull())
  {
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(option.rect, Qt::AlignCenter, index.data().toString());
    return;
  }

  painter->drawPixmap(option.rect, index.data(Qt::DecorationRole).value< QPixmap >());
}

QSize EnginesToolbar::ItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
  QPixmap pm = index.data(Qt::DecorationRole).value< QPixmap >();
  if (pm.isNull())
  {
    QFontMetrics fm(QApplication::font());
    return fm.size(Qt::TextSingleLine, index.data().toString());
  }

  return index.data(Qt::DecorationRole).value< QPixmap >().size();
}

EnginesToolbar::View::View(QWidget *pParent) :
  QListView(pParent)
{
  setItemDelegate(new EnginesToolbar::ItemDelegate(this));
}

Window::Window(const Application *pApp, QWidget *parent) :
  QDialog(parent)
{
  QHBoxLayout *mainLayout = new QHBoxLayout;

  QAbstractItemView *view = new EnginesToolbar::View(this);

  QAbstractItemModel *mdlEngines = new ::Wally::Engines::Model(pApp->engines(), this);
  view->setModel(mdlEngines);

  QItemSelectionModel *ismEngines = new QItemSelectionModel(mdlEngines);
  view->setSelectionModel(ismEngines);

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
