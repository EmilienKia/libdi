## Process this file with automake to produce Makefile.in
## Created by Anjuta

AM_CFLAGS = -Wall -g 

AM_CPPFLAGS = -I../src

bin_PROGRAMS = depinj

depinj_SOURCES = \
	main.cpp \
	service01.hpp

depinj_LDADD = liblibrary01.la ../src/libdi.la 


lib_LTLIBRARIES =  \
	module01.la \
	module02.la \
	liblibrary01.la \
	liblibrary02.la

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
