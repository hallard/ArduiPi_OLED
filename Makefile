
#*********************************************************************
# This is the makefile for the ArduiPi OLED library driver
#
#	02/18/2013 	Charles-Henri Hallard (http://hallard.me)
#							Modified for compiling and use on Raspberry ArduiPi Board
#							LCD size and connection are now passed as arguments on 
#							the command line (no more #define on compilation needed)
#							ArduiPi project documentation http://hallard.me/arduipi
# 
# 07/26/2013	Charles-Henri Hallard
#							modified name for generic library using different OLED type
#
# 08/26/2015	Lorenzo Delana (lorenzo.delana@gmail.com)
#							added bananapi specific CCFLAGS and conditional macro BANANPI
#
# *********************************************************************

# Makefile itself dir
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# hw platform as from autogen.sh choose
HWPLAT:=$(shell cat $(ROOT_DIR)/hwplatform)

# sets CCFLAGS hw platform dependant
ifeq ($(HWPLAT),BananaPI)
	CCFLAGS=-Wall -Ofast -mfpu=vfpv4 -mfloat-abi=hard -march=armv7 -mtune=cortex-a7 -DBANANAPI
else # fallback to raspberry
	# The recommended compiler flags for the Raspberry Pi
	CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
endif

# Where you want it installed when you do 'make install'
PREFIX=/usr/local

# Library parameters
# where to put the lib
LIBDIR=$(PREFIX)/lib
# lib name 
LIB=libArduiPi_OLED
# shared library name
LIBNAME=$(LIB).so.1.0

CXX=g++
CC=gcc
CFLAGS=$(CCFLAGS)

# make all
# reinstall the library after each recompilation
all: ArduiPi_OLED install

# Make the library
ArduiPi_OLED: ArduiPi_OLED.o Adafruit_GFX.o bcm2835.o Wrapper.o
	$(CXX) -shared -Wl,-soname,$(LIB).so.1 $(CFLAGS) $(LDFLAGS)  -o ${LIBNAME} $^ -li2c

# Library parts (use -fno-rtti flag to avoid link problem)
ArduiPi_OLED.o: ArduiPi_OLED.cpp
	$(CXX) -Wall -fPIC -fno-rtti $(CFLAGS) -c $^

Adafruit_GFX.o: Adafruit_GFX.cpp
	$(CXX) -Wall -fPIC -fno-rtti $(CFLAGS) -c $^

bcm2835.o: bcm2835.c
	$(CC) -Wall -fPIC $(CFLAGS) -c $^

Wrapper.o: Wrapper.cpp
	$(CC) -Wall -fPIC $(CFLAGS) -c $^

# Install the library to LIBPATH
install: 
	@echo "[Install Library]"
	@if ( test ! -d $(PREFIX)/lib ) ; then mkdir -p $(PREFIX)/lib ; fi
	@install -m 0755 ${LIBNAME} ${LIBDIR}
	@ln -sf ${LIBDIR}/${LIBNAME} ${LIBDIR}/${LIB}.so.1
	@ln -sf ${LIBDIR}/${LIBNAME} ${LIBDIR}/${LIB}.so
	@ldconfig
	@rm -rf ${LIB}.*

	@echo "[Install Headers]"
	@if ( test ! -d $(PREFIX)/include ) ; then mkdir -p $(PREFIX)/include ; fi
	@cp -f  Adafruit_*.h $(PREFIX)/include
	@cp -f  ArduiPi_*.h $(PREFIX)/include
	@cp -f  bcm2835.h $(PREFIX)/include
	
	
# Uninstall the library 
uninstall: 
	@echo "[Uninstall Library]"
	@rm -f ${LIBDIR}/${LIB}.*

	@echo "[Uninstall Headers]"
	@rm -rf  $(PREFIX)/include/ArduiPi_OLED*
	@rm -rf  $(PREFIX)/include/bcm2835.h
	
# clear build files
clean:
	rm -rf *.o ${LIB}.* ${LIBDIR}/${LIB}.*


