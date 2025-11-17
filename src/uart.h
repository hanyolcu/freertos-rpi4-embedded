#ifndef UART_COMM_H
#define UART_COMM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <sys/types.h>

#define MAX_PAYLOAD 256
#define UART_HEADER_T 0x0807060504030201ULL

typedef struct {
    u_int8_t header[8];
    __uint16_t payload_length;
    __uint16_t crc16;
    u_int8_t payload[MAX_PAYLOAD];
} sUart_Frame;

typedef struct {
    u_int8_t header[8];
    __uint16_t payload_length;
    __uint16_t crc16;
    u_int8_t payload[MAX_PAYLOAD];
} sUart_Frame_recieve;

// Function declarations
int uart_init(const char* device, int baud_rate);
int update_txbuffer(const char* payload, size_t payload_length);
__uint16_t crc16_calculator(const unsigned char* data, size_t length);
void* tx_thread_func(void* arg);
void* receive_thread_func(void* arg);
void cleanup();
int uart_start_threads();

#endif