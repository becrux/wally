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
