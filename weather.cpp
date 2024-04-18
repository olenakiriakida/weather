#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <cstring>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

string getWeatherForecast(const string& city) 
{
    string url = "sinoptik.ua";
    string get_http = "GET /погода-" + city + " HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        cout << "WSAStartup failed.\n";
        return "";
    }

    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (Socket == INVALID_SOCKET) 
    {
        cout << "Error creating socket.\n";
        WSACleanup();
        return "";
    }

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(url.c_str(), "http", &hints, &result);

    if (iResult != 0) 
    {
        cout << "getaddrinfo failed: " << iResult << "\n";
        closesocket(Socket);
        WSACleanup();
        return "";
    }

    iResult = connect(Socket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) 
    {
        cout << "Unable to connect to server.\n";
        freeaddrinfo(result);
        closesocket(Socket);
        WSACleanup();
        return "";
    }

    iResult = send(Socket, get_http.c_str(), static_cast<int>(get_http.length()), 0);

    if (iResult == SOCKET_ERROR) 
    {
        cout << "Error sending HTTP request.\n";
        closesocket(Socket);
        WSACleanup();
        return "";
    }

    char buffer[4096];
    string website_HTML;

    do {

        iResult = recv(Socket, buffer, 4096, 0);
        if (iResult > 0) 
        {
            website_HTML.append(buffer, iResult);
        }

        else if (iResult == 0) 
        {
            cout << "Connection closed by server.\n";
        }

        else 
        {
            cout << "recv failed: " << WSAGetLastError() << "\n";
        }

    } while (iResult > 0);

    closesocket(Socket);
    WSACleanup();

    return website_HTML;
}

int main() 
{
    string cities[] = { "Kyiv", "Lviv", "Odesa", "Kharkiv", "Dnipro", "Zaporizhzhia", "Vinnytsia", "Ivano-frankivsk", "Lutsk", "Chernivtsi" };

    for (const string& city : cities) 
    {
        cout << "Weather forecast for " << city << ":\n";
        string weatherHTML = getWeatherForecast(city);
       
        cout << "-----------------------\n";
    }

    cout << "\nPress any key to close.\n";
    cin.ignore();
    cin.get();

    return 0;
}