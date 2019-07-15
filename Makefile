CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
AR=$(TOOLROOT)/arm-none-eabi-ar
AS=arm-none-eabi-as
MKIMG=arm-none-eabi-objcopy

LDFLAGS+= -mthumb -mcpu=cortex-m0 
CFLAGS+= -mcpu=cortex-m3 -mthumb -g 

as: 
	$(AS) $(CFLAGS) -o start.o start.asm

all:
	$(AS) $(CFLAGS) -o start.o start.asm
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o main.o main.c
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o uart.o uart.c
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o ivt.o ivt.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o systick.o systick.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o sysinfo.o sysinfo.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o lib.o lib.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o mm.o mm.c 
	$(LD) -nostartfiles -T link.ld -o start.out start.o main.o uart.o ivt.o systick.o sysinfo.o lib.o mm.o
	$(MKIMG) -Obinary -R .data start.out kernel.bin

run:
	/usr/local/bin/qemu-system-arm -serial stdio  -M stm32-f103c8 -kernel kernel.bin

examine:
	arm-none-eabi-objdump -S start.out

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@




