#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <deque>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <stdio.h>
#include <sstream>

//using namespace std;

// TCP port used to access the server
#define SERVER_PORT 7734
#define BUFFER_SIZE 1024

class Person {
	std::string name;
	std::string favoriteDog;
	
public:
	Person(std::string na, std::string dog) {
		setName(na);
		setDog(dog);
	}

	void setName(std::string nam) {
		name = nam;
	}
	
	std::string getName() {
		return name;
	}
	
	void setDog(std::string dog) {
		favoriteDog = dog;
	}
	
	std::string getDog() {
		return favoriteDog;
	}
	

};




std::vector<Person> people;

void loadUsers() {
	// open users and dogs file
	std::ifstream myfile;
	// open the vault file
	myfile.open("passwd.txt");
	// declare a string for each line
	std::string line;
	
	
	// read each user
	while (std::getline(myfile, line)) {
		// declaring a string for the name
		std::string name;
		// declaring a string for the dog
		std::string dog;
		
		// set a string stream for the line
		std::istringstream iss(line);
		
		// Get the user from the line
		std::getline(iss, name, ',');
		// Get the dog from the line
		std::getline(iss, dog, ',');
		
		// Crate a Person object
		Person myPerson(name, dog);
		
		// Add the Person object to the vector of people
		people.push_back(myPerson);
	}
	
	myfile.close();
	
}

// Generate a consistent response format from the server
std::string serverResponse(std::string msg) {
	// Set the response
	std::string resp = "\r\n*************** Server Response ************************\r\n" +  \
	msg + "\r\n" + "*************** Server Response ************************";
	
	// Return the response string
	return resp;
}

// Send the message - creating this function to save unnecessary repeated code
void sendMsg(int sock, std::string msg) {
	// send the message to the server
	try {
		 // send the message
		 if (send(sock, msg.c_str(), strlen(msg.c_str()), 0) < 0) {
			std::cerr << "Failed to send the message.";
		 } else {
            std::cout << "Successfully sent the message: \n" << msg << std::endl;
    	}
    } catch (...) {
    	std::cout << "There was an issue trying to send this LIST request." << std::endl;
    }
}

