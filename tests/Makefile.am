MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir) -Wall -Werror @ZEROMQ_CFLAGS@

check_PROGRAMS = test_zreactor_run_halt test_zreactor_sockevents_registration test_zreactor_fdevents_registration test_ztask_run_halt test_ztask_invoke test_ztask_periodic test_ztask_invoke_halt

test_zreactor_run_halt_SOURCES = test_zreactor_run_halt.cpp
test_zreactor_run_halt_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

test_zreactor_sockevents_registration_SOURCES = test_zreactor_sockevents_registration.cpp
test_zreactor_sockevents_registration_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

test_zreactor_fdevents_registration_SOURCES = test_zreactor_fdevents_registration.cpp
test_zreactor_fdevents_registration_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

test_ztask_run_halt_SOURCES = test_ztask_run_halt.cpp
test_ztask_run_halt_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

test_ztask_invoke_SOURCES = test_ztask_invoke.cpp
test_ztask_invoke_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

test_ztask_invoke_halt_SOURCES = test_ztask_invoke_halt.cpp
test_ztask_invoke_halt_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

test_ztask_periodic_SOURCES = test_ztask_periodic.cpp
test_ztask_periodic_LDADD = $(top_builddir)/oc2/libzcomponents.la @ZEROMQ_LIBS@

TESTS = test_zreactor_run_halt test_zreactor_sockevents_registration test_zreactor_fdevents_registration test_ztask_run_halt test_ztask_invoke_halt test_ztask_invoke test_ztask_periodic
