# Favorite Dog Vault
Creating a Multi-Threaded Vault Server capable of handling multiple clients modifying the vault at once. The goal of this exercise is to demonstrate C++ skills.

# Environment

## Compiler
Apple clang version 15.0.0 (clang-1500.1.0.2.5)
Target: x86_64-apple-darwin23.2.0
Thread model: posix

## OS
macOS Sonoma 14.2.1

# How to Use the Vault

## Start by Running the Server

Run the server:
```
clang++ -o server server.cpp  -std=c++11
./server
```

## Then, Run the Client

Run the client
```
clang++ -o client client.cpp  -std=c++11
./client
```

Immediately after running the client program, the server and client will create a socket.

We should see this in the STDOUT for the server and client:
client: ''' The server(hostname) and the port(7734) are now connected ''''
server: '''Got connection from 127.0.0.1'''
## Then, choose a port and press Enter (in this example, I chose port 49152)
```
Please reenter port number between 49152-65535: 49152
```

## Authenticate

Since your favorite dog is considered public information, simply enter your username to proceed. If you do not have a username yet, just enter _logan_.


## Choose an Operation
The options will be displayed in STDOUT from the client program:
```
1. Show Personal Favorite Dog
2. Show All Favorite Dogs 
3. Set a Different Favorite Dog
4. Add a new user to the Vault
5. Save Changes
```

## Once you are done, use option 5 to persistently save the changes for the next time you run the client executable file.

Exit from both the server and client by either killing (ctrl+C) or suspending (ctrl+Z) the process.
