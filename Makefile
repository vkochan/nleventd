CC=gcc
CFLAGS=-c
RM=rm

SOURCES=main.c rtnl_handler.c key_value.c utils.c event.c nl_handler.c log.c netlink.c udev_handler.c
TARGET=nleventd

OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o
	$(RM) $(TARGET)
