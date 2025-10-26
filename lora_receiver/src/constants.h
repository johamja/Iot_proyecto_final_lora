#ifndef CONSTANTS_H
#define CONSTANTS_H

// Comunicacion de la terminal o Begin serial 
#define SERIAL_BAUDRATE 115200

// Configuracion lora
#define RADIO_CS_PIN 18
#define RADIO_RST_PIN 23
#define RADIO_DIO0_PIN 26
#define FREQUENCY 915E6

// Configuración para TTGO T-Beam
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18

// Configuracion del WIFI
#define SSID "UPBWiFi"
#define PASSWORD ""

// LED interno del TTGO T-Beam
#define LED 13


#endif // CONSTANTS_H
