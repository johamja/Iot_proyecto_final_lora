

#ifdef CONSTANTS_H
#define CONSTANTS_H

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

#endif // CONSTANTS_H
