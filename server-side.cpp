#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <thread>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

void HandleClient(SOCKET clientsocket) {
    char buffer[1024] = {};

    while (true) {
        int bytesreceived = recv(clientsocket, buffer, sizeof(buffer), 0);
        if (bytesreceived == SOCKET_ERROR) {
            std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
            closesocket(clientsocket);
            return;
        }
        if (bytesreceived == 0) {
            std::cerr << "Client disconnected." << std::endl;
            closesocket(clientsocket);
            return;
        }
        std::cout << "Client: " << buffer <<  std::endl; // You can customize the name to whatever you want by replacing 'Client' to your desired name, this is used to show the output message that the client has sent
        
    }
}

int main() {
    WSADATA wsadata;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (iResult != 0) {
        std::cerr << "WSA Did not startup properly" << std::endl;
        return 1;
    }

    SOCKET listenconnection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenconnection == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in localaddr = {};
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //This is the ip address that is running the server code, in order to be able to chat with another computer you will need to replace this with your local IPv4 Address 
    localaddr.sin_port = htons(8080); //This is the port that this service will be running on, if you plan to communicate to another computer connected to the same network as you, change this to an unoccupied port number such as '4444'

    if (bind(listenconnection, (sockaddr*)&localaddr, static_cast<int>(sizeof(localaddr))) == SOCKET_ERROR) {
        std::cerr << "Error binding socket: " << WSAGetLastError() << std::endl;
        closesocket(listenconnection);
        WSACleanup();
        return 1;
    }

    if (listen(listenconnection, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error listening on socket" << std::endl;
        closesocket(listenconnection);
        WSACleanup();
        return 1;
    }

    std::cout << "Server Started, waiting for connections" << std::endl;

    while (true) {
        SOCKET clientsocket = accept(listenconnection, NULL, NULL);
        if (clientsocket == INVALID_SOCKET) {
            std::cerr << "Error accepting socket" << std::endl;
            continue; // Continue listening for new connections
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientsocket, clientIP, INET_ADDRSTRLEN);
        std::cout << "Client Connected: " << clientIP << std::endl;

        std::thread(HandleClient, clientsocket).detach();
        
        // Allow the server to send messages to the client
        while (true) {
            std::string message;
            std::cout << "Server: "; //You can customize this to whatever you want, this is the prompt that will show, to let you know that you can type a message
            std::getline(std::cin, message);
            int bytessent = send(clientsocket, message.c_str(), message.size(), 0);
            if (bytessent == SOCKET_ERROR) {
                std::cerr << "Error sending data: " << WSAGetLastError() << std::endl;
                closesocket(clientsocket);
                return 1;
            }
            
        }
    }

    closesocket(listenconnection);
    WSACleanup();

    return 0;
}
