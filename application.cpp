#include "application.hpp"
#include "trayicon.hpp"

#include "engines/flickr.hpp"

#include <QSystemTrayIcon>

using namespace Wally;

Application::Application(int &argc, char **argv) :
  QApplication(argc, argv),
  _trayIcon(new TrayIcon(this))
{
  _engines << new Engines::Flickr::Engine(this);

  QMetaObject::invokeMethod(_engines.at(0), "selectNext", Qt::QueuedConnection);
}
