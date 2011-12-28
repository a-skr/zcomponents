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

#include <oc2/ZTask.hpp>
#include <stdio.h>

using namespace oc2;

ZTask * task;

class HaltCommand : public ZCommand
{
public:
  virtual void execute () {task->halt ();};
};

int main (int, char **)
{
  zmq::context_t context (1);
  ZTask a_task ("inproc://cmd", "inproc://ctrl", &context);
  task = &a_task;

  ZProxy * proxy = task->get_proxy ();
  proxy->invoke (new HaltCommand ());
  task->run_with_current_thread ();

  delete proxy;

  return 0;
}
