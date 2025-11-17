#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ethernet.h"
#include "uart.h"

int main() {
    ethernet_init("eth0");

    uart_init("/dev/serial0", 115200);

    uart_start_threads();


    uint8_t dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t src_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    uint32_t src_ip = inet_addr("192.168.1.100");
    uint32_t dst_ip = inet_addr("192.168.1.255");
    
    const char *msg = "Test";
    int result = send_udp_packet(dest_mac, src_mac, src_ip, dst_ip,
                                12345, 8080, (uint8_t *)msg, strlen(msg));      
    printf("Packet sent: %d bytes\n", result);  


    
    close(g_raw_sock);
    return 0;
}