#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>

#define SERIAL_MAGIC_SLOWDOWN 27
#define SERIAL_DELAY (1000000 / SERIAL_BAUD - SERIAL_MAGIC_SLOWDOWN)

static void inline serial_hi() {
    SERIAL_PORT |= SERIAL_MASK;
}

static void inline serial_lo() {
    SERIAL_PORT &= ~SERIAL_MASK;
}

static void inline serial_delay() {
    _delay_us(SERIAL_DELAY);
}

void serial_init() {
    // Start mark
    serial_hi();
}

void serial_sendChar(char character) {
    // Start bit
    serial_lo();
    serial_delay();

    // Data bits
    for(int i = 0; i < 8; ++i) {
        if((1 << i) & character)
            serial_hi();
        else
            serial_lo();

        serial_delay();
    }

    // Stop bit
    serial_lo();
    serial_delay();

    // Set mark signal
    serial_hi();
}

int serial_sendString(char *string) {
    while(*string != 0)
        serial_sendChar(*string++);
}


#endif //SERIAL_H
