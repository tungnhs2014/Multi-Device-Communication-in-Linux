#include "inc/device.h"
#include "inc/handler.h"

device this_device = {0};   // Current device
device device_connect_to[MAX_CLIENTS] = {0};   // List of connected devices
device device_connect_from[MAX_CLIENTS] = {0}; // List of devices connected from

int total_device_to = 0;    // Total number of devices that device is connected to
int total_device_from = 0;  // Total number of connected devices

pthread_t Accept_thr_id, Receive_thr_id;

char command[100];          // String to store user input
char command_option[10];   // String to store command line option

void clear_input_buffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

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
    
    print_list_command(); // Display command list
    
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

        switch (get_command_code(command_option))
        {
                          
            case 1: // help
                /* Display command line when user enter help*/
                print_help();
                break;

            case 2: // myip
                /* Display the ip of the current device */
                print_myIP(this_device.my_ip);
                break;

            case 3: // myport
                /* Display the port number of the current device */
                print_myPort();
                break;

            case 4: // connect
                {
                    char IP_d[20];
                    int port_n;
                    sscanf(command, "%*s %s %d", IP_d, &port_n);
                    
                    // Set up information for the device to connect
                    device_connect_to[total_device_to].fd = socket(AF_INET, SOCK_STREAM, 0);
                    device_connect_to[total_device_to].id = total_device_to;
                    device_connect_to[total_device_to].port_num = port_n;
                    strcpy(device_connect_to[total_device_to].my_ip, IP_d);
                    device_connect_to[total_device_to].addr.sin_family = AF_INET;
                    device_connect_to[total_device_to].addr.sin_port = htons(device_connect_to[total_device_to].port_num);
                    inet_pton(AF_INET, device_connect_to[total_device_to].my_ip, &device_connect_to[total_device_to].addr.sin_addr.s_addr);

                    if(connect_to(device_connect_to[total_device_to]) == 0)
                    {
                        printf("Connected to IP: %s, Port %d\n", IP_d, port_n);
                        total_device_to++;
                    }
                    else
                    {
                        printf("Connection failed.\n");
                    }
                }
                break;

            case 5: // list
                 /* Display list of connected devices */
                print_list_peer();
                break;

            case 6: // send
                /* Process the message sending command */
                {
                    char message[100];
                    int id;
                    // Ensure valid input format
                    if (sscanf(command, "%*s %d %[^\n]", &id, message) < 2) {
                        printf("ERROR: Invalid input. Usage: send <id> <message>\n");
                        break;
                    }

                    // Send message to device with corresponding ID
                    int found = 0;
                    for (int i = 0; i < total_device_to; i++) {
                        if (id == device_connect_to[i].id) {
                            if (send_to(device_connect_to[i], message) == 0) {
                                printf("ERROR: Failed to send message to device %d.\n", id);
                            }
                            found = 1;
                            break;
                        }
                    }

                    // Inform user if device ID is not found
                    if (!found) {
                        printf("ERROR: Device with ID %d not found.\n", id);
                    }
                            
                    break;
                }

            case 7: // terminate
                {
                    int ID_temp;
                    // Get the ID of the device to disconnect
                    sscanf(command, "%*s %d", &ID_temp);
                    
                    for (int i = 0; i < total_device_to; i++)
                    {
                        if(ID_temp == device_connect_to[i].id)
                        {
                            terminate_id(&device_connect_to[i]);
                        }
                    }
                    break;
                }
               
            case 8: // exit
                {
                    // Disconnect from all devices
                    for (int i = 0; i < total_device_to; i++)
                    {
                        terminate_id(&device_connect_to[i]);
                    }
                    printf("**************************************************************************\n");
                    printf("-----------------------ENDING PROGRAM-------------------------------------\n");
                    printf("**************************************************************************\n");
                    break;
                }
              
            default:
                printf("INVALID command.\n");
                break;
        }
    }

    printf("\nPress Enter to continue...");
    clear_input_buffer(); 

    return 0;
}
