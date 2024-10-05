#ifndef DEVICE_H
#define DEVICE_H

#include <netinet/in.h> // Provide for struct sockaddr_in

#define MAX_SERVER 15       // Maximum number of connections the server can accept
#define MAX_CLIENTS 100      // Maximum number of connections the client can accept

/* Struct representing a device */
typedef struct {
    int id; // Device ID
    int fd; // File descriptor
    int port_num; // Port number the device is using
    struct sockaddr_in addr;  // Device IP address and port
    char my_ip[50]; // String to store the device ID
} device;

#endif