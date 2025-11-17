#include "uart.h"
#include <semaphore.h>


sem_t gtx_semaphore;
sUart_Frame gTx_Buffer, gRx_Buffer;
int uart_fd = -1;
pthread_t tx_thread, receive_thread;
volatile int running = 1;
u_int8_t uart_header[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};


// UART initialization function
int uart_init(const char* device, int baud_rate) {
    struct termios options;
    
    uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    
    
    tcgetattr(uart_fd, &options);

    
    speed_t speed;
    switch (baud_rate) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 57600:  speed = B57600; break;
        case 115200: speed = B115200; break;
        default:     speed = B9600; break;
    }
    
    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);

    // Configure 8N1
    options.c_cflag &= ~PARENB;   // No parity
    options.c_cflag &= ~CSTOPB;   // 1 stop bit
    options.c_cflag &= ~CSIZE;    // Clear data size bits
    options.c_cflag |= CS8;       // 8 data bits
    options.c_cflag |= CREAD | CLOCAL;
    
    // Raw input/output mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
    options.c_oflag &= ~OPOST;
    
    // interbyte timeout mode
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 1;
    
    tcsetattr(uart_fd, TCSANOW, &options);

    tcflush(uart_fd, TCIOFLUSH);
    return 0;
}


int uart_start_threads() {
    // Initialize semaphore
    sem_init(&gtx_semaphore, 0, 0);
    
    // Create TX thread
    pthread_create(&tx_thread, NULL, tx_thread_func, NULL);

    
    // Create receive thread
    pthread_create(&receive_thread, NULL, receive_thread_func, NULL);

    return 0;
}


int update_txbuffer(const char* payload, size_t payload_length) {
    
    // Clear buffer
    memset(&gTx_Buffer, 0, sizeof(gTx_Buffer));
    
    // Copy header 
    memcpy(gTx_Buffer.header, &(u_int64_t){UART_HEADER_T}, 8);
    
    
    // Set payload length
    gTx_Buffer.payload_length = (__uint16_t)payload_length;
    
    // Copy payload 
    memcpy(gTx_Buffer.payload, payload, payload_length);
    
    

    __uint16_t crc = crc16_calculator((const unsigned char*)payload, payload_length);
    gTx_Buffer.crc16 = crc;
    
    // Signal transmitter thread
    sem_post(&gtx_semaphore);
    
    return 0;
}


__uint16_t crc16_calculator(const unsigned char* data, size_t length) {
    __uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        crc ^= (__uint16_t)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}


void* tx_thread_func(void* arg) {

    
    while (running) {
        // Wait for semaphore signal
        sem_wait(&gtx_semaphore);
        
        if (!running) break;
        
        int bytes_written = write(uart_fd, &gTx_Buffer, 8+2+2+gTx_Buffer.payload_length);

        tcdrain(uart_fd);

        
    }
    
    return NULL;
}


void cleanup() {
    running = 0;
    
    // Signal semaphore to wake up transmitter thread
    sem_post(&gtx_semaphore);
    
    // Wait for threads to finish
    if (tx_thread) {
        pthread_join(tx_thread, NULL);
    }
    
    if (receive_thread) {
        pthread_join(receive_thread, NULL);
    }
    
    sem_destroy(&gtx_semaphore);
    
    // Close UART
    if (uart_fd >= 0) {
        close(uart_fd);
    }
    
    printf("Cleanup completed\n");
}


// Thread function for receiving data
void* receive_thread_func(void* arg) {

    
    
    
    while (running) {
        int header_read = read(uart_fd, &gRx_Buffer, sizeof(sUart_Frame_recieve));
        
        if (header_read > 0) {
            if (memcmp(uart_header, gRx_Buffer.header, 8) == 0) {
                //echo the signal
                update_txbuffer((const char*)gRx_Buffer.payload, gRx_Buffer.payload_length);
                //clear buffer
                memset(&gRx_Buffer, 0, sizeof(gRx_Buffer));

                

            }

        }
              

    }
    
    printf("Receive thread stopping\n");
    return NULL;
}