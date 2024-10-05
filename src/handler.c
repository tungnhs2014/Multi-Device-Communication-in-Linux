#include "handler.h"
#include "device.h"

extern device this_device;
extern device device_connect_to[MAX_CLIENTS];
extern device device_connect_from[MAX_CLIENTS];
extern int total_device_to;
extern int total_device_from;

extern pthread_t Receive_thr_id;

/* SIGINT signal handler function to quickly shut down the program */
void sig_handler()
{
    printf("***********************************************************\n");
    printf("-----------------Program terminated!-----------------------\n");
    printf("***********************************************************\n");
    exit(EXIT_SUCCESS);
}
// Function to map command line commands to numeric values
int get_command_code(char *command) {
    if (!strcmp(command, "help")) return 1;
    if (!strcmp(command, "myip")) return 2;
    if (!strcmp(command, "myport")) return 3;
    if (!strcmp(command, "connect")) return 4;
    if (!strcmp(command, "list")) return 5;
    if (!strcmp(command, "send")) return 6;
    if (!strcmp(command, "terminate")) return 7;
    if (!strcmp(command, "exit")) return 8;

    return -1; // invalid command

}
/* Function to display list of commands */
void print_list_command()
{
    printf("************************Command List*************************\n");
    printf("help                         : Display all command\n");
    printf("myip                         : Display IP of this device\n");
    printf("myport                       : Display port of this device\n");
    printf("connect <ip> <port_num>:     : Connect to device with IP and port at port num\n");
    printf("list                         : Display all device connect\n");
    printf("send <id> <message>          : send message to device with id connect id \n");
    printf("terminate <id>               : Disconnect with device at id connect id\n");
    printf("exit                         : close application\n");
    printf("************************************************************\n");
}

/* Function to display list of commands */
void print_help()
{
    printf("************************Command List*************************\n");
    printf("help                         : Display all command\n");
    printf("myip                         : Display IP of this device\n");
    printf("myport                       : Display port of this device\n");
    printf("connect <ip> <port_num>:     : Connect to device with IP and port at port num\n");
    printf("list                         : Display all device connect\n");
    printf("send <id> <message>          : send message to device with id connect id \n");
    printf("terminate <id>               : Disconnect with device at id connect id\n");
    printf("exit                         : close application\n");
    printf("************************************************************\n");
}

/* Function to display the port number of the current device */
void print_myPort() 
{
    printf("My port is: %d\n", this_device.port_num);
}

/* Function to display list of connected devices */
void print_list_peer() 
{
    printf("Device connection to*********************\n");
    printf("ID |        IP Address         | Port No.\n");

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(device_connect_to[i].fd > 0)
        {
            printf("ID: %d | IP: %s | Port: %d\n", device_connect_to[i].id, device_connect_to[i].my_ip, device_connect_to[i].port_num);
        }
    }
    printf("*****************************************\n");
}

/* Function to connect to other devices */
int connect_to(device dev)
{
    return connect(dev.fd, (struct sockaddr*)&dev.addr, sizeof(dev.addr));
}

void print_myIP(char *ip_add)
{
    // Open stream to get device IP address via 'hostname -I' command
    FILE *fd = popen("hostname -I", "r");
    
    if (fd == NULL)
    {
        printf("ERROR: Can not get IP address\n");
        return;
    }

    if (fgets(ip_add, 100, fd) == NULL)
    {
        printf("ERROR: Can not get IP address\n");
        return;
    }

    printf("My IP is : %s", ip_add); 
}

/* Function to send message to other device */
int send_to(device dev, char *message)
{
    // Check if the device is still connected
    if (dev.fd < 0)
    {
        printf("This device has just been terminated\n");
        return 0;
    }
    // Send message to device via socket
    if(write(dev.fd, message, strlen(message) + 1) < 0)
    {
        printf("ERROR: Can not send message\n");
        return 0;
    }
    return 1;
}

/* Function to accept connection from other device */
void *Accept_handler(void *args)
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
            printf("Accept new device failed.\n");
            return 0;
        }

        // Save newly connected device information to array
        device_connect_from[total_device_from].fd = client_fd;
        device_connect_from[total_device_from].id = total_device_from;
        device_connect_from[total_device_from].addr = cli_addr;

        device_connect_from[total_device_from].port_num = ntohs(cli_addr.sin_port);
        inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, device_connect_from[total_device_from].my_ip, 50);

        printf("\nNew connection from IP: %s, Port: %d\n", device_connect_from[total_device_from].my_ip, device_connect_from[total_device_from].port_num);

        // Create a thread to receive messages from newly connected devices
        if(pthread_create(&Receive_thr_id, NULL, &Receive_handler, &device_connect_from[total_device_from]))
        {
            printf("Can not create thread to receive message\n");
            return 0;
        }
        total_device_from++;  // Increase the number of connected devices

    }
}

void *Receive_handler(void *args)
{
    device *rev_thr = (device *)args; // Get the pointer to the device receiving data
    char buff_rev[100]; // Buffer to hold received data

    while (1)
    {
        // Read data from device via socket
        ssize_t bytes_read = read(rev_thr->fd, buff_rev, sizeof(buff_rev) - 1);  // Limit buffer to size - 1
        if (bytes_read < 0) {
            printf("ERROR: Can not read data\n");
            return NULL;
        }

        // Ensure null-terminated string
        buff_rev[bytes_read] = '\0';

        if(rev_thr->fd >= 0)
        {
            printf("***Message receive from: %s\n", rev_thr->my_ip);  // Sender's IP
            printf("***Sender Port:          %d\n", rev_thr->port_num); // Sender's Port
            printf("***Message:              %s\n", buff_rev); // content message
        }
        else
        {
            printf("Notification: %s\n", buff_rev); // Connection error message
        }
    }
}

/* Function to disconnect from device by ID */
void terminate_id(device *dev)
{
    char str[100];
    // Notify other devices that the connection has been dropped
    sprintf(str, "The connection at port %d has just been terminated\n", dev->port_num);
    send_to(*dev, str);
    dev->fd = -1;  // Set file descriptor to -1 to annouce disconnection
}
