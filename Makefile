# Copyright 2019 - Robin Krens
# Cross compilers links
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
AR=$(TOOLROOT)/arm-none-eabi-ar
AS=arm-none-eabi-as
MKIMG=arm-none-eabi-objcopy

# Compiler flags
# TODO:Cortex-m3 or Cortex-m0?
LDFLAGS+= -mthumb -mcpu=cortex-m3 
CFLAGS+= -mcpu=cortex-m3 -mthumb -g 

# Start up machine assembly
as: 
	$(AS) $(CFLAGS) -o start.o start.asm

# Compile and link all
all:
	$(AS) $(CFLAGS) -o start.o start.asm
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o main.o main.c
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o uart.o uart.c
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o ivt.o ivt.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o systick.o systick.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o sysinfo.o sysinfo.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o lib.o lib.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o mm.o mm.c 
	$(CC) $(CFLAGS) -c -I./include -ffreestanding -o regf.o regf.c 
	$(LD) -nostartfiles -T link.ld -o start.out start.o main.o uart.o ivt.o systick.o sysinfo.o lib.o mm.o regf.o
	$(MKIMG) -Obinary -R .data start.out kernel.bin

# Run in Qemu; note this is a patched version for stm32-f103c8
run:
	/usr/local/bin/qemu-system-arm -serial stdio  -M stm32-f103c8 -kernel kernel.bin

# Examine all sections
examine-all:
	arm-none-eabi-objdump -D start.out | less

# Examine just headers
examine-header:
	arm-none-eabi-objdump -x start.out | less

# Flash kernel to board
flash:
	stm32flash -w kernel.bin -v /dev/ttyUSB0


%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@




