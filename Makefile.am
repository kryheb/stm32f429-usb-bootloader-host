ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = bootloaderhost

bootloaderhost_SOURCES = \
src/main.cpp \
src/usbhid.cpp

bootloaderhost_CPPFLAGS = \
-I /usr/include/hidapi \
-I src

