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

#ifndef _OC2_ZTASKIMPL_HPP_
#define _OC2_ZTASKIMPL_HPP_

#include <pthread.h>
#include <zmq.hpp>
#include "ZTask.hpp"
#include "ZReactor.hpp"

namespace oc2 
{

  /**
   * This callback is associated with the ZTaskImpl (invoke command)
   */
  class ZTaskCommandCallback : public ZCallback
  {
  public:
    virtual ~ZTaskCommandCallback () {};
    virtual void execute (zmq::pollitem_t *, zmq::socket_t *);
    virtual void release (int, zmq::socket_t *) {};
  };

  /**
   * each periodic task will use of of these callbacks
   */
  class ZTaskPeriodicCallback : public ZCallback
  {
  public:
    ZTaskPeriodicCallback (ZCommand * command) : _command(command) {};
    virtual ~ZTaskPeriodicCallback () { _command->release(); };
    virtual void execute (zmq::pollitem_t *, zmq::socket_t *);
    virtual void release (int, zmq::socket_t *);
  protected:
    ZCommand * _command;
  };

  /**
   * proxy on task.
   * Allows stacking of commands and scheduler.
   * Based on PUSH/PULL pattern.
   */
  class ZTaskProxyImpl : public ZProxy
  {
  public:
    ZTaskProxyImpl (ZReactor * reactor,
		    const char * command_url, 
		    zmq::context_t * context);
    virtual ~ZTaskProxyImpl () {};

    /**
     * @param command to execute
     * @param period (nanoseconds)
     * @param delay before first execution (nanoseconds)
     */
    int schedule (ZCommand * command, uint64_t period, uint64_t delay);
    void cancel_schedule (int timerid);
    virtual void invoke (ZCommand *);

  protected:
    zmq::socket_t _socket;
    ZReactor * _reactor;
  };

  /**
   * This class is the basic ZTask implementation (publisher / subscriber pattern)
   */
  class ZTaskImpl 
  {
  public:

    /**
     * @param command_url url to stack commands on.
     * @param control_url underlying ZReactor control url
     */
    ZTaskImpl (const char * command_url,
	       const char * control_url,
	       zmq::context_t * context);

    ZProxy * get_proxy ();
    ZReactor * get_reactor ();

    void run();
    void run_with_current_thread();
    void halt();

  protected:

    // execution things.
    void svc ();
    static void * run_me (void *);
    bool _self_run;
    pthread_t _thread;

    // command and control
    const char * _command_url;
    zmq::context_t * _context;
    zmq::socket_t _command_sock;
    ZTaskCommandCallback _callback;
    ZReactor _reactor;
  };


};

#endif
