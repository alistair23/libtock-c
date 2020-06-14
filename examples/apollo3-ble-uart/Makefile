# Makefile for user application

# Specify this directory relative to the current application.
TOCK_USERLAND_BASE_DIR = ../..

# Which files to compile.
C_SRCS := $(wildcard *.c)

# External libraries used
EXTERN_LIBS += exactle/wsf
EXTERN_LIBS += exactle/ble-host

CPPFLAGS += -Iexactle/wsf/sources/util/
CPPFLAGS += -Iexactle/wsf/sources/


# Include userland master makefile. Contains rules and flags for actually
# building the application.
include $(TOCK_USERLAND_BASE_DIR)/AppMakefile.mk
