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

#include <assert.h>
#include <oc2/ZTask.hpp>
#include <stdio.h>
#include <unistd.h>

using namespace oc2;

int fd;
ZTask * task;

class HaltMe : public ZCommand
{
public:
  virtual void execute () {
    task->halt();
  };
};

class PCommand : public ZCommand
{
public:
  virtual void execute () {
    ++count;
    if (count == 5) {
      ZProxy * proxy = task->get_proxy();
      proxy->cancel_schedule (fd);
      proxy->invoke (new HaltMe);
      delete proxy;
    }
  };
  static unsigned count;
};
unsigned PCommand::count = 0;

int main (int, char **)
{
  zmq::context_t context (1);
  ZTask a_task ("inproc://cmd", "inproc://ctrl", &context);
  task = &a_task;

  PCommand * pc = new PCommand;
  ZProxy * proxy = a_task.get_proxy();
  fd = proxy->schedule (pc, MSEC (60));

  a_task.run_with_current_thread();

  assert (PCommand::count == 5);
  delete proxy;
  
  return 0;
}
