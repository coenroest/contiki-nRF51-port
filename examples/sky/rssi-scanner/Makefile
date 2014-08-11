all: rssi-scanner

%.class: %.java
	javac $(basename $<).java

viewrssi: ViewRSSI.class
	$(MAKE) login | java ViewRSSI

viewrssi3d: ViewRSSI3D.class
	$(MAKE) login | java ViewRSSI3D

upload:	rssi-scanner.ihex
	$(MAKE) upload-ihex FILE=rssi-scanner.ihex

ifndef TARGET
TARGET=sky
endif

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
