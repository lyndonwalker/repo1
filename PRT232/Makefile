# makefile, written by guido socher
MCU=attiny2313
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes 

#-------------------
all: rpm.hex
#-------------------
rpm.hex : rpm.out 
	$(OBJCOPY) -R .eeprom -O ihex rpm.out rpm.hex 
rpm.out : rpm.o 
	$(CC) $(CFLAGS) -o rpm.out -Wl,-Map,rpm.map rpm.o 
rpm.o : rpm.c 
	$(CC) $(CFLAGS) -Os -c rpm.c
# you need to erase first before loading the program.
# load (program) the software into the eeprom:
program: rpm.hex
#	avrdude -p t2313 -c stk200 -U flash:w:rpm.hex -U lfuse:w:0x7d:m
	avrdude -p t2313 -c stk200 -U flash:w:rpm.hex -U lfuse:w:0xFd:m
clean:
	rm -f *.o *.map *.out
#-------------------
