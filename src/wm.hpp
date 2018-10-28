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

#pragma once

#include "application.hpp"

#include <QString>

namespace Wally
{
  namespace WindowManagers
  {
    class IWindowManager
    {
    public:
      virtual ~IWindowManager() = default;

      virtual void showPhoto(const QString &sFileName) = 0;

      virtual FileFormats requestedFormat() const = 0;
    };
  } // namespace WindowManagers
} // namespace Wally
