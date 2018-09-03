void uart_enable(void);
void uart_disable(void);
void uart_send_byte(uint8_t data);
bool uart_recv_byte_non_block(char *data);
char uart_recv_byte_block(void);
void uart_send(const char data[]);
void uart_recv(char data[], int size);
