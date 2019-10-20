# CORTEX M3 FROM SCRATCH

This is my little 'Cortex M3 from scratch' project. No external libraries 
or IDEs are used. Everything is written from scratch. My development environment
consist of nothing more than:

	* GCC-arm-none (standard ARM cross compiler)
	* VIM editor
	* stm32flash

The board I use is a cheap Chinese STM32F103 ripoff. In theory, you should be able to 
port this code to any Cortex M0/M3/M4/M7 board. 

## PROGRESS STATUS
* Setup bare development environment [COMPLETED] -- FILES: Makefile, link.d
* Boot and jump to C [COMPLETED] -- FILES: start.asm, main.c, include/sys/mmap.h, include/sys/robsys.h 
* Interrupt Handling [COMPLETED] -- FILES: ivt.c, lib/string.c
* Basic input and output (UART) [COMPLETED] -- FILES: driver/uart.c, lib/stdio.c
* SysTick [COMPLETED] -- FILE: systick.c
* System Info [COMPLETED] -- FILE: sysinfo.c 
* High Speed External Clock / tuning [COMPLETED] -- FILE: clock.c
* RTC (Real Time Clock) [COMPLETED] -- FILE: rtc.c
* Built-in-shell [COMPLETED] -- FILE: term.c
* Port printf/libc library [COMPLETED] -- FILE: lib/tinyprintf.c
* Basic drivers:
	* EEPROM: drivers/at24c.c [COMPLETED]
	* UART drivers/uart.c [COMPLETED]
	* LED segment display: drivers/tm1637.c [COMPLETED]
	* Temperature sensor: drivers/tsensor.c [COMPLETED]
	* OLED display [PLANNED]
	* Joystick: drivers/mk450_joystick.c [COMPLETED]
	* TFT Screen: drivers/st7735s.c, include/libs/fonts/wogfont.h [IN PROGRESS]
* Memory Management [IN PROGRESS] -- FILE: lib/pool.c
* User Mode [PLANNED]
* System Call PendV implementation [PLANNED]
* Stack trace debug [IN PROGRESS]
* Memory Protection Unit [PLANNED]
* Loadable programs from EEPROM [PLANNED]
* Multiple processes and scheduling [PLANNED]

## SCREENSHOTS
Here are some screenshots that shows the terminal just after booting:

Serial (over UART)

![Screenshot](https://github.com/robinkrens/cortex-from-scratch/raw/master/img/serial.png "serial terminal screenshot")

TFT screen output (SPI):

![Screenshot](https://github.com/robinkrens/cortex-from-scratch/raw/master/img/tft.png "tft peripheral screenshot")





