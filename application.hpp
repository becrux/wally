#ifndef WALLY_APPLICATION_HPP
#define WALLY_APPLICATION_HPP

#include <QApplication>

namespace Wally
{
  class TrayIcon;

  namespace Engines
  {
    class Base;
  }

  class Application : public QApplication
  {
    Q_OBJECT

    TrayIcon *_trayIcon;
    QList< Engines::Base * > _engines;

  public:
    explicit Application(int &argc, char **argv);
  };
} // namespace Wally

#endif // APPLICATION_HPP
