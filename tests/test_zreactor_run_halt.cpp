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
