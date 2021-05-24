# Process this file with autoconf to produce a configure script.
# Written by Salonia Matteo on 16/05/2021

AC_PREREQ([2.69])

AC_INIT([quiz], [1.3], [saloniamatteo@pm.me])
AC_CONFIG_SRCDIR([src])

# Checks for programs.
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h strings.h signal.h unistd.h getopt.h time.h sys/types.h pwd.h err.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_CHECK_FUNCS(secure_getenv)

# Custom parameter
AC_ARG_ENABLE([cpuopts],
	[AS_HELP_STRING([--enable-cpuopts=yes|no], [Enable or Disable -march=native and -mtune=native (Default: enabled)])],
	[enable_cpuopts="$enableval"],
	[enable_cpuopts="yes"])

# Check if "yes" has been passed
if test "x$enable_cpuopts" = "xno"; then
	AC_SUBST([CPUOPTS], [""])
else
	AC_SUBST([CPUOPTS], ["-march=native -mtune=native"])
fi

# Package info
AC_SUBST([AUTHOR], ["Matteo Salonia"])
AC_SUBST([PACKAGE_URL], ["https://github.com/saloniamatteo/quiz"])
AC_SUBST([PACKAGE_DONATE], ["https://saloniamatteo.top/donate.html"])

# Automake
AM_INIT_AUTOMAKE([gnu subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_PROG_CC

AC_OUTPUT
