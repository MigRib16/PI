import socket
import subprocess

PORTA_COMUNICACAO = 12345
PORTA_DESCOBERTA = 12347

def atribuir_funcao(ip_raspberry, funcao):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((ip_raspberry, PORTA_COMUNICACAO))
            s.sendall(funcao.encode())
            print(f"Atribuída a função '{funcao}' para o Raspberry Pi em {ip_raspberry}")
        except Exception as e:
            print(f"Erro ao conectar e atribuir função ao Raspberry Pi em {ip_raspberry}: {str(e)}")

def descobrir_raspberrys():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.settimeout(5)

        try:
            s.sendto(b"DESCOBERTA", ('<broadcast>', PORTA_DESCOBERTA))
            data, addr = s.recvfrom(1024)
            ip_raspberry = data.decode()
            print(f"Raspberry Pi descoberto em {ip_raspberry}")
            return ip_raspberry
        except socket.timeout:
            print("Nenhum Raspberry Pi encontrado.")
            return None

# Descobrir os Raspberry Pis na rede local
ip_rpi1 = descobrir_raspberrys()
ip_rpi2 = descobrir_raspberrys()

# Atribuir funções aos Raspberry Pis
atribuir_funcao(ip_rpi1, "recetor")
atribuir_funcao(ip_rpi2, "transmissor")

# Iniciar programa remotamente nos Raspberry Pis via SSH
def iniciar_programa_remotamente(ip_raspberry, funcao):
    try:
        subprocess.run(["ssh", ip_raspberry, f"./caminho/do/seu/programa/programa {funcao}"])
    except Exception as e:
        print(f"Erro ao iniciar programa remotamente no Raspberry Pi em {ip_raspberry}: {str(e)}")

# Iniciar o programa remotamente para cada Raspberry Pi
iniciar_programa_remotamente(ip_rpi1, "recetor")
iniciar_programa_remotamente(ip_rpi2, "transmissor")