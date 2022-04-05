TARGET		:= src/anydisk
LIBOBJS 	:= inih/ini.o src/config.o
OBJS		:= $(TARGET:=.o) $(LIBOBJS)
THRID_PARTY := $(shell ls -l ./thrid | grep ^d | awk '{print "thrid/"$$9}')

CC		:= /usr/bin/gcc
override CFLAGS += -g -pedantic -Wall -Wextra -I./include -I./ -I./thrid/disksim-4.0/src
LDFLAGS		:= -L./lib -lbuse -ldisksim -lddbg -lparam -ldiskmodel -lmemsmodel -lssdmodel -lm

.PHONY: all clean
# all: $(TARGET)
all: $(LIBOBJS) $(TARGET:=.o)
	$(MAKE) -C thrid/BUSE
	cp thrid/BUSE/*.a lib/
	cp thrid/BUSE/buse.h include/

	$(MAKE) -C thrid/disksim-4.0
	cp thrid/disksim-4.0/lib/*.a lib/
	cp thrid/disksim-4.0/libddbg/lib/*.a lib/
	cp thrid/disksim-4.0/libparam/lib/*.a lib/
	cp thrid/disksim-4.0/diskmodel/lib/*.a lib/
	cp thrid/disksim-4.0/memsmodel/lib/*.a lib/
	cp thrid/disksim-4.0/ssdmodel/lib/*.a lib/

	cp -r thrid/disksim-4.0/include/* include
	cp -r thrid/disksim-4.0/libddbg/include/* include/
	cp -r thrid/disksim-4.0/libparam/include/* include/
	cp -r thrid/disksim-4.0/diskmodel/include/* include/
	cp -r thrid/disksim-4.0/memsmodel/include/* include/
	cp -r thrid/disksim-4.0/ssdmodel/include/* include/

	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# $(TARGET): $(LIBOBJS) $(TARGET:=.o) $(THRID_PARTY)
# 	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(TARGET:=.o): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIBOBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<




# $(TARGET): %: %.o $(STATIC_LIB)
# 	$(CC) -o $@ $< $(LDFLAGS)

# $(TARGET:=.o): %.o: %.c buse.h
# 	$(CC) $(CFLAGS) -o $@ -c $<

# $(STATIC_LIB): $(LIBOBJS)
# 	ar rcu $(STATIC_LIB) $(LIBOBJS)

# $(LIBOBJS): %.o: %.c
# 	$(CC) $(CFLAGS) -o $@ -c $<

clean: 
# rm -f $(TARGET) $(OBJS) $(STATIC_LIB)
	rm -rf lib/*a include/*

	for d in $(THRID_PARTY); do \
		$(MAKE) -C $$d $@; \
	done
