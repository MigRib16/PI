import subprocess
import paramiko

def executar_raspberry(ip, funcao, *variaveis):
    comando = f"ssh -t {ip} './main {funcao} {' '.join(variaveis)}'"
    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(ip, username='pi')  # Substitua 'pi' pelo nome de utilizador correto

        stdin, stdout, stderr = ssh.exec_command(comando, get_pty=True)
        saida = stdout.read().decode('utf-8')

        print(f"Saida do Raspberry ({ip}):")
        print(saida)

        ssh.close()
    except Exception as e:
        print(f"Erro ao executar no Raspberry ({ip}):")
        print(e)

def obter_input_usuario():
    escolha = input("Digite '1' para o Raspberry ser o transmissor, '2' para ser o recetor: ")
    if escolha == '1':
        return 'transmissor'
    elif escolha == '2':
        return 'recetor'
    else:
        print("Escolha inválida. Tente novamente.")
        return obter_input_usuario()

# Substitua com os IPs reais dos seus raspberrys
ip_raspberry_1 = "192.168.1.2"
ip_raspberry_2 = "192.168.1.3"

# Determinar a função de cada raspberry
funcao_raspberry_1 = obter_input_usuario()
funcao_raspberry_2 = 'recetor' if funcao_raspberry_1 == 'transmissor' else 'transmissor'

# Especificar função e variáveis para cada raspberry
executar_raspberry(ip_raspberry_1, funcao_raspberry_1, "variavel1", "variavel2")
executar_raspberry(ip_raspberry_2, funcao_raspberry_2)
     #A variável "SEU_USUARIO" deve ser substituída pelo nome de usuário 
    # que você usa para se conectar aos Raspberry Pis usando o SSH. Se, por exemplo, 
    # o nome de usuário que você usa para se conectar aos Raspberry Pis é "pi", então você substituiria "SEU_USUARIO" 
    # por "pi" no comando SSH.