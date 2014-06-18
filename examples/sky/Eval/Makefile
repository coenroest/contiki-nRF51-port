ifndef TARGET
TARGET=sky
endif

DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI = ../../..
PROJECT_SOURCEFILES += simple-energest.c csma-fast.c nullrdc-fast.c

all: request-response high-throughput

include $(CONTIKI)/Makefile.include
