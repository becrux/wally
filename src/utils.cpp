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

#include "src/utils.hpp"

using namespace Wally;

#ifdef LINUX
#include <QDirIterator>

bool Utils::isProcessRunning(const QString &sProgramName)
{
  QDirIterator di("/proc", QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

  while (di.hasNext())
  {
    QString path = di.next();

    bool ok;
    path.toInt(&ok);
    if (!ok)
      continue;

    if (QFileInfo(path).symLinkTarget().contains(sProgramName))
      return true;
  }

  return false;
}
#endif

#ifdef MACOSX
#include <stdlib.h>
#include <sys/sysctl.h>

template< typename T >
class Pointer
{
  void *_ptr;

public:
  explicit Pointer(size_t length) :
    _ptr(malloc(length)) { }
  Pointer(const Pointer &) = delete;
  Pointer(Pointer &&o) :
    _ptr(o._ptr)
  {
    o._ptr = NULL;
  }

  ~Pointer()
  {
    if (_ptr)
      free(_ptr);
  }

  T *get() const
  {
    return reinterpret_cast< T * >(_ptr);
  }

  operator bool() const
  {
    return _ptr != NULL;
  }

  Pointer &operator=(const Pointer &) = delete;
  Pointer &operator=(Pointer &&o)
  {
    _ptr = o._ptr;
    o._ptr = NULL;

    return *this;
  }
};

bool Utils::isProcessRunning(const QString &sProgramName)
{
  static int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

  size_t length = 0;
  if (sysctl(name, 3, NULL, &length, NULL, 0) == -1)
    return false;

  Pointer< kinfo_proc > result(length);

  if (!result)
    return false;

  if (sysctl(name, 3, result.get(), &length, NULL, 0) == -1)
    return false;

  size_t numItems = length / sizeof(kinfo_proc);

  kinfo_proc *ptr = result.get();
  for (size_t i = 0; i < numItems; ++i, ++ptr)
    if (!sProgramName.compare(QString::fromUtf8(ptr->kp_proc.p_comm)))
      return true;

  return false;
}
#endif
