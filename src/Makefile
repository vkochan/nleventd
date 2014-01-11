CC=gcc
CFLAGS=-c
RM=rm

SOURCES=main.c rtnl_parser.c key_value.c utils.c rules.c nl_parser.c log.c netlink.c
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
