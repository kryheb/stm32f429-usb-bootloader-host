ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = bootloaderhost

bootloaderhost_SOURCES = \
src/main.cpp \
src/bootloader/usbhid.cpp \
src/bootloader/bootloader.cpp \
src/bootloader/error.cpp \
src/bootloader/ihexparser.cpp \
src/utils/log/log.cpp

bootloaderhost_CXXFLAGS = \
-I /usr/include/hidapi \
-I src/bootloader \
-I src \
-DBOOST_LOG_DYN_LINK \
-std=c++17

bootloaderhost_LDADD = \
$(PTHREAD_CFLAGS) \
-lboost_log \
-lboost_log_setup \
-lboost_system \
-lboost_thread \
-lboost_date_time
