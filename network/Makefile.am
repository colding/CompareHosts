#  
# Copyright (C) 2013 by Jules Colding <jcolding@gmail.com>.
#
# All Rights Reserved.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
#

SUBDIRS = 

noinst_LTLIBRARIES = libnetwork.la
libnetwork_la_LDFLAGS = -static
libnetwork_la_SOURCES = \
	network.c \
	network.h \
	net_interfaces.c \
	net_interfaces.h

AM_CPPFLAGS = $(PROGRAM_CPPFLAGS)
AM_CFLAGS = $(PROGRAM_CFLAGS)

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = $(BUILT_SOURCES) $(CLEAN_IN_FILES) Makefile
CLEANFILES = *~
