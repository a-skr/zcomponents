// copyright Alexandre Skrzyniarz 2011-2012 All rights reserved.

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
