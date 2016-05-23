# Makefile
ACCESS_TCPIP := TCPIP
ACCESS_NOTA := NOTA
ACCESS_PROTO := $(ACCESS_TCPIP)

APISOURCE = ../SmartSlog_lib/SmartRoomOntology.c
APIHEADER = ../SmartSlog_lib/SmartRoomOntology.h

OBJECT = ../SmartSlog_lib/SmartRoomOntology.o

THREADS := -pthread
#EXPAT_AND_SCEW := -L/usr/lib -L/usr/local/lib -lexpat -lscew -lsmartslog
#INCLUDES := -I/usr/local/include -I/usr/local/include/scew -I/home/vanag/mag/install/KPI_Low/ -I/usr/local/include/smartslog/
LIBS := $(THREADS)  $(EXPAT_AND_SCEW)

CC     = gcc
DEBUG := -g
CFLAGS = -Wall -c $(DEBUG) $(THREADS) -DACCESS_TCPIP -DTHREADS -DEXPAT_AND_SCEW $(INCLUDES)
LFLAGS = -Wall $(DEBUG) $(THREADS)

PROGRAM_NAME = Qserver

targets = main

pkg_packages := scew smartslog ckpi

PKG_CFLAGS  := $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS := $(shell pkg-config --libs $(pkg_packages))

.PHONY: all

all : $(targets)

main: $(OBJECT) Qserver.o
	$(CC) $(LFLAGS) $(PKG_LDFLAGS) Qserver.o $(OBJECT) $(LIBS) -o $(PROGRAM_NAME)

.c.o:
	$(CC) $(CFLAGS) $(PKG_CFLAGS) $(LFLAGS) -o $@ $<


.PHONY: clean
clean:
	rm -f $(targets) *.o
	rm -f $(OBJECT)
	rm -f $(PROGRAM_NAME)
