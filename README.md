# IoT Sleep Monitoring System

Sistema de monitoramento de sono baseado em arquitetura IoT, utilizando comunicação via CoAP, persistência em SQLite e visualização em Node-RED.
O projeto foi desenvolvido inicialmente para a disciplina de Internet das Coisas (UNIFESP – 2º semestre de 2025) e posteriormente refatorado com foco em:
- Separação de responsabilidades
- Validação estruturada de dados
- Tratamento adequado de exceções
- Persistência desacoplada da camada de visualização
- Reprodutibilidade do ambiente

# Arquitetura do Sistema

Fluxo de dados:

ESP32 (sensores)
        ↓
CoAP Server (Python)
        ↓
Validação de Payload
        ↓
SQLite (persistência)
        ↓
Node-RED (visualização)

O servidor é responsável por:
- Receber dados via CoAP
- Validar estrutura e tipos do payload
- Persistir dados no banco SQLite
- Gerenciar erros de forma adequada
- O Node-RED é utilizado exclusivamente para visualização dos dados armazenados.

# Componentes de Hardware (opcional)

Para execução com hardware real:
- ESP32
- Arduino (usado para evitar interferências nas GPIOs com WiFi ativo)
- Microfone INMP441
- Sensor DHT11
- Protoboard
- Cabos de conexão

Observação: É possível utilizar apenas um microcontrolador confiável, eliminando a comunicação I2C entre dispositivos.

# Execução do Projeto (modo simulador)

O projeto pode ser executado sem hardware físico utilizando o script simulador.

1️ - Clone o repositório
git clone <url-do-repositorio>
cd Projeto-IoT-Monitor-de-Sono
2️ - Crie e ative um ambiente virtual
python -m venv .venv

Windows:

.venv\Scripts\activate

3️ - Instale as dependências
pip install -r requirements.txt

4️ - Inicie o servidor CoAP
python coap_server/server.py

O banco de dados será inicializado automaticamente na primeira execução.

5️ - (Opcional) Inicie o simulador
python simulator/simulator.py

# Visualização com Node-RED

Inicie o Node-RED

Configure o nó SQLite apontando para:
data/sono.db

Utilize os nós de dashboard para visualizar as leituras