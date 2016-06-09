program=voltmeter

voltemeter: main.hex

main.elf: main.c
	avr-gcc -Os -std=c99 -mmcu=attiny13 -o main.elf main.c

main.hex: main.elf
	objcopy -R .eeprom -O ihex main.elf main.hex

clean:
	rm -f main.bin
	rm -f main.hex
	rm -f main.elf

flash: main.hex
	avrdude -u -c usbasp -p attiny13 -U flash:w:main.hex:i

log:
	stty -F /dev/ttyUSB1 raw speed 9600 -crtscts cs8 -parenb -cstopb
	cat /dev/ttyUSB1

serverlog:
	stty -F /dev/ttyUSB0 raw speed 9600 -crtscts cs8 -parenb -cstopb
	cat /dev/ttyUSB0
