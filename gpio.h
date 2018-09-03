
#pragma once

/* Direction: DDRA, DDRB, DDRC, DDRD */
#define gpio_dir_in(dir, pin)		dir &= ~(1 << pin)
#define gpio_dir_out(dir, pin)		dir |= (1 << pin)

/* Port: PORTA, PORTB, PORTC, PORTD */
#define gpio_set_low(port, pin)		port &= ~(1 << pin)
#define gpio_set_high(port, pin)	port |= (1 << pin)

/* Port: PINA, PINB, PINC, PIND */
#define gpio_get(port, pin)		((port >> pin) & 1)
