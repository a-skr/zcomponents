// copyright Alexandre Skrzyniarz 2011-2012 All rights reserved.

#include <zmq.hpp>
#include <oc2/ZTask.hpp>

int main (int, char **)
{
  zmq::context_t context (1);
  oc2::ZTask task ("inproc://any", "inproc://control-task-0", &context);

  task.run();
  task.halt();

  return 0;
}
