/*
 * Copyright (c) 2011 Alexandre Skrzyniarz.
 *
 * This file is part of oc2/zcomponents.
 *
 * oc2/zcomponents is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * oc2/zcomponents is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with oc2/zcomponents.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <zmq.hpp>
#include <oc2/ZTask.hpp>

// command that increments a simple counter
class MyCommand : public oc2::ZCommand
{
public:
  MyCommand () {};
  virtual ~MyCommand () {};
  virtual void execute () { ++_count; };
  virtual void release () {}; // reuse command.
  unsigned get_count () {return _count;};
protected:
  static unsigned _count;
};
unsigned MyCommand::_count = 0;

// a simple example of invocation with command reuse.
int main (int, char **)
{
  zmq::context_t context (1);
  oc2::ZTask task ("inproc://command", "inproc://control", &context);

  task.run ();
  oc2::ZProxy * proxy = task.get_proxy ();

  MyCommand cmd;
  for (unsigned i = 0 ; i < 5 ; ++i)
    proxy->invoke (&cmd);

  // sleep, then check _count
  usleep (5000); // 5 milliseconds. Should be plenty.
  assert (cmd.get_count () == 5);

  task.halt();

  delete (proxy);
  return 0;
}
