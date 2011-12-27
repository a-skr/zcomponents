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

#include "Logger.hpp"
#include <string.h>

using namespace oc2;

Logger::loglevel_t Logger::loglevel = Logger::WARNING;

void Logger::init ()
{
  char * loglevel = getenv ("OC2_LOGLEVEL");
  
  if (loglevel)
    {
      if (! strcmp ("silent", loglevel))
	Logger::loglevel = Logger::SILENT;
      else if (! strcmp ("error", loglevel))
	Logger::loglevel = Logger::ERROR;
      else if (! strcmp ("warning", loglevel))
	Logger::loglevel = Logger::WARNING;
      else if (! strcmp ("info", loglevel))
	Logger::loglevel = Logger::INFO;
      else if (! strcmp ("debug", loglevel))
	Logger::loglevel = Logger::DEBUG;
    }
}
