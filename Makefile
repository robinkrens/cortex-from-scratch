CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
AR=$(TOOLROOT)/arm-none-eabi-ar
AS=arm-none-eabi-as
MKIMG=arm-none-eabi-objcopy

LDFLAGS+= -mthumb -mcpu=cortex-m3 
CFLAGS+= -mcpu=cortex-m3 -mthumb -g 

as: 
	$(AS) $(CFLAGS) -o start.o start.asm

all:
	$(AS) $(CFLAGS) -o start.o start.asm
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o main.o main.c
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o uart.o uart.c
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o ivt.o ivt.c 
	$(LD) -nostartfiles -T link.ld -o start.out start.o main.o uart.o ivt.o
	$(MKIMG) -Obinary -R .data start.out kernel.bin

run:
	qemu-system-arm -serial stdio  -M lm3s6965evb -kernel kernel.bin

examine:
	arm-none-eabi-objdump -S start.out

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@




