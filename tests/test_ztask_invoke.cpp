// copyright Alexandre Skrzyniarz 2011-2012 All rights reserved.

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
