// copyright Alexandre Skrzyniarz 2011-2012 All rights reserved.

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
