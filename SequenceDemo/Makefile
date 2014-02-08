# makefile, written by guido socher
MCU=attiny85
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -DF_CPU=8000000UL

#-------------------
all: sequence.hex
#-------------------
sequence.hex : sequence.out 
	$(OBJCOPY) -R .eeprom -O ihex sequence.out sequence.hex 
sequence.out : sequence.o 
	$(CC) $(CFLAGS) -o sequence.out -Wl,-Map,sequence.map sequence.o 
sequence.o : sequence.c 
	$(CC) $(CFLAGS) -Os -c sequence.c
# you need to erase first before loading the program.
# load (program) the software into the eeprom:
program: sequence.hex
	avrdude -p t85 -c usbtiny -D -U flash:w:sequence.hex
clean:
	rm -f *.o *.map *.out
	avrdude -p t85 -c usbtiny -e
#-------------------
