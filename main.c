#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#define F_CPU 1200000UL
#include <util/delay.h>

#define SERIAL_PORT PORTB
#define SERIAL_MASK 0b00010000
#define SERIAL_BAUD 9600
#include "serial.h"

//#define MAX_VOL 4950UL
#define MAX_VOL 4848UL
#define MAX_VAL 1023UL

#define LIION_DISABLED 0
#define LIION_ENABLED 1
#define LIION_LOW 2

#define BATT_DEF 0
#define BATT_LION 1

#define STATE_UNDF 0
#define STATE_LO 1
#define STATE_HI 2


#define LIION_LED_MASK 0b00000010
#define LIION_DISCHARGE_MASK 0b00000001

//#define MAGIC_TIME_BALANCE 28423
#define MAGIC_TIME_BALANCE 28100

uint16_t adc_read() {
    // Start conversion
    ADCSRA |= _BV(ADSC);

    // Loop until conversion is complete
    while(!bit_is_set(ADCSRA,ADIF));

    // Clear ADIF by writing a 1 (this sets the value to 0)
    ADCSRA |= _BV(ADIF);

    return(ADC);
}

void adc_init() {
    // Enable ADC and set 128 prescale
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    // Select ADC3 as source
    ADMUX = 0b00000011;
}

int main(void) {
    // Set Port B I/O direction
    DDRB = SERIAL_MASK | LIION_LED_MASK | LIION_DISCHARGE_MASK;

    serial_init();

    char buf1[5];
    char buf2[5];

    adc_init();

    int batt = BATT_DEF;
    int state = STATE_UNDF;

    while(1) {
        // Sample the voltage value
        const int samples = 42;
        uint16_t val = 0;
        for(int i = 0; i < samples; i++)
            val += adc_read(0);

        // Get voltage and it's decimal part
        uint16_t voltage = MAX_VOL * val / MAX_VAL / 1000 / samples;
        int decimals = MAX_VOL * val / MAX_VAL / samples % 1000;

        // Detect lion battery
        if(voltage > 3 && state != STATE_LO) {
            batt = BATT_LION;
            state = STATE_HI;
        }

        // Detect def battery
        if(voltage > 1 && state != STATE_LO && batt == BATT_DEF)
            state = STATE_HI;

        // Handle li-ion battery discharge
        if(batt == BATT_LION) {
            if((voltage == 3 && decimals < 300) || voltage < 3)
                state = STATE_LO;
        }

        // Handle def battery discharge
        if(batt == BATT_DEF) {
            if(voltage < 1 && decimals < 900)
                state = STATE_LO;
        }

        // Control discharge gate
        if(state == STATE_HI) {
            // Start discharge
            PORTB |= LIION_DISCHARGE_MASK;
            PORTB |= LIION_LED_MASK;
        } else {
            // Stop discharge
            PORTB &= ~LIION_DISCHARGE_MASK;
            PORTB &= ~LIION_LED_MASK;
        }

        // Send voltage over serial line
        itoa(voltage, buf1, 10);
        itoa(decimals, buf2, 10);
        serial_sendString(buf1);
        serial_sendChar('.');
        if(decimals < 10)
            serial_sendChar('0');
        if(decimals < 100)
            serial_sendChar('0');
        serial_sendString(buf2);
        serial_sendChar('\n');

        // Slow down to 10 measurements per second
        _delay_us(MAGIC_TIME_BALANCE);
    }
}
