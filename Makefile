#
#test L218 tz1000
#
#

TARGET = tz1000_l218_main

INCLUDES = -I./

OBJECTS = tz1000_l218_main.o \
	      Serial.o 

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -Wall -std=gnu99 -g -c $< -o $@


all:  build

build: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET) -lc

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
