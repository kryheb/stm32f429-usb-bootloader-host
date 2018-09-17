#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([bootloaderhost], [0.0.0], [krystianheberlein@gmail.com])
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE([1.10 no-define foreign subdir-objects])
AC_CONFIG_HEADERS([config.h])

AX_CXX_COMPILE_STDCXX_14(, [mandatory])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

AC_CHECK_LIB(hidapi-libusb, hid_enumerate, [], [AC_MSG_ERROR([Could not find hidapi lib, install libhidapi-dev])])


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
