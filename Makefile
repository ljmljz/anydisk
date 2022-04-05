TARGET		:= src/anydisk
LIBOBJS 	:= inih/ini.o src/config.o
OBJS		:= $(TARGET:=.o) $(LIBOBJS)
THRID_PARTY := $(shell ls -l ./thrid | grep ^d | awk '{print "thrid/"$$9}')
DISKSIM     := thrid/disksim-4.0

CC		:= /usr/bin/gcc
override CFLAGS += -g -pedantic -Wall -Wextra -I./include -I./ -I./$(DISKSIM)/src
LDFLAGS		:= -L./lib -lbuse -ldisksim -lddbg -lparam -ldiskmodel -lmemsmodel -lssdmodel -lm

.PHONY: all clean
# all: $(TARGET)
all: $(LIBOBJS) $(TARGET:=.o)
	mkdir -p lib

	$(MAKE) -C thrid/BUSE
	cp thrid/BUSE/*.a lib/

	$(MAKE) -C $(DISKSIM)
	cp $(DISKSIM)/lib/*.a lib/
	cp $(DISKSIM)/libddbg/lib/*.a lib/
	cp $(DISKSIM)/libparam/lib/*.a lib/
	cp $(DISKSIM)/diskmodel/lib/*.a lib/
	cp $(DISKSIM)/memsmodel/lib/*.a lib/
	cp $(DISKSIM)/ssdmodel/lib/*.a lib/

	cp -r $(DISKSIM)/include/* include/
	cp -r $(DISKSIM)/libddbg/include/* include/
	cp -r $(DISKSIM)/libparam/include/* include/
	cp -r $(DISKSIM)/diskmodel/include/* include/
	cp -r $(DISKSIM)/memsmodel/include/* include/
	cp -r $(DISKSIM)/ssdmodel/include/* include/

	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

	mv $(TARGET) ./

$(TARGET:=.o): %.o: %.c
	mkdir -p include
	cp thrid/BUSE/buse.h include/
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIBOBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean: 
	rm -f $(TARGET) $(OBJS)
	rm -rf lib/*a include/*

	for d in $(THRID_PARTY); do \
		$(MAKE) -C $$d $@; \
	done
