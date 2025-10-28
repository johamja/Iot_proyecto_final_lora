#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.h"
#include "services.h"
#include "repository.h"

int state = 1;

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);
    pinMode(LED, OUTPUT);

    Lora_connection();
    WiFi_connection();

    // Check if the subscription is active
    while (true)
    {
        DynamicJsonDocument sucriptions(4096);
        sucriptions = Get_subscriptions();
        if (!sucriptions.is<JsonArray>())
        {
            if (Has_description_and_type(sucriptions, SUSCRIPTION_DESCRIPTION, SUSCRIPTION_TYPE))
            {
                Serial.println("[GOOD] Description and type found");
                break;
            }
            else
            {
                Serial.println("[BAD] The condition (description+type) was not met");
                bool subscription = Post_subscription(SUSCRIPTION_DESCRIPTION, SUSCRIPTION_TYPE, URL_SERVER_NOTIFICATION_SUBS);
                if (subscription)
                {
                    Serial.println("[GOOD] The subscription was created");
                    break;
                }
            }
        }
        else
        {
            Serial.println("[BAD] Unable to get subscriptions, sleeping for 3 seconds");
            sleep(3000);
        }
    }

    Serial.println("[OK] FIWARE LoRa Receiver Ready - Waiting for data...");
}

void loop()
{
    DynamicJsonDocument orion_data_new(2048);

    switch (state)
    {
    case 1: // RECEIVE - CHIRP - Esperar datos LoRa del receptor
    {
        int packetSize = LoRa.parsePacket();

        if (packetSize)
        {
            // Read message
            String message = "";
            while (LoRa.available())
            {
                message += (char)LoRa.read();
            }

            int rssi = LoRa.packetRssi();
            long error_hz = LoRa.packetFrequencyError();
            float snr = LoRa.packetSnr();

            Serial.println("=== DATO RECIBIDO ===");
            Serial.printf("Mensaje: %s", message);
            Serial.printf("RSSI: %s/dBm \t\t Packet Frequency Error: %d/Hz \t\t SNR: %f/dB\n", rssi, error_hz, snr);

            // Create JSON
            orion_data_new = Create_orion_package(message, rssi, snr, error_hz);
            state = 2;
        }
        else
        {
            Serial.println("XXX XXX XXX Data no recivido XXX XXX XXX");
        }

        break;
    }


    case 2:
    {

        Serial.println("[INFO] Sending data to FIWARE Orion...");

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi desconectado - Reconectando...");
            WiFi_connection();
            delay(1000);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            bool success = Patch_entity_attrs(ID, orion_data_new);
            if (success)
            {
                Serial.println("[GOOD] Data successfully sent to FIWARE");
                state = 3;
            }
            else
            {
                Serial.println("[BAD] Failed to send to FIWARE");
                Serial.println("[INFO] Create the entity in Orion");

                // Campos base de la entidad
                orion_data_new["id"] = ID;     // e.g. "sensor2"
                orion_data_new["type"] = TYPE; // e.g. "receiver"
                success = Post_new_entity(orion_data_new);

                if (success)
                {
                    Serial.println("[GOOD] Data successfully sent to FIWARE and a new entity was created");
                    state = 3;
                }
                else
                {
                    Serial.println("[BAD] Unable to create entity in Orion");
                    delay(1000);
                }
            }
        }
        else
        {
            Serial.println("[BAD] Could not connect to WiFi");
        }

        break;
    }


    case 3: // WAIT - Breve espera
    {
        Serial.println("[OK] Waiting 2 seconds...");
        delay(2000);
        state = 1;
        break;
    }

    
    default:
    {
        Serial.println("[BAD] Unrecognized State - Restarting");
        state = 1;
        break;
    }
    }
}