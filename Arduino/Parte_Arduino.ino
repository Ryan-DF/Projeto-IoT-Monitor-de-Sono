#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>

#define DHTPIN 11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

unsigned long interval = 100, timer = millis(), previous_time = 0;

void treansmit_temperature(float temperature){
    Wire.beginTransmission(8);
    byte temperature_bytes[sizeof(temperature)];
    memcpy(temperature_bytes, &temperature, sizeof(temperature));
    Wire.write('b'); // envia um byte de identificação
    Wire.write(temperature_bytes, sizeof(temperature)); // envia os bytes do float
    Wire.endTransmission();
}

void transmit_humidity(float humidity){
    Wire.beginTransmission(8);
    byte humidity_bytes[sizeof(humidity)];
    memcpy(humidity_bytes, &humidity, sizeof(humidity));
    Wire.write('c'); // envia um byte de identificação
    Wire.write(humidity_bytes, sizeof(humidity)); // envia os bytes do float
    Wire.endTransmission();
}

void setup(){
  Wire.begin();
  Serial.begin(115200);
  dht.begin();

  Serial.println("SERIAL INICIADA");
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}



void loop(){
  timer = millis();
  if(timer >= previous_time + interval){

    float temperature = dht.readTemperature();
    treansmit_temperature(temperature);
    
    float humidity = dht.readHumidity();
    transmit_humidity(humidity);
    
    previous_time = millis();
  }
}