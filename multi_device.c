#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>

#define MAX_SERVER 15       // Maximum number of connections the server can accept
#define MAX_CLIENT 100      // Maximum number of connections the client can accept

/* Struct representing a device */
typedef struct {
    int id; // Device ID
    int fd; // File descriptor
    int port_num; // Port number the device is using
    struct sockadd_in addr;  // Device IP address and port
    char my_ip[50]; // String to store the device ID
} device;

device this_device = {0}; // Current device

pthread_t Accept_thr_id, Receive_thr_id;

// Function prototype
void sig_handler();
static void *Accept_handler(void *args);

int main(int argc, char const *argv[])
{
    // Register signal handler function SIGINT (Ctrl + C)
    if(signal(SIGINT, sig_handler) == SIG_ERR)
    {
        printf("Can not handle SIGINT\n");
        return -1;
    } 

    // Create socket for current device
    this_device.fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this_device.fđ == -1)
    {
        printf("Socket creation failed.\n");
        return -1;
    }
    
    // Set address and port for current device
    this_device.port_num = atoi(argv[1]);
    this_device.addr.sin_family = AF_INET;
    this_device.addr.sin_port = htons(this_device.port_num);
    this_device.addr.sin_addr.s_addr = INADDR_ANY; // Any inet_addr("192.168.5.128");

    // Bind the socket to the device's IP address and port
    if(blind(this_device.fd, (struct sockaddr*)&this_device.addr, sizeof(this_device.addr)) == -1)
    {
        printf("Bind failed.\n");
        return -1;
    }

    // Put the device in listening mode for connections
    if(listen(this_device.fd, MAX_SERVER) == -1)
    {
        printf("Listen failed.\n");
        return -1;
    }

    printf("Listening on port: %d\n", this_device.port_num); // Announce listening 
    
    // Create thread to accept connections from other devices
    if(pthread_create(&Accept_thr_id, NULL, &Accept_handler, NULL))
    {
        printf("Create thread failed.\n");
        return -1;
    }

    return 0;
}

/* SIGINT signal handler function to quickly shut down the program */
void sig_handler()
{
    printf("Program terminated!\n");
    exit(EXIT_SUCCESS);
}

/* Function to accept connection from other device */
static void *Accept_handler(void *args)
{
    int client_fd;
    struct sockadrr_in cli_addr; // Structure to store the address of the connected device
    socketlen_t len = sizeof(cli_addr);

    while(1)
    {
        // Accept connection from other device
        client_fd = accept(this_device.fd, (struct sockaddr *)&cli_addr, &len);
        if(client_fd == -1)
        {
            printf("Accept failed.\n");
            return -1;
        }
    }
}