#CC           = avr-gcc
#CFLAGS       = -Wall -mmcu=atmega16 -Os -Wl,-Map,test.map
#OBJCOPY      = avr-objcopy
CC           = gcc
LD           = gcc
CFLAGS       = -Wall -Os -c 
LDFLAGS      = -Wall -Os -Wl,-Map,test.map

OBJCOPYFLAFS = -j .text -O ihex
OBJCOPY      = objcopy

# include path to AVR library
INCLUDE_PATH = /usr/lib/avr/include
# splint static check
SPLINT       = splint test.c aes.c -I$(INCLUDE_PATH) +charindex -unrecog

default: test.elf

.SILENT:
.PHONY:  lint clean

test.hex : test.elf
	echo copy object-code to new image and format in hex
	$(OBJCOPY) ${OBJCOPYFLAFS} $< $@

gf_cpu.o : gf_cpu.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_wgen.o : gf_wgen.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_w4.o : gf_w4.c
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_w8.o : gf_w8.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_w16.o : gf_w16.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_w32.o : gf_w32.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_w64.o : gf_w64.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_w128.o : gf_w128.c
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

gf_complete.o : gf_complete.c 
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

VGF_AES.o : VGF_AES.c gf_complete.o
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

test.o : test.c aes.h gf_complete.o aes.o
	echo [CC] $@
	$(CC) $(CFLAGS) -o  $@ $<

aes.o : aes.c aes.h VGF_AES.o 
	echo [CC] $@
	$(CC) $(CFLAGS) -o $@ $<

test.elf : aes.o test.o VGF_AES.o gf_complete.o gf_cpu.o gf_wgen.o gf_w4.o gf_w8.o gf_w16.o gf_w32.o gf_w64.o gf_w128.o
	echo [LD] $@
	$(LD) $(LDFLAGS) -o $@ $^


clean:
	rm -f *.OBJ *.LST *.o *.gch *.out *.hex *.map

lint:
	$(call SPLINT)
