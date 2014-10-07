CC=gcc
CFLAGS=-c
RM=rm -f
INSTALL=install

SOURCES=main.c rtnl_handler.c key_value.c utils.c event.c nl_handler.c log.c \
	netlink.c udev_handler.c pollfd.c fsnotify.c

TARGET=nleventd
PREFIX=/usr

OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o
	$(RM) $(TARGET)

install:
	$(INSTALL) -m 755 $(TARGET) $(PREFIX)/bin
	$(INSTALL) -d -m 755 /etc/nleventd
	$(INSTALL) -d -m 755 /etc/nleventd/rules
	$(INSTALL) -d -m 755 /etc/nleventd/scripts

uninstall:
	$(RM) $(PREFIX)/bin/$(TARGET)
