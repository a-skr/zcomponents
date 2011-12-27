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

#ifndef _OC2_ZTASK_HPP_
#define _OC2_ZTASK_HPP_

#include <stdint.h>
#include <zmq.hpp>
#include <oc2/ZReactor.hpp>

// convenience macros to express durations
#define NSEC(X) (uint64_t(X))
#define USEC(X) (uint64_t(X))*1000
#define MSEC(X) (uint64_t(X))*1000000
#define SEC(X)  (uint64_t(X))*1000000000

namespace oc2 
{
  
  class ZCommand 
  {
  public:
    virtual ~ZCommand () {};
    virtual void execute () =0;
    virtual void release () {delete this;};
  };

  class ZProxy
  {
  public:
    virtual ~ZProxy () {};
    virtual void invoke (ZCommand *) =0;
    virtual int schedule (ZCommand *, uint64_t, uint64_t =0) =0;
    virtual void cancel_schedule (int timerid) =0;
  };

  class ZTask 
  {
  public:

    /**
     * Constructor.
     * @param command_url is the url to push commands to.
     * @param control_url is the url to control the underlying reactor.
     */
    ZTask (const char * command_url,
	   const char * control_url, 
	   zmq::context_t * context);
    ~ZTask ();

    /**
     * returns new proxy on this task.
     * you will have to delete it yourself.
     */
    ZProxy * get_proxy ();

    /**
     * Returns a pointer to the underlying reactor.
     */
    ZReactor * get_reactor ();

    void run ();
    void run_with_current_thread ();
    void halt ();

  private:
    void * _private;
  };

}

#endif
