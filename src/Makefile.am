## Process this file with automake to produce Makefile.in
## Created by Anjuta

AM_CPPFLAGS = 

AM_CFLAGS = -Wall -g

noinst_PROGRAMS = depinj

depinj_SOURCES = \
	main.cpp \
	service01.hpp

depinj_LDADD = liblibrary01.la libdi.la 



lib_LTLIBRARIES = libdi.la

noinst_LTLIBRARIES =  \
	module01.la \
	module02.la \
	liblibrary01.la \
	liblibrary02.la

libdi_la_SOURCES =  \
	di.cpp \
	di.hpp

libdi_la_CFLAGS =

libdi_la_LDFLAGS = -lltdl

module01_la_SOURCES =  \
	module01.cpp

module01_la_LDFLAGS = -module \
	-avoid-version 

module02_la_SOURCES =  \
	module02.cpp

module02_la_LDFLAGS = -module \
	-avoid-version 

liblibrary01_la_SOURCES =  \
	library01.cpp

liblibrary02_la_SOURCES =  \
	library02.cpp
