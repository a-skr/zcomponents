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

#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "ZTaskImpl.hpp"
#include "Logger.hpp"

using namespace oc2;

void ZTaskCommandCallback::execute (zmq::pollitem_t *, zmq::socket_t * socket)
{
  zmq::message_t zmsg;
  socket->recv (&zmsg);
  ZCommand * command;
  memcpy (&command, zmsg.data (), sizeof (ZCommand *));
  command->execute ();
  command->release ();
}

void ZTaskPeriodicCallback::release (int fd, zmq::socket_t *)
{
  close (fd);
  delete this;
}

void ZTaskPeriodicCallback::execute (zmq::pollitem_t * item, zmq::socket_t *)
{
  unsigned long long missed;
  int rc = read (item->fd, &missed, sizeof (missed));
  if (rc == -1) {
    ERROR ("cannot read fd: %s.", sys_errlist[errno]);
    return;
  }
  
  if (missed != 0)
    DEBUG ("missed %lld timeouts.\n", missed);

  _command->execute ();
}

ZTaskProxyImpl::ZTaskProxyImpl (ZReactor * reactor,
				const char * command_url, 
				zmq::context_t * context)
  : _socket (*context, ZMQ_PUSH),
    _reactor (reactor)
{
  _socket.connect (command_url);
}

void ZTaskProxyImpl::invoke (ZCommand * command)
{
  zmq::message_t zmsg (sizeof (ZCommand *));
  memcpy (zmsg.data (), &command, sizeof (ZCommand *));
  _socket.send (zmsg);
}

int ZTaskProxyImpl::schedule (ZCommand * cmd, uint64_t period, uint64_t delay)
{
  int fd = timerfd_create (CLOCK_MONOTONIC, 0);
  if (fd == -1) 
    ABORT ("%s", sys_errlist[errno]);
  
  struct itimerspec itval;
  itval.it_interval.tv_sec = period / 1000000000;
  itval.it_interval.tv_nsec = period % 1000000000;
  itval.it_value.tv_sec = delay / 1000000000;
  itval.it_value.tv_nsec = delay % 1000000000;

  // it_value set to 0 means "disarm timer", so...
  if (!itval.it_value.tv_sec && !itval.it_value.tv_nsec)
    itval.it_value.tv_nsec = 1;

  int rc = timerfd_settime (fd, 0, &itval, NULL);
  if (rc == -1)
    {
      if (errno != EINVAL)
	ABORT ("schedule: %s.", sys_errlist[errno]);
      WARNING ("schedule: %s.", sys_errlist[errno]);
      close (fd);
      return -1;
    }

  ZTaskPeriodicCallback * callback = new ZTaskPeriodicCallback (cmd); 
  _reactor->register_events (fd, callback);

  return fd;
}

void ZTaskProxyImpl::cancel_schedule (int timerid)
{
  _reactor->unregister_events (timerid);
}

ZTaskImpl::ZTaskImpl (const char * command_url,
		      const char * control_url, 
		      zmq::context_t * context)
  : _self_run (false),
    _command_url (command_url),
    _context (context),
    _command_sock (*_context, ZMQ_PULL),
    _reactor (control_url, context)
{
  _command_sock.bind (_command_url);
  _reactor.register_events (&_command_sock, &_callback);
}

void ZTaskImpl::run()
{
  _self_run = false;
  int rc = pthread_create (&_thread, NULL, run_me, (void *)this);
  if ( rc )
    ABORT ("run: %s.", sys_errlist [rc]);
}

void ZTaskImpl::run_with_current_thread()
{
  _self_run = true;
  svc ();
}

void ZTaskImpl::halt()
{
  _reactor.halt ();
  if ( ! _self_run )
    pthread_join (_thread, NULL);
}

void * ZTaskImpl::run_me (void * me)
{
  ZTaskImpl * myself = (ZTaskImpl *)me;
  myself->svc();
  return 0;
}

void ZTaskImpl::svc ()
{
  _reactor.run_event_loop ();
}

ZProxy * ZTaskImpl::get_proxy ()
{
  return new ZTaskProxyImpl (&_reactor, _command_url, _context);
}

ZReactor * ZTaskImpl::get_reactor ()
{
  return &_reactor;
}
