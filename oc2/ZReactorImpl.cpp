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
#include "Logger.hpp"
#include "ZReactorImpl.hpp"

using namespace oc2;

ZReactorImpl::ZReactorImpl (const char * control_url, zmq::context_t * context)
  : _shall_continue (false),
    _control_url (control_url), 
    _context (context),
    _admin_socket (*_context, ZMQ_SUB),
    _poll_items (0),
    _poll_length (0)
{
  _admin_socket.setsockopt (ZMQ_SUBSCRIBE, "", 0);
  _admin_socket.bind (_control_url);
}

void ZReactorImpl::run_event_loop()
{
  // build pollitem list
  build_pollitems ();

  _shall_continue = true;
  while (_shall_continue)
    {
      // poll events
      int rc = zmq_poll (_poll_items, _poll_length, -1);
      if (rc ==-1) {
	if (errno == EINTR || errno == EAGAIN)
	  {
	    DEBUG ("poll: %s.", sys_errlist[errno]);
	    continue;
	  }
	else
	  ABORT ("poll: %s.", sys_errlist[errno]);
      }
      
      // handle admin socket first (always the first item in poll items).
      if (_poll_items[0].revents == ZMQ_POLLIN)
	{
	  zmq::message_t zmsg;
	  _admin_socket.recv (&zmsg);
	  struct ZControlMessage * cmsg = (struct ZControlMessage *)zmsg.data();
	  admin_callback (cmsg);
	  continue;
	}

      // handle other events in registration order.
      for (unsigned idx = 1 ; idx < _poll_length ; ++idx)
	trigger_callback (_poll_items[idx]);
    }
}

void ZReactorImpl::halt()
{
  zmq::socket_t sock (*_context, ZMQ_PUB);
  sock.connect (_control_url);

  struct ZControlMessage message;
  message.command = ZREACTOR_HALT;

  zmq::message_t zmsg (sizeof (struct ZControlMessage));
  memcpy (zmsg.data(), &message, sizeof (struct ZControlMessage));
  sock.send (zmsg);
}

void ZReactorImpl::admin_callback (struct ZControlMessage * cmsg)
{
  switch (cmsg->command)
    {
    case ZREACTOR_HALT:
      _shall_continue = false;
      break;
    
    case ZREACTOR_ADD_REGISTRATION:
      add_event (cmsg->registration);
      build_pollitems();
      break;

    case ZREACTOR_REMOVE_REGISTRATION:
      remove_event (cmsg->registration);
      build_pollitems();
      break;
    }
}

void ZReactorImpl::register_events (int fd, ZCallback * cb, int mask)
{
  zmq::socket_t asock (*_context, ZMQ_PUB);
  asock.connect (_control_url);

  struct ZControlMessage message;
  message.command = ZREACTOR_ADD_REGISTRATION;
  message.registration.fd = fd;
  message.registration.socket = 0;
  message.registration.mask = mask;
  message.registration.callback = cb;

  zmq::message_t zmsg (sizeof (struct ZControlMessage));
  memcpy (zmsg.data(), &message, sizeof (struct ZControlMessage));
  asock.send (zmsg);
}

void ZReactorImpl::register_events(zmq::socket_t * sock, ZCallback * cb, int mask)
{
  zmq::socket_t asock (*_context, ZMQ_PUB);
  asock.connect (_control_url);
  
  struct ZControlMessage message;
  message.command = ZREACTOR_ADD_REGISTRATION;
  message.registration.fd = -1;
  message.registration.socket = sock;
  message.registration.mask = mask;
  message.registration.callback = cb;

  zmq::message_t zmsg (sizeof (struct ZControlMessage));
  memcpy (zmsg.data(), &message, sizeof (struct ZControlMessage));
  asock.send (zmsg);
}

void ZReactorImpl::unregister_events(int fd)
{
  zmq::socket_t asock (*_context, ZMQ_PUB);
  asock.connect (_control_url);

  struct ZControlMessage message;
  message.command = ZREACTOR_REMOVE_REGISTRATION;
  message.registration.fd = fd;
  message.registration.socket = 0;
  message.registration.mask = 0;
  message.registration.callback = 0;

  zmq::message_t zmsg (sizeof (struct ZControlMessage));
  memcpy (zmsg.data(), &message, sizeof (struct ZControlMessage));
  asock.send (zmsg);
}

void ZReactorImpl::unregister_events(zmq::socket_t * sock)
{
  zmq::socket_t asock (*_context, ZMQ_PUB);
  asock.connect (_control_url);

  struct ZControlMessage message;
  message.command = ZREACTOR_REMOVE_REGISTRATION;
  message.registration.fd = -1;
  message.registration.socket = sock;
  message.registration.mask = 0;
  message.registration.callback = 0;

  zmq::message_t zmsg (sizeof (struct ZControlMessage));
  memcpy (zmsg.data(), &message, sizeof (struct ZControlMessage));
  asock.send (zmsg);
}

void ZReactorImpl::build_pollitems ()
{
  if (_poll_items)
    delete [] _poll_items;

  _poll_length = 1 + _registrations.size();
  _poll_items = new zmq_pollitem_t [_poll_length];
  
  // admin
  _poll_items[0].socket = _admin_socket;
  _poll_items[0].fd = -1;
  _poll_items[0].events = ZMQ_POLLIN;
  _poll_items[0].revents = 0;

  // other registrations
  unsigned idx = 1;
  for ( std::map<struct ZERKey,ZEREntry>::iterator it = _registrations.begin() ;
	it != _registrations.end() ;
	it++ )
    {
      _poll_items[idx].socket = it->first.socket;
      _poll_items[idx].fd     = it->first.fd;
      _poll_items[idx].events = it->second.mask;
      _poll_items[idx].revents = 0;
      idx++;
    }
}

void ZReactorImpl::add_event(struct ZEventRegistration & reg)
{
  ZERKey key;
  key.fd = reg.fd;
  if (reg.socket)
    key.socket = *(reg.socket);
  else 
    key.socket = 0;

  ZEREntry & entry = _registrations [key];
  entry.socket   = reg.socket;
  entry.callback = reg.callback;
  entry.fd       = reg.fd;
  entry.mask     = reg.mask;
}

void ZReactorImpl::remove_event(struct ZEventRegistration & reg)
{
  ZERKey key;
  key.fd = reg.fd;
  if (reg.socket)
    key.socket = *(reg.socket);
  else
    key.socket = 0;
  _registrations.erase (key);
}

void ZReactorImpl::trigger_callback (zmq_pollitem_t & item)
{
  // exit if no event.
  if ( ! item.revents )
    return;

  // look up callback
  ZERKey key;
  key.fd = item.fd;
  key.socket = item.socket;
  ZCallback * callback = _registrations[key].callback; 
  
  // trigger callback
  if ( callback )
    callback->execute(&item, _registrations[key].socket);
}
