CC=gcc
CFLAGS=-c
RM=rm

SOURCES=main.c nl_route.c key_value.c utils.c rules.c nl_parser.c
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
