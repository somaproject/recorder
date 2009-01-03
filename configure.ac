#
# This is partly based on
# http://www.openismus.com/documents/linux/building_libraries/building_libraries.shtml
# and 
# Prelude . 
AC_INIT([recorder], [1.0], [soma-bugs@mit.edu])
AC_CONFIG_SRCDIR([src/recorder.cc])


AM_CONFIG_HEADER(config.h)

dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------


#release versioning
GENERIC_MAJOR_VERSION=1
GENERIC_MINOR_VERSION=3
GENERIC_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)


dnl --------------------------------
dnl Package name and version number
dnl --------------------------------


GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4]) # not strictly needed yet, but let's be future-proof


# -----------------------------------------------
# Checks for programs.
# -----------------------------------------------

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# glib for benchmark
PKG_CHECK_MODULES(GTKDEPS, gtkmm-2.4 >= 2.10 glibmm-2.4 >= 2.13.0 dbus-c++-1 >= 0.5.0 log4cpp >= 1.0)
AC_SUBST(GTKDEPS_CFLAGS)
AC_SUBST(GTKDEPS_LIBS)

PKG_CHECK_MODULES(SOMADEPS,   somanetwork-1.0 >= 1.0 )
AC_SUBST(SOMADEPS_CFLAGS)
AC_SUBST(SOMADEPS_LIBS)

# Boost!!! 
AX_BOOST_BASE([1.34.1])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_UNIT_TEST_FRAMEWORK
AX_BOOST_FILESYSTEM
AX_BOOST_THREAD


AM_SANITY_CHECK

AC_LANG_CPLUSPLUS

dnl ------------------------------------------------
dnl This supposedly lets us build code in subdirs into temporary libs,
dnl see http://www.openismus.com/documents/linux/automake/automake.shtml
dnl ------------------------------------------------

dnl ------------------------------------------------
dnl HDF5-related cflags and tools
dnl ------------------------------------------------

HDF5_LIBS="-L/usr/local/lib/ -lhdf5_cpp -lhdf5  -lhdf5_hl"
HDF5_CFLAGS="-I/usr/local/include -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG "

AC_SUBST(HDF5_LIBS)
AC_SUBST(HDF5_CFLAGS)


dnl -----------------------------------------------
dnl Generates Makefile's, configuration files and scripts
dnl -----------------------------------------------

AC_CONFIG_FILES([Makefile src/Makefile unittest/Makefile ])

AC_OUTPUT
