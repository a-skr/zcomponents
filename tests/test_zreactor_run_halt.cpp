// copyright Alexandre Skrzyniarz 2011-2012, All Rights Reserved.

#include <pthread.h>
#include <oc2/ZReactor.hpp>
#include <stdio.h>
#include <unistd.h>

using namespace oc2;

void * thr_main (void * data)
{
  ZReactor * reactor = (ZReactor *)data;
  reactor->run_event_loop();
  return NULL;
}

int main (int, char **)
{
  zmq::context_t context (1);
  ZReactor reactor ("inproc://reactor-0", &context);

  pthread_t reactor_thread;
  int rc = pthread_create (&reactor_thread, NULL, thr_main, (void*)(&reactor));
  assert (rc == 0);

  reactor.halt ();
  pthread_join (reactor_thread, NULL);
 
  return 0;
}
