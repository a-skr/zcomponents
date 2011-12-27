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

#ifndef _OC2_ZREACTOR_HPP_
#define _OC2_ZREACTOR_HPP_

#include <zmq.hpp>
#include <map>
#include <poll.h>

namespace oc2
{

  /**
   * extend this class at will.
   */
  class ZCallback
  {
  public:
    virtual ~ZCallback() {};
    virtual void execute (zmq::pollitem_t *, zmq::socket_t *) =0;
    virtual void release(int /* fd */, zmq::socket_t *) {delete this;};
  };

  /**
   * This class is a simple reactor that allows demultiplexing of events.
   * Handled events include timers (via timerfd API) and zmq socket events.
   * It is the "private implementation" of the ZReactor class.
   */
  class ZReactor
  {
  public:
    
    /**
     * @brief Constructor
     * @param control_url url of the zmq socket used for reactor control.
     *        an inproc socket should be used.
     */
    ZReactor (const char * control_url, zmq::context_t * context);

    /**
     * @brief Destructor
     */
    ~ZReactor ();

    /**
     * the current thread will run the event loop (poll file descriptors).
     */
    void run_event_loop ();
    
    /**
     * Those methods will queue up an event on the admin queue. events on the 
     * admin queue will be processed before all other events.
     */
    void halt ();
    void register_events (int fd, 
			  ZCallback * cb, 
			  int mask = POLLIN);
    void register_events (zmq::socket_t * sock, 
			  ZCallback * cb, 
			  int mask = ZMQ_POLLIN);
    void unregister_events (int fd);
    void unregister_events (zmq::socket_t * sock);

  private:    
    void * _private;
  };

}

#endif
