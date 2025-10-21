

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Comunicacion de la terminal o Begin serial 
#define SERIAL_BAUDRATE 115200

// Configuración para TTGO T-Beam
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18
#define LORA_RST 14
#define LORA_DIO0 26


// LED interno del TTGO T-Beam
#define LED 13

// url del servidor para enviar datos (si es necesario)
#define SERVER_URL "http://example.com/receive_data"
#define SERVER_URL "192.168.0.198:5000/data"

// Configuracion de la red WIF
#define WIFI_SSID_UPB "UPBWiFi"
#define WIFI_SSID "IoT_network_for_maintenance"
#define PASSWORD "7t8CAaMPk1FIW3pl4Ukx8DAYypKRO2"


#endif // CONSTANTS_H
