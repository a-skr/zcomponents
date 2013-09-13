# Prelude
AC_PREREQ(2.68)
AC_INIT(zcomponents, 0.0.2, alexandre.skrzyniarz@laposte.net)
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
AC_CHECK_LIB([pthread], [pthread_create])

# custom path to install files
AC_SUBST(oc2includedir, [${includedir}/oc2])
AC_SUBST(pkgconfigdir, [${libdir}/pkgconfig])

# headers
AC_HEADER_STDBOOL
AC_TYPE_UINT64_T
AM_CONFIG_HEADER(config.h)

# output files
AC_CONFIG_FILES(Makefile oc2/Makefile oc2/libzcomponents.pc tests/Makefile)
AC_OUTPUT
