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

#include "positionbutton.hpp"

#include <QAbstractTableModel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QImageReader>
#include <QPainter>
#include <QScopedPointer>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QVBoxLayout>

namespace
{
  QString positionToString(::Wally::Wallpaper::Position ePosition)
  {
    switch (ePosition)
    {
      case Wally::Wallpaper::Position::Centered:
        return QObject::tr("Centered");

      case Wally::Wallpaper::Position::Tiled:
        return QObject::tr("Tiled");

      case Wally::Wallpaper::Position::CenterTiled:
        return QObject::tr("Center Tiled");

      case Wally::Wallpaper::Position::CenteredMaxpect:
        return QObject::tr("Centered Maxpect");

      case Wally::Wallpaper::Position::TiledMaxpect:
        return QObject::tr("Tiled Maxpect");

      case Wally::Wallpaper::Position::Scaled:
        return QObject::tr("Scaled");

      case Wally::Wallpaper::Position::CenteredAutoFit:
        return QObject::tr("Centered Auto Fit");

      case Wally::Wallpaper::Position::ScaleAndCrop:
        return QObject::tr("Scale & Crop");

      case Wally::Wallpaper::Position::SymmetricalTiled:
        return QObject::tr("Symmetrical Tiled");

      case Wally::Wallpaper::Position::MirroredTiled:
        return QObject::tr("Mirrored Tiled");

      case Wally::Wallpaper::Position::SymmetricalMirroredTiled:
        return QObject::tr("Symmetrical Mirrored Tiled");

      default:
        return QString();
    }
  }

  class Model : public QAbstractTableModel
  {
  public:
    explicit Model(QObject *pParent = nullptr) :
      QAbstractTableModel(pParent)
    {

    }

    int columnCount(const QModelIndex & = QModelIndex()) const override
    {
      return 2;
    }

    QVariant data(const QModelIndex &cIndex, int iRole = Qt::DisplayRole) const override
    {
      if (!cIndex.isValid() || (iRole != Qt::DisplayRole))
        return QVariant();

      switch (cIndex.column())
      {
        case 0:
          return positionToString(static_cast< Wally::Wallpaper::Position >(cIndex.row()));

        case 1:
          switch (static_cast< Wally::Wallpaper::Position >(cIndex.row()))
          {
            case Wally::Wallpaper::Position::Centered:
              return ":/images/centered";

            case Wally::Wallpaper::Position::Tiled:
              return ":/images/tiled";

            case Wally::Wallpaper::Position::CenterTiled:
              return ":/images/center_tiled";

            case Wally::Wallpaper::Position::CenteredMaxpect:
              return ":/images/centered_maxpect";

            case Wally::Wallpaper::Position::TiledMaxpect:
              return ":/images/tiled_maxpect";

            case Wally::Wallpaper::Position::Scaled:
              return ":/images/scaled";

            case Wally::Wallpaper::Position::CenteredAutoFit:
              return ":/images/centered_autofit";

            case Wally::Wallpaper::Position::ScaleAndCrop:
              return ":/images/scale_crop";

            case Wally::Wallpaper::Position::SymmetricalTiled:
              return ":/images/symmetrical_tiled";

            case Wally::Wallpaper::Position::MirroredTiled:
              return ":/images/mirrored_tiled";

            case Wally::Wallpaper::Position::SymmetricalMirroredTiled:
              return ":/images/symmetrical_mirrored_tiled";

            default:
              return QString();
          }

        default:
          return QVariant();
      }
    }

    QVariant headerData(int iSection, Qt::Orientation eOrientation, int iRole = Qt::DisplayRole) const override
    {
      if ((eOrientation == Qt::Vertical) || (iRole != Qt::DisplayRole))
        return QVariant();

      switch (iSection)
      {
        case 0:
          return QObject::tr("Position");

        case 1:
          return QObject::tr("Picture smaller than screen on the left, greater than screen on the right");

        default:
          return QVariant();
      }
    }

    int rowCount(const QModelIndex & = QModelIndex()) const override
    {
      return static_cast< int >(Wally::Wallpaper::Position::SymmetricalMirroredTiled) + 1;
    }
  };

  class PixmapItemDelegate : public QStyledItemDelegate
  {
  public:
    explicit PixmapItemDelegate(QObject *pParent = nullptr) :
      QStyledItemDelegate(pParent)
    {

    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
      painter->setRenderHint(QPainter::Antialiasing, true);
      painter->fillRect(
        option.rect,
        (option.state & QStyle::State_Selected)?
          option.palette.brush(QPalette::Highlight) :
          option.palette.brush(QPalette::Base)
      );
      painter->drawPixmap(option.rect.topLeft() + QPoint(2, 2), QPixmap(index.data().toString()));
    }

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override
    {
      static QSize pixmapSize = QImageReader(":/images/centered_maxpect").size() + QSize(5, 5);

      return pixmapSize;
    }
  };

  class LabelItemDelegate : public QStyledItemDelegate
  {
  public:
    explicit LabelItemDelegate(QObject *pParent = nullptr) :
      QStyledItemDelegate(pParent)
    {

    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
      QStyleOptionViewItem itemOption(option);
      itemOption.state = itemOption.state & ~QStyle::State_HasFocus;
      QStyledItemDelegate::paint(painter, itemOption, index);
    }
  };
}

using namespace Wally::Ui;

PositionButton::PositionButton(QWidget *pParent) :
  QPushButton(pParent),
  _position(::Wally::Wallpaper::Position::Centered)
{
  setText(positionToString(_position));

  connect(this, &QPushButton::clicked, this,
    [this] ()
      {
        QScopedPointer< QDialog > d(new QDialog(this));
        QDialog *dPtr = d.data();

        Model *model = new Model(dPtr);
        QTableView *view = new QTableView(dPtr);
        QItemSelectionModel *selectionModel = new QItemSelectionModel(model, dPtr);

        dPtr->setWindowTitle(tr("Set position"));

        QVBoxLayout *layout = new QVBoxLayout;
        QDialogButtonBox *dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                               Qt::Horizontal, dPtr);
        connect(dialogButtons, SIGNAL(accepted()), dPtr, SLOT(accept()));
        connect(dialogButtons, SIGNAL(rejected()), dPtr, SLOT(reject()));

        view->setModel(model);
        view->setItemDelegateForColumn(0, new LabelItemDelegate);
        view->setItemDelegateForColumn(1, new PixmapItemDelegate);
        view->setSelectionBehavior(QAbstractItemView::SelectRows);
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        view->setSelectionModel(selectionModel);
        view->verticalHeader()->setVisible(false);
        view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);

        layout->addWidget(view);
        layout->addWidget(dialogButtons);
        dPtr->setLayout(layout);
        dPtr->resize(700, 500);
        connect(dPtr, SIGNAL(executed()), view, SLOT(resizeColumnsToContents()));
        connect(dPtr, SIGNAL(executed()), view, SLOT(resizeRowsToContents()));

        selectionModel->select(model->index(static_cast< int >(position()), 0),
                               QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        view->scrollTo(model->index(static_cast< int >(position()), 0));
        if (dPtr->exec() == QDialog::Accepted)
          setPosition(static_cast< Wally::Wallpaper::Position >(selectionModel->currentIndex().row()));
      }
  );
}

::Wally::Wallpaper::Position PositionButton::position() const
{
  return _position;
}

void PositionButton::setPosition(::Wally::Wallpaper::Position ePosition)
{
  if (ePosition != position())
  {
    _position = ePosition;
    setText(positionToString(ePosition));
    emit positionChanged(ePosition);
  }
}
