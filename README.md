# PI

Descoberta do IP de cada raspberry:
O PC de Controle envia uma mensagem de descoberta ("DESCOBERTA") para o endereço de broadcast na porta de descoberta (PORTA_DESCOBERTA).
Cada Raspberry Pi configurado para escutar na porta de descoberta receberá essa mensagem e responderá com seu endereço IP atual.
