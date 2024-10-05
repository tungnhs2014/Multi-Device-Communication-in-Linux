## Author: Nguyen Huu Son Tung

## Project
"Project for designing a communication chat application between multiple devices on Linux, applying IPC Socket (TCP) and multi-threading. Users can interact with the system using various commands with different features."

<p align="center">
  <img src="https://github.com/user-attachments/assets/fce01e13-3154-4181-a4a2-f40e86850d13" alt="Alt text" width="800" height="300">
</p>

## Directories Structure
- `bin`: Directory for executable files
- `inc`: Directory for header files
- `lib`: Directory for static and shared libraries
- `obj`: Directory for object files
- `src`: Directory for source files

## Commands
1. **myip**: Display the IP address of this process.
2. **myport**: Display the port on which this process is listening for incoming connections.
3. **help**: Display information about command manual.
4. **connect \<IP\> \<port no\>**: Establish a new TCP connection to the specified \<IP\> at the specified \<port no\>. The \<destination\> is the IP address of the computer. Invalid connection attempts should be rejected with a suitable error message.
5. **list**: Display a numbered list of all connections this process is part of, including connections initiated by this process and other processes., 
   - Example
   ```
     1: 192.168.48.130 6060
     2: 192.168.48.130 7070
     3: 192.168.48.131 8080
     4: 192.168.48.131 9090
     ``'
6. **terminate \<connection id.\>**: Terminate the connection listed under the specified number when the `list` command is used to display.
7. **send \<connection id.\> \<message\>**: Send the message to the host on the connection designated when the `list` command is used. The message can be up to 100 characters long, including blank spaces. 
8. **exit**: Close all connections and terminate this process.

## Usage
- `make static`: Build object files with static library
- `make shared`: Build object files with shared library
- `make clean`: Clean files lib, obj, bin
- `run`: ./bin/multi_device port_no (Ex: ./bin/multi_device 8080)
