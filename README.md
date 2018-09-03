# Soft_UART_Atmega328p
Implementation of software UART for Atmega328p

Software UART for Atmega328p is based on Timer1 and INT1. 
Function main() contains a validation code that send/recv
byte over software UART and recv/send it by hardware UART.
Implementation has been tested on Raspberry Pi and Arduino
Nano.

Config of software UART:
Soft UART RX pin	==> PD3
Soft UART TX pin 	==> PD2
Baud rate		==> 9600
Packet Byte size	==> 8
