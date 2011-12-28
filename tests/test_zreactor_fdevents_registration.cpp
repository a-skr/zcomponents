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
