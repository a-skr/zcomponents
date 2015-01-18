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

#ifndef _OC2_ZREACTORIMPL_HPP_
#define _OC2_ZREACTORIMPL_HPP_

#include <zmq.hpp>
#include <map>
#include <poll.h>
#include "ZReactor.hpp"

namespace oc2
{
  /**
   * This class is a simple reactor that allows demultiplexing of events.
   * Handled events include timers (via timerfd API) and zmq socket events.
   * It is the "private implementation" of the ZReactor class.
   */
  class ZReactorImpl
  {
  public:
    
    /**
     * @brief Constructor
     * @param control_url url of the zmq socket used for reactor control.
     *        an inproc socket should be used.
     */
    ZReactorImpl (const char * control_url, zmq::context_t * context);

    /**
     * @brief Destructor
     */
    ~ZReactorImpl();

    /**
     * the current thread will run the event loop (poll file descriptors).
     */
    void run_event_loop(int timeout_ms);
    
    /**
     * Those methods shall be used only when the reactor is running.
     * The will queue up an event on the admin queue. events on the 
     * admin queue will be processed before all other events.
     */
    void halt();
    void register_events(int fd, 
			 ZCallback * cb, 
			 int mask = POLLIN);
    void register_events(zmq::socket_t * sock, 
			 ZCallback * cb, 
			 int mask = ZMQ_POLLIN);
    void unregister_events(int fd);
    void unregister_events(zmq::socket_t * sock);

    //protected:
    
    // types for reactor control.
    enum ZReactorCommand
      {
	ZREACTOR_HALT,   
	ZREACTOR_ADD_REGISTRATION,
	ZREACTOR_REMOVE_REGISTRATION
      };

    struct ZEventRegistration
    {
      int fd;
      zmq::socket_t * socket;
      int mask;
      ZCallback * callback;
    };

    struct ZControlMessage
    {
      ZReactorCommand command;
      struct ZEventRegistration registration;
    };

    // ZEventRegistration Key
    class ZERKey 
    {
    public:
      bool operator < (const ZERKey & right) const { 
	if ( fd == right.fd )
	  return socket < right.socket;
	return fd < right.fd;
      };
      int fd;
      void * socket;
    };

    // ZEventRegistration Entry
    class ZEREntry
    {
    public:
      ZEREntry () : callback (0), socket (0), fd(-1), mask(0) {};
      ~ZEREntry() { if ( callback ) callback->release(fd, socket); };

      ZCallback * callback;
      zmq::socket_t * socket;
      int fd;
      int mask;
    };

    bool _shall_continue;
    const char * _control_url;
    zmq::context_t * _context;
    zmq::socket_t _admin_socket;
    zmq_pollitem_t * _poll_items;
    unsigned _poll_length;
    std::map<ZERKey,ZEREntry> _registrations;

    void admin_callback (struct ZControlMessage * cmsg);
    void build_pollitems ();
    void add_event(struct ZEventRegistration & reg);
    void remove_event(struct ZEventRegistration & reg);
    void trigger_callback (zmq_pollitem_t & item);
  };

}

#endif
