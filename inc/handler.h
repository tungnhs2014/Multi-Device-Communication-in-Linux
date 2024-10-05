#ifndef HANDLER_H
#define HANDLER_H

// libraries needed for socket, thread, and signal handling
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include "device.h" 

// Declare functions needed for the program
void sig_handler();
int get_command_code(char *command);
void print_myPort();
void print_list_peer();
void print_list_command();
void print_help();
void print_myIP(char *ip_add);

int connect_to(device dev);
int send_to(device dev, char *message);
void terminate_id(device *dev);

void *Accept_handler(void *args);
void *Receive_handler(void *args);

#endif