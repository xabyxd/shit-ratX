import socket
import sys

HOST = "0.0.0.0"
PORT = 4444

while True:
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    print(f"[+] Esperando conexiones en {HOST}:{PORT}...")

    client_socket, client_address = server_socket.accept()
    print(f"[+] Conexión recibida de {client_address[0]}:{client_address[1]}")

    try:
        while True:
            command = input("\n> ")
            if not command.strip():
                continue
            client_socket.send(command.encode())

            full_response = ""
            while True:
                chunk = client_socket.recv(4096).decode(errors="ignore")
                if not chunk:
                    print("[-] Conexión cerrada por el cliente.")
                    raise ConnectionError
                full_response += chunk
                if "[END]" in full_response:
                    full_response = full_response.replace("[END]", "")
                    break
            
            print(full_response)
            sys.stdout.flush()
    except:
        print("[-] Cliente desconectado. Esperando nueva conexión...")
        client_socket.close()
