ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = bootloaderhost

bootloaderhost_SOURCES = \
src/main.cpp \
src/usbhid.cpp \
src/bootloader.cpp \
src/error.cpp \
src/ihexparser.cpp

bootloaderhost_CXXFLAGS = \
-I /usr/include/hidapi \
-I src

bootloaderhost_LDADD = $(PTHREAD_CFLAGS)