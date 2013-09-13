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

#ifndef _OC2_LOGGER_HPP_
#define _OC2_LOGGER_HPP_

#include <stdio.h>
#include <stdlib.h>

#define ABORT(fmt,...) { if (oc2::Logger::loglevel >= oc2::Logger::ERROR) fprintf (stderr, "[fatal] (file: %s, line: %d) " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
  abort (); }

#define ERROR(fmt,...) { if (oc2::Logger::loglevel >= oc2::Logger::ERROR) fprintf (stderr, "[error] (file: %s, line: %d) " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); }

#define WARNING(fmt,...) { if (oc2::Logger::loglevel >= oc2::Logger::WARNING) fprintf (stderr, "[warning] (file: %s, line: %d) " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); }

#define INFO(fmt,...) { if (oc2::Logger::loglevel >= oc2::Logger::INFO) fprintf (stderr, "[info] (file: %s, line: %d) " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); }

#define DEBUG(fmt,...) {if (oc2::Logger::loglevel >= oc2::Logger::DEBUG) fprintf (stderr, "[debug] (file: %s, line: %d) " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); }

namespace oc2
{

  class Logger
  {
  public:

    typedef enum {
      SILENT,
      ERROR,
      WARNING,
      INFO,
      DEBUG
    } loglevel_t;

    // called when loading library.
    static void init (); 

    static loglevel_t loglevel;
  };

};

#endif
