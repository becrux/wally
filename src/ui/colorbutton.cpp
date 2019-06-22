#include "ui/colorbutton.hpp"

#include <QApplication>
#include <QColorDialog>
#include <QPainter>
#include <QStyleOptionButton>

using namespace Wally::Ui;

ColorButton::ColorButton(QWidget *pParent) :
  QPushButton(pParent)
{
  // Just to calculate sizeHint
  setText("Caption");
}

void ColorButton::setColor(const QColor &cColor)
{
  if (color() != cColor)
  {
    _color = cColor;
    update();
    emit colorChanged(_color);
  }
}

QColor ColorButton::color() const
{
  return _color;
}

void ColorButton::paintEvent(QPaintEvent *event)
{
  QPushButton::paintEvent(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  QStyleOptionButton option;

  option.initFrom(this);
  option.text.clear();

  QRect rect = QApplication::style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);

  if (_color.isValid())
    painter.fillRect(rect.adjusted(rect.width() / 8, rect.height() / 3, -rect.width() / 8, -rect.height() / 3), color());
  else
    painter.drawText(rect, Qt::AlignCenter, tr("Auto"));
}

void ColorButton::nextCheckState()
{
  QPushButton::nextCheckState();

  if (color().isValid())
    setColor(QColor());
  else
  {
    QColor newColor = QColorDialog::getColor(color().rgb());
    if (newColor.isValid())
      setColor(newColor);
  }
}
