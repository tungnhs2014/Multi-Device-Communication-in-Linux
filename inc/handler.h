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
void print_myPort();
void print_list_peer();
int connect_to(device dev);
int send_to(device dev, char *message);

void *Accept_handler(void *args);
void *Receive_handler(void *args);

#endif