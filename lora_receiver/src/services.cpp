#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include "constants.h"

void Lora_connection()
{
    Serial.println("===========> LORA <============");
    Serial.println("Frecuencia: 915 MHz");

    // Configuración de pines LoRa para TTGO T-Beam
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);

    // Inicializar LoRa en 915 MHz
    if (!LoRa.begin(FREQUENCY))
    {
        Serial.println("Error iniciando LoRa!");
        while (1)
            ;
    }

    Serial.println("LoRa Receptor listo!");
    Serial.println();
}

void WiFi_connection()
{
    Serial.println("===========> WIFI <============");
    Serial.println("Connecting to " + String(SSID));
    WiFi.begin(SSID, PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
        digitalWrite(LED, !digitalRead(LED));
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED, HIGH);
    }
    else
    {
        Serial.println("\nError conectando WiFi!");
        digitalWrite(LED, LOW);
    }

    Serial.println();
}