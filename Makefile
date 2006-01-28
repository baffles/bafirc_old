# gnuish Makefile

.PHONY: all clean clean distclean install uninstall

include makefile.plat

#DEBUGMODE=1
#STATICLINK=1
#UNIX=1
ifndef ARCH_TYPE
ARCH_TYPE = pentium
endif

LIBNAME = bafirc

CFLAGS = -I./include
LDFLAGS = -L./lib

ECFLAGS = -I./include
ELDFLAGS = -L./lib

# Check debug options
ifdef DEBUGMODE
LIBNAME := bircd
CFLAGS += -MMD -g3 -W -Wall -march=$(ARCH_TYPE) -DBAFIRC_DEBUG -DFORTIFY -I./fortify
LDFLAGS += -g3

ECFLAGS += -MMD -g3 -W -Wall -march=$(ARCH_TYPE) -DBAFIRC_DEBUG -DFORTIFY -I./fortify
ELDFLAGS += -g3 $(FORTIFY)

FORTIFY = fortify/fortify.o
else
CFLAGS += -MMD -O3 -march=$(ARCH_TYPE) -W -Wall -fomit-frame-pointer -ffast-math -funroll-loops
LDFLAGS += -O3

ECFLAGS += -MMD -O3 -march=$(ARCH_TYPE) -W -Wall -fomit-frame-pointer -ffast-math -funroll-loops
ELDFLAGS += -O3
fortify = 
endif

# Extra stuff
ARCH = lib$(LIBNAME).a
ifndef UNIX
CFLAGS += -Iwin32/pthreads/include
LDFLAGS += -Lwin32/pthreads/lib -lwsock32 -lpthreadgc1
SHARED = $(LIBNAME).dll
else
LDFLAGS += -lpthread
ELDFLAGS += -lpthread
SHARED = $(LIBNAME).so
endif

ifdef STATICLINK
CFLAGS += -DSTATIC
TARGET = lib\/$(ARCH)
else
CFLAGS += -DBUILD_DLL
TARGET = lib\/$(SHARED)
endif

# Our compiler
CC = gcc

# Build a list of source and object files
SRCS = $(wildcard src/*.c)
OBJS = $(subst src/,obj/,$(subst .c,.o,$(SRCS))) $(FORTIFY)

EXS = $(wildcard examples/*.c)
EXO = $(subst examples/,examples/obj/,$(subst .c,.o,$(EXS))) $F
EX =  $(subst .c,$(EXE),$(EXS))

# Default target: make dependancies then the lib
all: $(TARGET) $(EX) $(FORTIFY)
	@echo You are now ready for `make install`! Remeber, on linux will have to su to root
	@echo to install it.

ifneq ($(wildcard obj/*.d),)
-include $(wildcard obj/*.d)
endif

ifndef UNIX
ifeq ($(OS),Windows_NT)
   WINSYSDIR = $(SYSTEMROOT)
   ifeq ($(WINSYSDIR),)
      WINSYSDIR = $(SystemRoot)
   endif
   WINSUBDIR = system32
else
   WINSYSDIR = $(WINDIR)
   ifeq ($(WINSYSDIR),)
      WINSYSDIR = $(windir)
   endif
   WINSUBDIR = system
endif

ifneq ($(WINSYSDIR),)
   WINDIR_U = $(subst \,/,$(WINSYSDIR)/$(WINSUBDIR))
   WINDIR_D = $(subst /,\,$(WINSYSDIR)\$(WINSUBDIR))
else
ifndef CROSSCOMPILE
badwin:
	@echo Your SYSTEMROOT or WINDIR environment variable is not set!
endif
endif
endif

ifndef STATICLINK
ifdef UNIX
$(TARGET): $(OBJS)
#	$(CC) -shared -o $(SHARED) $(OBJS) -Wl,-export-dynamic $(LDFLAGS)
	ar -r $(ARCH) $(OBJS)
	mv ./$(ARCH) ./lib/
#	mv ./$(SHARED) ./lib/
else
$(TARGET): $(OBJS)
	dllwrap --output-lib=$(ARCH) --dllname=$(SHARED) --driver-name=gcc $(OBJS) $(LDFLAGS)
	move $(ARCH) lib
	move $(SHARED) lib
endif
else
$(TARGET): $(OBJS)
	$(CC) -o $(ARCH) $(OBJS) $(LDFLAGS)
endif

ifndef UNIX
clean:
	clean
else
clean:
	rm -f obj/*.o obj/*.d lib/*.*
endif


obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

examples/%$(EXE): examples/obj/%.o
	$(CC) $(ELDFLAGS) $< -o $@ -L./lib -l$(LIBNAME)
examples/obj/%.o: examples/%.c
	$(CC) $(ECFLAGS) -c $< -o $@

distclean: clean
	echo UNIX = 1 > makefile.plat
	echo EXE = >> makefile.plat
	echo IDIR = /usr/local/include>> makefile.plat
	echo LDIR = /usr/local/lib>> makefile.plat

install: $(TARGET)
ifndef UNIX
	copy /y lib\$(SHARED) $(WINDIR_D)
ifdef INSTALL_PTHREADS
	copy /y win32\pthreads\lib\pthreadGC.dll $(WINDIR_D)
endif
	-mkdir $(IDIR)\bafirc
	copy include\bafirc.h $(IDIR)
	copy include\bafirc\callback.h $(IDIR)\bafirc
	copy include\bafirc\identd.h $(IDIR)\bafirc
	copy include\bafirc\irc.h $(IDIR)\bafirc
	copy include\bafirc\socket.h $(IDIR)\bafirc
	copy include\bafirc\threads.h $(IDIR)\bafirc
	copy include\bafirc\typedefs.h $(IDIR)\bafirc
	copy lib\$(ARCH) $(LDIR)
else
	-mkdir $(IDIR)/bafirc
	cp include/bafirc.h $(IDIR)/
	cp -v include/bafirc/*.h $(IDIR)/bafirc/
	cp lib/$(ARCH) $(LDIR)/
endif
	@echo All installed!

uninstall:
ifndef UNIX
	del $(WINDIR_D)\$(SHARED)
	del $(IDIR)\bafirc.h
	del $(IDIR)\bafirc\*.h
	del $(LDIR)\$(ARCH)
else
	rm -f $(IDIR)/bafirc.h
	rm -rf $(IDIR)/bafirc/
	rm -rf $(LDIR)/$(ARCH)
endif
	@echo All uninstalled :/
	
fortify/fortify.o: fortify/fortify.c
	gcc -Wall -DFORTIFY -Wformat -x c -c fortify/fortify.c -o fortify/fortify.o
