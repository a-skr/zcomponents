MAINTAINERCLEANFILES = Makefile.in

# to build
lib_LTLIBRARIES = libzcomponents.la
oc2include_HEADERS = ZReactor.hpp ZTask.hpp

# libraries
libzcomponents_la_SOURCES = init.cpp Logger.hpp Logger.cpp ZReactor.hpp ZReactor.cpp ZReactorImpl.hpp ZReactorImpl.cpp ZTaskImpl.hpp ZTaskImpl.cpp ZTask.hpp ZTask.cpp
libzcomponents_la_LIBADD = @ZEROMQ_LIBS@
libzcomponents_la_CPPFLAGS = -I$(top_srcdir) -Wall -Werror @ZEROMQ_CFLAGS@