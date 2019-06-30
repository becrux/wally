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
#include "ui/positionbutton.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFontMetrics>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImageReader>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
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
  _cbRandomOrder(new QCheckBox(tr("Choose in random order"), this)),
  _cbSwitchOnPlay(new QCheckBox(tr("Switch photo on play"), this)),
  _cbPlayOnStart(new QCheckBox(tr("Play on application start"), this)),
  _cbStartOnLogon(new QCheckBox(tr("Start at system logon"), this)),
  _cbOnlyLandscapePhotos(new QCheckBox(tr("Only use landscape photos"), this)),
  _cbRotateUsingEXIF(new QCheckBox(tr("Rotate photos according to EXIF"), this)),
  _cbxInfoPhotoCorner(new QComboBox(this)),
  _cbInfoInSysTray(new QCheckBox(tr("View info in system tray tooltip"), this)),
  _cbUseFullDesktopArea(new QCheckBox(tr("Use full desktop area"), this)),
  _cbxPhotoMinimumAllowedSize(new QComboBox(this)),
  _sbMinimimumDiskQuota(new QSpinBox(this)),
  _cbxMinimumDiskQuotaUnits(new QComboBox(this)),
  _sbHistoryLength(new QSpinBox(this)),
  _cbxHistoryLengthUnits(new QComboBox(this)),
  _cbxProxyType(new QComboBox(this)),
  _leProxyServer(new QLineEdit(this)),
  _sbProxyPort(new QSpinBox(this)),
  _leProxyUsername(new QLineEdit(this)),
  _leProxyPassword(new QLineEdit(this))
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  mainLayout->addWidget(_cbRandomOrder);
  mainLayout->addWidget(_cbSwitchOnPlay);
  mainLayout->addWidget(_cbPlayOnStart);
  mainLayout->addWidget(_cbStartOnLogon);
  mainLayout->addWidget(_cbOnlyLandscapePhotos);
  mainLayout->addWidget(_cbRotateUsingEXIF);

  QHBoxLayout *ipcLayout = new QHBoxLayout;
  ipcLayout->addWidget(new QLabel(tr("View info at:"), this));
  ipcLayout->addWidget(_cbxInfoPhotoCorner);
  _cbxInfoPhotoCorner->addItem(tr("No corner"));
  _cbxInfoPhotoCorner->addItem(tr("Top left"));
  _cbxInfoPhotoCorner->addItem(tr("Top right"));
  _cbxInfoPhotoCorner->addItem(tr("Bottom left"));
  _cbxInfoPhotoCorner->addItem(tr("Bottom right"));
  mainLayout->addLayout(ipcLayout);

  mainLayout->addWidget(_cbInfoInSysTray);
  mainLayout->addWidget(_cbUseFullDesktopArea);

  QHBoxLayout *masLayout = new QHBoxLayout;
  masLayout->addWidget(new QLabel(tr("Photo has to be"), this));
  masLayout->addWidget(_cbxPhotoMinimumAllowedSize);
  masLayout->addWidget(new QLabel(tr("size of the desktop"), this));
  _cbxPhotoMinimumAllowedSize->addItem(tr("independent of"));
  _cbxPhotoMinimumAllowedSize->addItem(tr("at least 1/2 of"));
  _cbxPhotoMinimumAllowedSize->addItem(tr("at least 3/4 of"));
  _cbxPhotoMinimumAllowedSize->addItem(tr("bigger than"));
  mainLayout->addLayout(masLayout);

  setLayout(mainLayout);
}

Dialog::Dialog(const Application *pApp, QWidget *pParent) :
  QDialog(pParent),
  _sbTime(new QSpinBox(this)),
  _cbxTimeUnits(new QComboBox(this)),
  _clbBackground(new Ui::ColorButton(this)),
  _pbPosition(new Ui::PositionButton(this))
{
  QVBoxLayout *dialogLayout = new QVBoxLayout;

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

  QGridLayout *generalLayout = new QGridLayout;
  generalLayout->setColumnStretch(2, 1);
  generalLayout->setColumnStretch(4, 1);

  generalLayout->addWidget(new QLabel(tr("Interval:"), this), 0, 0, 1, 2);
  generalLayout->addWidget(new QLabel(tr("Border:"), this), 0, 3);
  generalLayout->addWidget(new QLabel(tr("Position:"), this), 0, 5);

  generalLayout->addWidget(_sbTime, 1, 0);
  generalLayout->addWidget(_cbxTimeUnits, 1, 1);
  generalLayout->addWidget(_clbBackground, 1, 3);
  generalLayout->addWidget(_pbPosition, 1, 5);

  _cbxTimeUnits->addItem(tr("Seconds"), 1);
  _cbxTimeUnits->addItem(tr("Minutes"), 60);
  _cbxTimeUnits->addItem(tr("Hours"), 3600);
  _cbxTimeUnits->addItem(tr("Days"), 86400);
  _cbxTimeUnits->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  gbGeneral->setLayout(generalLayout);

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

  dialogLayout->addLayout(mainLayout);

  QDialogButtonBox *dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                         Qt::Horizontal, this);
  connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()));

  dialogLayout->addWidget(dialogButtons);

  setLayout(dialogLayout);
}
