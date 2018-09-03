#include <avr/io.h>

#include <util/setbaud.h>
#include <stdbool.h>

/* Pins for UART communication */
#define RXD	PIND0
#define TXD	PIND1

void uart_enable(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);
}

void uart_disable(void)
{
	char dummy;
	uint8_t i;

	UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));

	for (i = 0; i < 5; i++) {
		if (!(UCSR0A & (1 << RXC0)))
			continue;
		dummy = UDR0;
	}
	i = dummy;
}

void uart_send_byte(char data)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

bool uart_recv_byte_non_block(char *data)
{
	if ((UCSR0A & (1 << RXC0))) {
		if (data)
			*data = UDR0;
		return true;
	}
	return false;
}

char uart_recv_byte_block(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

/* Last byte in array has to be 0 */
void uart_send(const char data[])
{
	while (*data)
		uart_send_byte(*data++);
}

/* Received message till new line or zero sign */
void uart_recv(char data[], int size)
{
	int i;
	for (i = 0; i < size; i++) {
		data[i] = uart_recv_byte_block();
		if (data[i] == '\n' || data[i] == '\0')
			break;
	}
}
