#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Global variables
extern int g_raw_sock;
extern struct sockaddr_ll device;
extern uint16_t temp_ethp;

// Function prototypes
int ethernet_init(const char *interface);
int ethernet_send(const uint8_t *payload, size_t payload_len);
uint16_t ip_checksum(const void *buf, size_t len);
int send_udp_packet(const uint8_t *dest_mac, const uint8_t *src_mac,
                    uint32_t src_ip, uint32_t dst_ip,
                    uint16_t src_port, uint16_t dst_port,
                    const uint8_t *data, size_t data_len);

struct ethernet_header {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
};

struct ip_header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
};

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
};

struct __attribute__((packed)) udp_packet {
    struct ethernet_header eh;
    struct ip_header ip;
    struct udp_header udp;
    uint8_t data[64];
};



#endif // ETHERNET_H