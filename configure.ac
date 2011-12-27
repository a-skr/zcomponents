# Prelude
AC_PREREQ(2.68)
AC_INIT(zcomponents, 0.0.1, alexandre.skrzyniarz@laposte.net)
AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(-Wall -Werror 1.9 dist-bzip2)

# libtool init
LT_INIT

# safety check, to make sure configure is not run from outher space...
AC_CONFIG_SRCDIR(oc2/ZReactor.hpp)

# Check for programs
AC_PROG_CXX

# check for libraries
PKG_CHECK_MODULES([ZEROMQ], libzmq)

# custom path to install oc2 headers in .../include/oc2
AC_SUBST(oc2includedir, [${includedir}/oc2])

# headers
AC_HEADER_STDBOOL
AM_CONFIG_HEADER(config.h)

# output files
AC_CONFIG_FILES(Makefile oc2/Makefile tests/Makefile)
AC_OUTPUT