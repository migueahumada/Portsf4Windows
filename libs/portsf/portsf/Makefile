# Makefile for portsf using nmake and Visual Studio Compiler

POBJS = ieee80.obj portsf.obj

CFLAGS = /I . /Dunix /O2

CC = cl

.c.obj:
    $(CC) $(CFLAGS) /c $< /Fo$@

all: libportsf.lib

libportsf.lib: $(POBJS)
    lib /OUT:libportsf.lib $(POBJS)

install: libportsf.lib
    copy libportsf.lib ..\lib

# dependencies
portsf.obj: portsf.h portsf.c
ieee80.obj: ieee80.h ieee80.c
