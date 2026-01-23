# Projeto-IoT-Monitor-de-Sono
Um pequeno projeto feito para disciplina de IoT cursada na UNIFESP (segundo semestre de 2025)

Para testar o projeto serão necessários os seguintes componentes:
. 1 - Arduino
. 1 - ESP32
. 1 - Microfone INMP441 (com conexão configurada para funcionar como lado esquerdo)
. 1 - DHT11
. 1 - Protoboard
. 1 - Fios para conexão dos componentes
. Node-RED com Sqlite instalado em um computador
. Python

A forma de fazer a ligação dos componentes pode ser facilmente encontrada pela internet, apenas certifique-se de que eles estão conectados as GPIOs corretas de acordo com o código dos microcontroladorese.
O Arduino é utilizado pois, quando o ESP32 está com o WiFi ligado alguns problemas podem ser experenciados nas leituras das GPIOs. Caso possua um microcontrolodaor confiável todos o sensores podem ser utilizados
junto a ele, eliminando assim a necessidade de utilizar a comunicação i2c entre microcontroladores.
