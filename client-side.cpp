#include <WinSock2.h>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <vector>
#include <string>
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

void receiveMessages(SOCKET sock) {
    const int BUFFER_SIZE = 1024;
    while (true) {
        char buffer[1024] = {};
        int bytesreceived = recv(sock, buffer, sizeof(buffer), 0);

        if (bytesreceived == SOCKET_ERROR) {
            std::cout << "Error receiving data: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(1); // Exit the program if there's an error receiving data.
        }

        std::cout << "Server: " << buffer << std::endl; // You can customize the name to whatever you want by replacing 'Server' to your desired name, this is used to show the output message that the server has sent
    }
}

int main() {
    WSADATA wsadata;
    int Iresult = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (Iresult != 0) {
        std::cerr << "WSAStartup Failed" << Iresult;
        system("pause");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serveraddr = {};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // This needs to be the same Ip address as the one set in the server-side code in order for the program to work
    serveraddr.sin_port = htons(8080); // This needs to be the same Port as the one set in the server-side code in order for the program to work.

    if (connect(sock, (sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
        std::cerr << "error connecting to server: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Welcome To The Server." << std::endl;

    std::thread receiveThread(receiveMessages, sock);

    while (true) {
        std::string p;
        
        std::getline(std::cin, p);
        const char* message = p.c_str();
        int bytessent = send(sock, message, strlen(message), 0);

        if (bytessent == SOCKET_ERROR) {
            std::cerr << "Error sending data: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }
    }

    closesocket(sock);
    WSACleanup();
    system("pause");
    return 0;
}
