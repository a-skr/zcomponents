// copyright Alexandre Skrzyniarz 2011-2012, All Rights Reserved.

#include <oc2/ZReactor.hpp>
#include <pthread.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <unistd.h>

using namespace oc2;

void * thr_main (void * data)
{
  ZReactor * reactor = (ZReactor *)data;
  reactor->run_event_loop();
  return NULL;
}

ZReactor * preactor;
bool handler_is_deleted = false;
unsigned event_count = 0;
class FDHandler : public ZCallback 
{
public:
  virtual ~FDHandler() { handler_is_deleted = true; };
  virtual void execute (zmq::pollitem_t * item, zmq::socket_t *)
  {
    unsigned long long missed;
    read (item->fd, &missed, sizeof (missed));
    event_count++;
    if (event_count == 5)
      preactor->halt();
  };
};

int main (int, char **)
{
  // init
  zmq::context_t context (1);

  // block to control deletion of objects
  {
    ZReactor reactor ("inproc://reactor-0", &context);
    preactor = &reactor;

    // spawn reactor task
    pthread_t reactor_thread;
    int rc = pthread_create (&reactor_thread, NULL, thr_main, (void*)(&reactor));
    assert (rc == 0);

    // Create the timer 
    int fd = timerfd_create (CLOCK_MONOTONIC, 0);
    assert (fd != -1);

    // register handler
    reactor.register_events (fd, new FDHandler());

    // Make the timer periodic
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 50000000;
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = 1; // if 0, no events.
    timerfd_settime (fd, 0, &itval, NULL);
  
    pthread_join (reactor_thread, NULL);
  } 
  assert (handler_is_deleted);
  assert (event_count = 5);

  return 0;
}
