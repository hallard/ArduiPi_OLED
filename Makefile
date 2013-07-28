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
# *********************************************************************

# Where you want it installed when you do 'make install'
PREFIX=/usr/local

# Library parameters
# where to put the lib
LIBDIR=$(PREFIX)/lib
# lib name 
LIB=libArduiPi_OLED
# shared library name
LIBNAME=$(LIB).so.1.0

# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

# make all
# reinstall the library after each recompilation
all: ArduiPi_OLED install

# Make the library
ArduiPi_OLED: ArduiPi_OLED.o Adafruit_GFX.o bcm2835.o 
	g++ -shared -Wl,-soname,$@.so.1 ${CCFLAGS}  -o ${LIBNAME} $^

# Library parts (use -fno-rtti flag to avoid link problem)
ArduiPi_OLED.o: ArduiPi_OLED.cpp
	g++ -Wall -fPIC -fno-rtti ${CCFLAGS} -c $^

Adafruit_GFX.o: Adafruit_GFX.cpp
	g++ -Wall -fPIC -fno-rtti ${CCFLAGS} -c $^

bcm2835.o: bcm2835.c
	gcc -Wall -fPIC ${CCFLAGS} -c $^

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


