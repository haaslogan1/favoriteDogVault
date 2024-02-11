#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <dirent.h>

#define BUFFER_SIZE 1024


using namespace std;


string usr;


string auth_request(string usr) {
    string message = "AUTH  \r\n" \
                     "User: " + usr + " \r\n";
    return message;
}

string genMsg(string str) {
	string message = "****************Client Request to Server******************\n" + \
			str  + "\n"\
			+ "****************Client Request to Server******************\n\n\n";
	return message;
}

void sendMsg(int sock, string msg) {
	// send the message to the server
	try {
		 // send the message
		 if (send(sock, msg.c_str(), strlen(msg.c_str()), 0) < 0) {
			std::cerr << "Failed to send the message.";
		 } else {
            std::cout << "Successfully sent the message: \n" << msg << endl;
    	}
    } catch (...) {
    	std::cout << "There was an issue trying to send the message." << endl;
    }
}

string receiveMsg(int sock) {
	char res_from_server[10000];
	// receive the message from the server 
	try {
		// Ensure that we receive more than 0 bytes. Otherwise, there was an issue.
		if ( recv(sock, res_from_server, 10000, 0) > 0) {
			cout << "Response from the server:\n" << res_from_server << endl;
		// 0 or a negative response indicates a failure
		} else {
			cout << "Failed to receive a response from the server.";
			return "Failed to receive a response from the server.";
		}
		
	} catch (...) {
		// There was an exception that the user should be informed of
		std::cerr << "There was an issue trying to receive the server's response." << endl;
		return "There was an issue trying to receive the server's response.";
	}
	
	string str(res_from_server);
	return str;
	
}

bool authUser(int sock) {
	
	// Prompt the user to authenticate
	std::cout << "\r\nYou must authenticate before accessing the vault\r\n";
	
	
	// User enters username
	std::cout << "Username: ";
	cin >> usr;
	
	
	// Get the authentication request
	string message = auth_request(usr);
   
   
   // send the message to the server
	sendMsg(sock, message);
	
	// Checking that the user exists in the vault
	std::cout << "\n\nChecking whether user " << usr << " exists in the vault server\n\n";
	std::cout << "Message to vault server:\n" << genMsg(message);
	
	
	
   
   // declare a charaacter array with 10,000 vacant characters
	string str = receiveMsg(sock);
   
    // receive the message from the server
   if (str.find("Authenticated") != std::string::npos) {
   		return true;
   	} else {
   		return false;
   	}
   
}

int main() {

	// setting connection information
    int portAddress = 7734;
    char serverName[256];
    gethostname(serverName, 256);
    
    // Attempt message
    std::cout << "Attempting to connect to host " << serverName << " on port " << portAddress << endl;
    

	// create the client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Check whether the socket was successfully created
    if (clientSocket < 0) {
    	std::cerr << "Failed to open the socket" << endl;
    	return -1;
    	std::cout << "Successfully opened a socket" << endl;
    } else {
    	std::cout << "Successfully opened a socket" << endl;
    }
    
    // Create the socket address which will be used to connect to the server
    struct sockaddr_in serv_addr;
    
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portAddress);
    
    // Attempt to convert the server address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    	std::cerr << "Invalid address" << endl;
    } else {
    	std::cout << "Successfully converted server address from text to binary" << endl;
    }
	
	// attempt to connect to the server 
    if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
    	// check the server address
    	cout <<  (struct sockaddr *)&serv_addr << endl;
    	
    	
    	// print that we are now connected
    	cout << "The server(" << serverName << ") and the port(" << portAddress << ") are now connected" << endl;
    } else { 
    	cout << "The server(" << serverName << ") and the port(" << portAddress << ") FAILED to connect" << endl; 
    	return 1;
    }
	
	// get the hostname
    string cl_hostName = inet_ntoa(serv_addr.sin_addr);
    
    // print the hostname 
    std::cout << "Using this client hostname: " << cl_hostName << endl;
    
    // set the client port number
    int cl_port_number;
    cout << "What port number would you like to listen to request on between 49152-65535: ";
    cin >> cl_port_number;
    
    // Only allow port numbers between 49152 and 65535
    while (cl_port_number < 49152 || cl_port_number > 65535) {
        cout << "Please reenter port number between 49152-65535: ";
        cin >> cl_port_number;
    }
	
	
	// split into a new process
    pid_t pid = fork();
    if (pid == 0) {
        exit(0);
    }
    
    // authenticate the user
    bool authenticated = false;
    while (!authenticated) {
    	authenticated = authUser(clientSocket);
    }

    bool isTrue = true;
    while (isTrue) {
    
    	// Prompt the user to decide between the 6 options
        cout << "\n\nEnter the behavior you want to operate" << endl;
        cout << "1. Show Personal Favorite Dog\n2. Show All Favorite Dogs \n3. Set a Different Favorite Dog\n4. Add a new user to the Vault\n5. Save Changes\nOperation:" << endl;
        
        // Accept the user's input
        int user_input;
        cin >> user_input;
        
        // Request sent to the server
        string msg;
		
		switch(user_input) {
			// If the user chooses to see their personal favorite dog
			case 1: {
				// send the message
				sendMsg(clientSocket, genMsg("Attempting to Show User " + usr + "'s Favorite Dog"));
				
				// listen for a response
				receiveMsg(clientSocket);
				
				break;
			}
			// The user wants to show all favorite dogs
			case 2: {

				
				// Send the message
				sendMsg(clientSocket, genMsg("Attempting to Show All User's Favorite Dog"));
				
				// listen for a response
				receiveMsg(clientSocket);
				
				break;
			}
			// The user wants to set a new favorite dog for their user
			case 3: {
				// Prompt for new favorite dog
				string newDog;
				cout << "Please enter your new favorite dog: ";
				cin >> newDog;
				
				// Send the message
				sendMsg(clientSocket, genMsg("Attempting to Set a new Favorite Dog " + newDog + " for User " + usr));
				
				
				// Receive the message
				receiveMsg(clientSocket);
				
				break;
			}
			// The user wants to add a new user
			case 4: {
				// declaring the new user's username string
				string newUser;
				// declaring the new user's favorite dog
				string dog;
				
				// prompting the user for the new user's username
				cout << "Please enter the new user's username: ";
				// receiving the new user's username
				cin >> newUser;
				
				
				// prompting the user for the new user's username
				cout << "Please enter the favorite dog of new user " << newUser << ": ";
				// receiving the new user's username
				cin >> dog;
				
				// Send the message
				sendMsg(clientSocket, genMsg("Attempting to Add a new User " + newUser + " with favorite dog " + dog + "."));
				
				// Listen for the response
				receiveMsg(clientSocket);
				
				break;
			}
			// The user wants to save changes
			case 5: {
				// Send the message
				sendMsg(clientSocket, genMsg("Persistently save changes."));
				
				// Listen for the response
				receiveMsg(clientSocket);
			}
			default: {
			
				cout << "Please enter valid operation input (1-4)\n\n";		
			}
		}
    }
    return 0;
}
