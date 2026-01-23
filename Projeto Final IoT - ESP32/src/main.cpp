#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include "ArduinoJson.h"
#include <driver/i2s.h>
#include <math.h>
#include <Wire.h>

// ===== Wi-Fi =====
const char* ssid     = "nome_da_rede_wifi";
const char* password = "senha_da_rede_wifi";

// ===== CoAP Server =====
IPAddress coapServerIP(0, 0, 0, 0);  // IP do servidor
const int coapServerPort = 5683;
const char* coapResource = "sono";

// ===== Identificação =====
const char* device_id = "sleep_01";

// ===== CoAP =====
WiFiUDP udp;
Coap coap(udp);

// ===== Intervalo =====
unsigned long lastSend = 0;
const unsigned long interval = 5000; // 5 segundos

// ===== Callback =====
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.print("Resposta CoAP: ");
  Serial.write(packet.payload, packet.payloadlen);
  Serial.println();
}

// ===== Microfone =====
// configuração para que o microfone capte o audio creditos (https://github.com/atomic14/esp32-i2s-mic-test)
#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000

#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_32
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_25
#define I2S_MIC_SERIAL_DATA GPIO_NUM_33

int32_t raw_samples[SAMPLE_BUFFER_SIZE];

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};


const float V_FS = (float)pow(2, 31);

// transforma os dados de audio em um parametro útil
float calculate_dbfs(int32_t* samples, int num_samples) {
    // 1. Soma dos Quadrados
    double sum_of_squares = 0.0;
    for (int i = 0; i < num_samples; i++) {
        // Conversão para double para evitar overflow na soma
        sum_of_squares += (double)samples[i] * (double)samples[i];
    }

    // 2. Média Quadrática (RMS)
    if (num_samples == 0) return -120.0; // Evita divisão por zero. -120 dBFS é perto do silêncio absoluto.

    double mean_square = sum_of_squares / num_samples;
    float v_rms = (float)sqrt(mean_square);

    // 3. Conversão para dBFS
    float ratio = v_rms / V_FS;
    
    // Proteção contra log(0)
    if (ratio == 0.0f) {
        return -120.0f; 
    }

    // Fórmula dBFS: 20 * log10(V_RMS / V_FS)
    float dbfs = 20.0f * log10f(ratio);

    return dbfs;
}

// ======= I2C =======

float temperature[10], humidity[10];
int temperature_index = 0, humidity_index = 0, distance_index = 0, send_index = 0;

void reception(int bytesReceived) {
  if(bytesReceived < sizeof(float) + 1) {
    return;
  }
  char dataType = Wire.read(); // Lê o byte de identificação
  float received_data;
  while (Wire.available() >= sizeof(float)) {
    Wire.readBytes((char*)&received_data, sizeof(float));
  }
 
  if(dataType == 'b') {
    temperature[temperature_index] = received_data;
    temperature_index = (temperature_index + 1) % 10;
  } else if(dataType == 'c') {
    humidity[humidity_index] = received_data;
    humidity_index = (humidity_index + 1) % 10;
  } else {
    Serial.println("Tipo de dado desconhecido recebido.");
  }
}

float atualiza_temperatura(){
  float temperatura;
  for(int i = 0; i < 10; i++){
    temperatura += temperature[i];
  }
  return temperatura /= 10;
}
float atualiza_umidade(){
  float umidade;
  for(int i = 0; i < 10; i++){
    umidade += humidity[i];
  }
  return umidade /= 10;
}

void setup() {

  Wire.begin(8);
  Wire.onReceive(reception);
  Serial.begin(115200);
  delay(1000);int32_t raw_samples[SAMPLE_BUFFER_SIZE];

  // Wi-Fi
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // CoAP
  coap.response(callback_response);
  Serial.println("Cliente CoAP iniciado");

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
}

void loop() {
  coap.loop();

  float temperatura = atualiza_temperatura();
  float umidade = atualiza_umidade();

  if (millis() - lastSend >= interval) {
    lastSend = millis();

    size_t bytes_read = 0;
    i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);

    // CÁLCULO dBFS
    float ruido_dbfs = calculate_dbfs(raw_samples, samples_read);

    // Monta JSON
    JsonDocument doc;
    doc["device_id"] = device_id;
    doc["ruido_dbfs"] = ruido_dbfs;
    doc["temperatura"] = temperatura;
    doc["umidade"] = umidade;

    char payload[128];
    serializeJson(doc, payload);


    Serial.print("Enviando CoAP POST: ");
    Serial.println(payload);

    uint16_t msgid = coap.send(
      coapServerIP,              // IP do servidor
      coapServerPort,            // Porta UDP 5683
      coapResource,              // Recurso
      COAP_CON,                  // Confirmável
      COAP_POST,                 // <<< POST REAL (0.02)
      NULL,                      // Token
      0,                         // Token length
      (uint8_t*)payload,         // Payload
      strlen(payload)            // Payload length
    );

    Serial.print("Mensagem enviada, msgid = ");
    Serial.println(msgid);
  }
}

