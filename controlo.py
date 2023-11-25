import socket
import sys

def enviar_comando(ip_destino, porta_destino, comando):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((ip_destino, porta_destino))
        s.sendall(comando.encode())

# Exemplo de uso para definir o Raspberry Pi como transmissor
ip_transmissor = "IP_DO_TRANSMISSOR"  # Substitua pelo IP real do Raspberry Pi Transmissor
porta_transmissor = 12345
enviar_comando(ip_transmissor, porta_transmissor, "definir_transmissor")

# Exemplo de uso para definir o Raspberry Pi como recetor
ip_recetor = "IP_DO_RECETOR"  # Substitua pelo IP real do Raspberry Pi Recetor
porta_recetor = 12345
enviar_comando(ip_recetor, porta_recetor, "definir_recetor")
