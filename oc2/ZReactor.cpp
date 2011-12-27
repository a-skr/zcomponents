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

#include <stdio.h>
#include "ZReactorImpl.hpp"
#define PIMPL ((ZReactorImpl *)_private)

using namespace oc2;

ZReactor::ZReactor (const char * control_url, zmq::context_t * context)
  : _private (new ZReactorImpl (control_url, context))
{
}

ZReactor::~ZReactor ()
{
  delete PIMPL;
}

void ZReactor::run_event_loop()
{
  PIMPL -> run_event_loop ();
}

void ZReactor::halt()
{
  PIMPL -> halt ();
}

void ZReactor::register_events (int fd, ZCallback * cb, int mask)
{
  PIMPL -> register_events (fd, cb, mask);
}

void ZReactor::register_events(zmq::socket_t * sock, ZCallback * cb, int mask)
{
  PIMPL -> register_events (sock, cb, mask);
}

void ZReactor::unregister_events(int fd)
{
  PIMPL -> unregister_events (fd);
}

void ZReactor::unregister_events(zmq::socket_t * sock)
{
  PIMPL -> unregister_events (sock);
}

