#include "ui/colorbutton.hpp"

#include <QApplication>
#include <QColorDialog>
#include <QStyleOptionButton>
#include <QStylePainter>

using namespace Wally::Ui;

ColorButton::ColorButton(QWidget *pParent) :
  QPushButton(pParent)
{
  // for sizeHint only
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

//QSize ColorButton::sizeHint() const
//{
//  return QSize(80, 20);
//}

void ColorButton::paintEvent(QPaintEvent * /*event*/)
{
  // QPushButton::paintEvent(event);

  QStylePainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  QStyleOptionButton option;

  option.initFrom(this);
  option.text.clear();

  painter.drawControl(QStyle::CE_PushButton, option);

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
