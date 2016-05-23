# Makefile
ACCESS_TCPIP := TCPIP
ACCESS_NOTA := NOTA
ACCESS_PROTO := $(ACCESS_TCPIP)

APISOURCE = ../SmartSlog_lib/SmartRoomOntology.c
APIHEADER = ../SmartSlog_lib/SmartRoomOntology.h

INCLUDES = -I$(CURDIR)/SmartSlog -I$(CURDIR)/includes/expat \
-I$(CURDIR)/includes/scew -I$(CURDIR)/Ontology \
-I$(CURDIR)/includes

SRCS =  ./SmartSlog/classes.c \
	./SmartSlog/kpi_interface.c \
	./SmartSlog/patterns.c \
	./SmartSlog/properties.c \
	./SmartSlog/property_changes.c \
	./SmartSlog/ss_classes.c \
	./SmartSlog/ss_func.c \
	./SmartSlog/ss_multisib.c\
	./SmartSlog/ss_populate.c \
	./SmartSlog/ss_properties.c \
	./SmartSlog/structures.c \
	./SmartSlog/subscription.c \
	./SmartSlog/subscription_changes.c \
	./SmartSlog/utils/check_func.c \
	./SmartSlog/utils/kp_error.c \
	./SmartSlog/utils/list.c \
	./SmartSlog/utils/util_func.c \
	./SmartSlog/expat/xmlparse.c \
	./SmartSlog/expat/xmlrole.c \
	./SmartSlog/expat/xmltok.c \
	./SmartSlog/expat/xmltok_impl.c \
	./SmartSlog/expat/xmltok_ns.c \
	./SmartSlog/scew/attribute.c \
	./SmartSlog/scew/element_attribute.c \
	./SmartSlog/scew/element.c \
	./SmartSlog/scew/element_compare.c \
	./SmartSlog/scew/element_copy.c \
	./SmartSlog/scew/element_search.c \
	./SmartSlog/scew/error.c \
	./SmartSlog/scew/list.c \
	./SmartSlog/scew/parser.c \
	./SmartSlog/scew/printer.c \
	./SmartSlog/scew/reader_buffer.c \
	./SmartSlog/scew/reader.c \
	./SmartSlog/scew/reader_file.c \
	./SmartSlog/scew/str.c \
	./SmartSlog/scew/tree.c \
	./SmartSlog/scew/writer_buffer.c \
	./SmartSlog/scew/writer.c \
	./SmartSlog/scew/writer_file.c \
	./SmartSlog/scew/xattribute.c \
	./SmartSlog/scew/xerror.c \
	./SmartSlog/scew/xparser.c \
	./SmartSlog/ckpi/ckpi.c \
	./SmartSlog/ckpi/compose_ssap_msg.c \
	./SmartSlog/ckpi/parse_ssap_msg.c \
	./SmartSlog/ckpi/process_ssap_cnf.c \
	./SmartSlog/ckpi/sib_access_tcp.c \
	./SmartSlog/ckpi/sskp_errno.c \
	./Ontology/SmartRoomOntology.c \
	./src/kp.c \
	./queueserver/phone.c 

OBJS = $(SRCS:.c=.o)
TARGET = MicServer
LIB_SSLOG = libsslog
OBJECT = ../SmartSlog_lib/SmartRoomOntology.o

THREADS := -pthread

LIBS := $(THREADS)  $(EXPAT_AND_SCEW)

CC     = gcc
DEBUG := -g
CFLAGS = -Wall -c $(DEBUG) $(THREADS) -DACCESS_TCPIP -DTHREADS -DEXPAT_AND_SCEW $(INCLUDES)
LFLAGS = -Wall $(DEBUG) $(THREADS)

PROGRAM_NAME = phone

targets = main

pkg_packages := scew smartslog ckpi

PKG_CFLAGS  := $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS := $(shell pkg-config --libs $(pkg_packages))

.PHONY: all

all : $(targets)

main: $(OBJECT) phone.o
	$(CC) $(LFLAGS) $(PKG_LDFLAGS) phone.o $(OBJECT) $(LIBS) -o $(PROGRAM_NAME)

.c.o:
	$(CC) $(CFLAGS) $(PKG_CFLAGS) $(LFLAGS) -o $@ $<


.PHONY: clean
clean:
	rm -f $(targets) *.o
	rm -f $(OBJECT)
	rm -f $(PROGRAM_NAME)
