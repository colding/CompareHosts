#  
# Copyright (C) 2014 Jules Colding <jcolding@gmail.com>.
#
# All Rights Reserved.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
#

SUBDIRS = 

noinst_PROGRAMS = comparehosts

comparehosts_LDFLAGS = -all-static

comparehosts_SOURCES = \
	main.c

comparehosts_LDADD = \
	$(PROGRAM_top_dir)/cmdline/libcmdline.la \
	$(PROGRAM_top_dir)/network/libnetwork.la \
	$(PROGRAM_top_dir)/marshal/libmarshal.la \
	$(PROGRAM_top_dir)/ipc/libipc.la

AM_CPPFLAGS = $(PROGRAM_CPPFLAGS)
AM_CFLAGS = $(PROGRAM_CFLAGS)

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = $(BUILT_SOURCES) $(CLEAN_IN_FILES) Makefile
CLEANFILES = *~ 
