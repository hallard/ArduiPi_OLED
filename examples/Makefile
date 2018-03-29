#*********************************************************************
# This is the makefile for the Adafruit SSD1306 OLED library driver
#
#	02/18/2013 	Charles-Henri Hallard (http://hallard.me)
#							Modified for compiling and use on Raspberry ArduiPi Board
#							LCD size and connection are now passed as arguments on 
#							the command line (no more #define on compilation needed)
#							ArduiPi project documentation http://hallard.me/arduipi
#
# 08/26/2015    Lorenzo Delana (lorenzo.delana@gmail.com)
#                                                       added bananapi specific CCFLAGS and conditional macro BANANPI
#
# *********************************************************************

# Makefile itself dir
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# hw platform as from autogen.sh choose
HWPLAT:=$(shell cat $(ROOT_DIR)/../hwplatform)

# sets CCFLAGS hw platform dependant
ifeq ($(HWPLAT),BananaPI)
        CCFLAGS=-Wall -Ofast -mfpu=vfpv4 -mfloat-abi=hard -march=armv7 -mtune=cortex-a7 -DBANANAPI
else # fallback to raspberry
        # The recommended compiler flags for the Raspberry Pi
        CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
endif

prefix := /usr/local

# define all programs
PROGRAMS = oled_demo teleinfo-oled
SOURCES = ${PROGRAMS:=.cpp}

CXX=g++
CFLAGS=${CCFLAGS}

all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
	$(CXX) $(CFLAGS) -Wall  -lArduiPi_OLED $@.cpp -o $@

clean:
	rm -rf $(PROGRAMS)

install: all
	test -d $(prefix) || mkdir $(prefix)
	test -d $(prefix)/bin || mkdir $(prefix)/bin
	for prog in $(PROGRAMS); do \
	  install -m 0755 $$prog $(prefix)/bin; \
	done

.PHONY: install


