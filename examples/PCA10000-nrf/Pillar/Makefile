TARGET = PCA10000-nrf

CONTIKI_PROJECT = BLE-Pillar

PROJECTDIRS += ./Pillar
PROJECT_SOURCEFILES += Pillar-func.c ble_lbs.c

BIN_FILE = BLE-Pillar.bin

binUp: obj-bin upload
all: $(CONTIKI_PROJECT) 

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
