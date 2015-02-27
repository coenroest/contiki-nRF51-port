TARGET = PCA10000-nrf

CONTIKI_PROJECT = BLE-Exprmt

PROJECTDIRS += ./Exprmt
PROJECT_SOURCEFILES += Pillar-func.c ble_lbs.c

BIN_FILE = BLE-Exprmt.bin

binUp: obj-bin upload
all: $(CONTIKI_PROJECT) 

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
