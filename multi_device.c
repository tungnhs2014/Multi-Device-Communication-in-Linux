#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>

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

device this_device = {0};   // Current device
device device_connect_to[MAX_CLIENTS] = {0};   // List of connected devices
device device_connect_from[MAX_CLIENTS] = {0}; // List of devices connected from

int total_device_to = 0;    // Total number of devices that device is connected to
int total_device_from = 0;  // Total number of connected devices

pthread_t Accept_thr_id, Receive_thr_id;

char command[100];          // String to store user input
char command_option[10];   // String to store command line option

// Function prototype
void sig_handler();
static void *Accept_handler(void *args);
static void *Receive_handler(void *args);

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
    if(this_device.fd == -1)
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
    if(bind(this_device.fd, (struct sockaddr*)&this_device.addr, sizeof(this_device.addr)) == -1)
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
        printf("Can not create thread for accept new device.\n");
        return -1;
    }

    while(1)
    {
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin); // Get command from the user
        sscanf(command, "%s", command_option);  // Extract commands and options from input string

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
    struct sockaddr_in cli_addr; // Structure to store the address of the connected device
    socklen_t len = sizeof(cli_addr);

    while(1)
    {
        // Accept connection from other device
        client_fd = accept(this_device.fd, (struct sockaddr *)&cli_addr, &len);
        if(client_fd == -1)
        {
            printf("Accept failed.\n");
            return 0;
        }

        // Save newly connected device information to array
        device_connect_from[total_device_from].fd = client_fd;
        device_connect_from[total_device_from].port_num = ntohs(cli_addr.sin_port);
        inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, device_connect_from[total_device_from].my_ip, 50);

        printf("New connection from IP: %s, Port: %d\n", device_connect_from[total_device_from].my_ip, device_connect_from[total_device_from].port_num);

        // Create a thread to receive messages from newly connected devices
        if(pthread_create(&Receive_thr_id, NULL, &Receive_handler, &device_connect_from[total_device_from]))
        {
            printf("Can not create thread to receive message\n");
            return 0;
        }
        total_device_from++;  // Increase the number of connected devices

    }
}

static void *Receive_handler(void *args)
{
    device *rev_thr = (device *)args; // Get the pointer to the device receiving data
    char buff_rev[100]; // Buffer to hold received data

    while (1)
    {
        // Read data from device via socket
        if (read(rev_thr->fd, buff_rev, sizeof(buff_rev)) < 0)
        {
            printf("Can not read data\n");
            return 0;
        }

        if(rev_thr->fd >= 0)
        {
            printf("Message from %s: %s\n", rev_thr->my_ip, buff_rev);
        }
    }
}