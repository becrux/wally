#ifndef WALLY_TRAYICON_HPP
#define WALLY_TRAYICON_HPP

#include <QSystemTrayIcon>

namespace Wally
{
  class TrayIcon : public QSystemTrayIcon
  {
  public:
    explicit TrayIcon(QObject *parent = nullptr);
  };
} // namespace Wally

#endif // WALLY_TRAYICON_HPP