// Handle this thread  
void handlePeer(int sock) {
	// declare the buffer
    char buffer[BUFFER_SIZE];
    //declare the amount of bytes read
    int bytesRead;
    // set the user to unauthenticated - cannot proceed until the user is authenticated
    bool isAuth = false;
    // setting the user for this session
    Person usr("null", "null");
    while ((bytesRead = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        // Null-terminate the received data
        buffer[bytesRead] = '\0'; 
        
        // Create a string for the message read from the buffer
        std::string msg(buffer);

        // Add your message parsing and processing logic here
        // Example parsing logic
        std::cout << msg << std::endl;
        
        // Check whether the message starts with the AUTH header
        if (msg.substr(0,4) == "AUTH") {
        
        
        	// Iterate through each person to see if the person exists
        	for (int i=0; i < people.size(); i++) {
        		if (msg.find(people[i].getName()) != std::string::npos) {
        			// send the message
					 if (send(sock, serverResponse("Authenticated").c_str(), strlen(serverResponse("Authenticated").c_str()), 0) < 0) {
					 
					 	// notify the user of the error
						std::cerr << "Failed to send the message.";
					 } else {
					 	// Output the message sent by the server
						std::cout << "Successfully sent the message: \n" << msg << std::endl;
						
						// Update the person object to match the authenticated user
						usr = Person(people[i].getName(), people[i].getDog());
					 }
					 
					 isAuth = true;
        		}
        	}
        	
        	// Send a response even if the user failed to authenticate
        	if (!isAuth) {
        		// send the message
        		sendMsg(sock, serverResponse("Failed to Authenticate"));
        	}
        
        }
        // Check if we are looking for all user's favorite dog
        else if (msg.find("All User's Favorite Dog") != std::string::npos) {
        	
        	// String included in the server's response
        	std::string str;
        	
        	for (int i = 0; i < people.size(); i++) {
        		str.append(people[i].getDog());
        		str.append("\n");
        	}
        	
        	// send the message
        	sendMsg(sock, serverResponse(str));
        }
        // Check if the message is specific to a single user's (possesive) favorite dog
        else if (msg.find("'s Favorite Dog") != std::string::npos) {
        	
        	  // send the message
        	  sendMsg(sock, serverResponse(usr.getDog()));
		// Check if the client wants to set a new favorite
        } else if (msg.find("Set a new Favorite") != std::string::npos) {
        	
        	// Update the new favorite dog
        	std::string dog = msg;
        	dog = dog.substr(msg.find("Favorite Dog ") + 13, msg.find("for User ") - (msg.find("Favorite Dog ") + 13));
        	
        	// Update the list
        	for (int i = 0; i < people.size(); i++) {
        		if (usr.getName() == people[i].getName()) {
        			people[i].setDog(dog);
        		}
        	}
        	
        	// Set the new favorite dog for the current user
        	usr.setDog(dog);
        	
        	// send the message
        	sendMsg(sock, serverResponse("New favorite dog for user " + usr.getName() + " is " + usr.getDog()));
        
        // Check if we want to add a new user
        } else if (msg.find("Add a new User") != std::string::npos) {
        	// Use a substring of the message to get the new UID and corresponding favorite dog
        	std::string newUser = msg.substr(msg.find("User ") + 5, msg.find(" with") - (msg.find("User ") + 5));
        	std::string dog = msg.substr(msg.find("dog ") + 4, msg.find(".") - (msg.find("dog ") + 4));
        	
        	
        	// Create the new Person object
        	Person person(newUser, dog);
        	
        	// Add the new person to the back of the vector
        	people.push_back(person); 
        	
        	// send the message
        	sendMsg(sock, serverResponse("New user " + person.getName() + " with favorite dog " + \
        	person.getDog() + " has been added to the vault."));
        	
        // Check if we want to save the current contents to the file
        } else if (msg.find("save") != std::string::npos) {
        	// Clear contents of the file
        	std::ofstream ofs;
			ofs.open("passwd.txt", std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			
			// Create and open a text file
  			std::ofstream o("passwd.txt");
			for (int i = 0; i < people.size(); i++) {
				o << people[i].getName() << "," << people[i].getDog() << "\n";
			}
			o.close();
			// Send the message informing the user that an invalid message was received
        	sendMsg(sock, serverResponse("Successfully saved changes. Please have your vault admin check the passwd.txt file."));
			
        } else {
        	// Send the message informing the user that an invalid message was received
        	sendMsg(sock, serverResponse("Invalid message."));
        }
        
        
        // Clear the buffer for the next message
        memset(buffer, 0, BUFFER_SIZE);
    }
	
	// Close the socket
    close(sock);
}

int main() {
	// Load users
	loadUsers();


	// create the socket
	
	// param: AF_INET indicates that we are making an IPv4 connection
	// param: SOCK_STREAM indicates that we are opening a TCP socket
	// param: 0 indicates IP as the protocol value
	// return: a non-negative value, a negative value indicates failure to create the socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket <= 0) {
        std::cerr << "Error: Unable to create socket." << std::endl;
        return 1;
    }

	// Declaring the variable for the transport address and port for the AF_INET address
    sockaddr_in serverAddr;
    
    // Setting the objects for the SOCKADDR_IN struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);
	

    // Bind, listen, and accept connections as in your original code
	
	// param: serverSocket - The socket descriptor returned by a previous socket() call.
	// param: einterpret_cast<sockaddr*>(&serverAddr) - pointer to the sockaddr struct
	// param: sizeof(serverAddr) - size of address
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Error: Binding failed." << std::endl;
        close(serverSocket);
        return 1;
    }
	
	// if the stream socket can open to accept a maximum of 100 connections, continue
	// otherwise, exit and throw an errror
    if (listen(serverSocket, 100) < 0) {
        std::cerr << "Error: Listening failed." << std::endl;
        close(serverSocket);
        return 1;
    }
	
	// socket stream is now open to accept connections
    std::cout << "The Server is ready and listening to peers...." << std::endl;

	
	// pre-emptively declare a socket address struct for the client
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
	
	// param: serverSocket - The socket descriptor returned by a previous socket() call.
	// param: einterpret_cast<sockaddr*>(&serverAddr) - pointer to the sockaddr struct
	// param: sizeof(serverAddr) - size of address
    while (true) {
    	// Listening to accept the socket connection from the client
        int connectionSocket =  accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        // Handle a failure to create the socket
        if (connectionSocket < 0) {
            std::cerr << "Error: Unable to accept connection." << std::endl;
            continue;
        }

		// Notify the server admin of the new connection
        std::cout << "\nGot connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;

        // Create a new thread to handle the peer
        std::thread t(handlePeer, connectionSocket);
        t.detach(); // Detach the thread to let it run independently
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
