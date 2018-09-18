#ifndef WALLY_ENGINE_HPP
#define WALLY_ENGINE_HPP

#include <QFileInfo>
#include <QObject>
#include <QWidget>

namespace Wally
{
  namespace Engines
  {
    class SettingsWidget : public QWidget
    {
      Q_OBJECT

    public:
      explicit SettingsWidget(QWidget *parent = nullptr);

    public slots:
      virtual void accept() = 0;
    };

    class Base : public QObject
    {
      Q_OBJECT

    public:
      explicit Base(QObject *parent = nullptr);

      virtual SettingsWidget *settingsWidget(QWidget *parent) = 0;

    signals:
      void pictureAvailable(const QFileInfo &pictureFileName);
      void failed();

    public slots:
      virtual void selectNext() = 0;
    };
  } // namespace Engines
} // namespace Wally

#endif // WALLY_ENGINE_HPP
