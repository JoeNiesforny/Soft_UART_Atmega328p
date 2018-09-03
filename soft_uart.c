#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/atomic.h>

#include "gpio.h"

#define TX_DDR	DDRD
#define TX_PORT	PORTD
#define TX_N	PD2

#define RX_DDR	DDRD
#define RX_PORT	PORTD
#define RX_PIN	PIND
#define RX_N	PD3

#define RX_BUFFER_SIZE		16

static volatile char rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_buffer_p;

/* Volatile is used to get variable directly from memory */
static volatile char tx_data;
static volatile uint8_t tx_data_cnt;
static volatile char rx_data_tmp;
static volatile uint8_t rx_data_ready;
static volatile uint8_t rx_data_cnt;

ISR(INT1_vect)
{
	if (rx_data_cnt)
		return;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		rx_data_cnt = 10;
		/*
		 * TODO: Not possible to simultaneously send and receive bytes
		 * using software uart
		 */
		TCNT1 = OCR1A / 2;
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (tx_data_cnt) {
		if (tx_data_cnt == 10) {
			gpio_set_low(TX_PORT, TX_N);
		} else if (tx_data_cnt > 1) {
			if (tx_data & 1)
				gpio_set_high(TX_PORT, TX_N);
			else
				gpio_set_low(TX_PORT, TX_N);
			tx_data >>= 1;
		} else {
			gpio_set_high(TX_PORT, TX_N);
		}

		tx_data_cnt--;
	}

	if (rx_data_cnt) {
		if (rx_data_cnt == 10) {
			rx_data_tmp = 0;
		} else if (rx_data_cnt > 1) {
			rx_data_tmp |= gpio_get(RX_PIN, RX_N) << (9 - rx_data_cnt);
		} else {
			rx_buffer[(rx_buffer_p + rx_data_ready) % RX_BUFFER_SIZE] = rx_data_tmp;
			rx_data_ready++;
		}
		rx_data_cnt--;
	}
}

static char soft_uart_recv_byte(bool block)
{
	char data;

	if (block)
		while (!rx_data_ready);
	else
		if (!rx_data_ready)
			return 0;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		data = rx_buffer[rx_buffer_p % RX_BUFFER_SIZE];
		rx_buffer_p++;
		rx_data_ready--;
	}

	return data;
}

char soft_uart_recv_byte_block()
{
	return soft_uart_recv_byte(true);
}

char soft_uart_recv_byte_no_block()
{
	return soft_uart_recv_byte(false);
}

void soft_uart_send_byte(const char byte)
{
	while (tx_data_cnt);
	tx_data = byte;
	tx_data_cnt = 10;
}

void soft_uart_send(const char *str)
{
	while (*str)
		soft_uart_send_byte(*str++);
}

void soft_uart_enable()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		/* Software TX */
		gpio_dir_out(TX_DDR, TX_N);
		gpio_set_high(TX_PORT, TX_N);

		/* Software RX */
		gpio_dir_in(RX_DDR, RX_N);
		gpio_set_high(RX_PORT, RX_N);

		/* Set ext interrupt on PD2 on falling edge */
		EICRA |= (1 << ISC11) | (0 << ISC10);
		EIMSK |= (1 << INT1);

		/* Set Timer1 */
		TIMSK1 |= (1 << OCIE1A);
		OCR1A = (F_CPU / BAUD) / 8;
		TCCR1B |= (1 << WGM12) | (1 << CS11); // prescaller == 8
	}
}

void soft_uart_disable()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		TCCR1B &= ~(1 << CS11);
		EIMSK &= ~(1 << INT1);
	}
}
