## Process this file with automake to produce Makefile.in
## Created by Anjuta

AM_CPPFLAGS =
AM_CFLAGS = -Wall -g


lib_LTLIBRARIES = libdi.la
libdi_la_SOURCES = di.cpp di.hpp
libdi_la_LDFLAGS = -lltdl


bin_PROGRAMS = didump
didump_SOURCES = didump.cpp
didump_CPPFLAGS = $(BOOST_CPPFLAGS)
didump_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS)
didump_LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_SYSTEM_LIBS) libdi.la

