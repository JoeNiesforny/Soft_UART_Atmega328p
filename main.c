#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include "gpio.h"
#include "soft_uart.h"
#include "uart.h"

int main(void)
{
	char buffer[256];

	uart_enable();
	soft_uart_enable();

	sprintf(buffer, "Hello from atmega328p\r\n");

	soft_uart_send(buffer);

	while (true) {
		uart_send_byte(soft_uart_recv_byte_block());
		//soft_uart_send_byte(uart_recv_byte_block());
		_delay_ms(10);
	};

	return 0;
}
