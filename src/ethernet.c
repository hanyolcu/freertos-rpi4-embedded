#include "ethernet.h"

// Global variables
int g_raw_sock = -1;
struct sockaddr_ll device;
uint16_t temp_ethp = ETH_P_ALL;


uint16_t ip_checksum(const void *buf, size_t len) {
    const uint16_t *data = buf;
    uint32_t sum = 0;
    
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    
    if (len == 1) {
        sum += *(uint8_t *)data;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

// Initialize raw socket
int ethernet_init(const char *interface) {
    //interface settings
    struct ifreq ifr;

    // Open raw socket, layer 2, manual ethernet frames, protocol
    g_raw_sock = socket(AF_PACKET, SOCK_RAW, htons(temp_ethp));


    // clear
    memset(&ifr, 0, sizeof(ifr));

    //get interface name
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    //passing index of the interface
    ioctl(g_raw_sock, SIOCGIFINDEX, &ifr);


    //clear 
    memset(&device, 0, sizeof(device));

    // Fill sockaddr_ll
    memset(&device, 0, sizeof(device));
    device.sll_family = AF_PACKET;
    device.sll_ifindex = ifr.ifr_ifindex;
    device.sll_halen = ETH_ALEN;
    device.sll_protocol = htons(ETH_P_IP);

    return 0;
}

// Send raw Ethernet packet
int ethernet_send(const uint8_t *payload, size_t payload_len) {

    ssize_t sent = sendto(g_raw_sock, payload, payload_len, 0,
                          (struct sockaddr *)&device, sizeof(device));
    


    return sent;
}

int send_udp_packet(const uint8_t *dest_mac, const uint8_t *src_mac,
                    uint32_t src_ip, uint32_t dst_ip,
                    uint16_t src_port, uint16_t dst_port,
                    const uint8_t *data, size_t data_len) {
    
    struct udp_packet packet;
    memset(&packet, 0, sizeof(packet));
    
    // Limit data length to buffer size
    if (data_len > sizeof(packet.data)) {
        data_len = sizeof(packet.data);
    }
    
    // Build Ethernet header
    memcpy(packet.eh.dest_mac, dest_mac, 6);
    memcpy(packet.eh.src_mac, src_mac, 6);
    packet.eh.ethertype = htons(0x0800); 
    
    // Build IP header
    packet.ip.version_ihl = 0x45; 
    packet.ip.tos = 0;
    packet.ip.total_length = htons(20 + 8 + data_len); 
    packet.ip.identification = htons(54321);
    packet.ip.flags_fragment = 0;
    packet.ip.ttl = 64;
    packet.ip.protocol = 17; // UDP
    packet.ip.checksum = 0;
    packet.ip.src_ip = src_ip;
    packet.ip.dst_ip = dst_ip;
    
    // Calculate IP checksum
    packet.ip.checksum = ip_checksum(&packet.ip, 20);
    
    // Build UDP header
    packet.udp.src_port = htons(src_port);
    packet.udp.dst_port = htons(dst_port);
    packet.udp.length = htons(8 + data_len); 
    packet.udp.checksum = 0; 
    
    // Copy data

    memcpy(packet.data, data, data_len);
    
    // Calculate total packet size
    size_t packet_len = sizeof(struct ethernet_header) + 
                        sizeof(struct ip_header) + 
                        sizeof(struct udp_header) + 
                        data_len;
    
    // Send packet
    return ethernet_send((uint8_t *)&packet, packet_len);
}