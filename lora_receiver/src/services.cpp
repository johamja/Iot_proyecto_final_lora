#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
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


DynamicJsonDocument Create_orion_package(String message, int rssi, float snr, int error_hz)
{
    // El mensaje llega como fragmento de atributos (sin llaves). Lo envolvemos.
    String wrapped = "{" + message + "}";

    DynamicJsonDocument msgDoc(4096);
    DeserializationError err = deserializeJson(msgDoc, wrapped);
    if (err) {
        Serial.print("[Create_orion_package] Error parseando atributos: ");
        Serial.println(err.c_str());
    }

    // Documento de salida en formato NGSIv2 (como sucription.json)
    DynamicJsonDocument outDoc(8192);

    // Atributos fijos
    {
        JsonObject model = outDoc.createNestedObject("model");
        model["value"] = MODEL;    // e.g. "ttgo-t-beam"
        model["type"]  = "Text";
    }
    {
        JsonObject project = outDoc.createNestedObject("project");
        project["value"] = PROJECT; // e.g. "Iot-2025-02"
        project["type"]  = "Text";
    }

    // Copiar atributos del mensaje (latitude, longitude, temperature, humidity)
    // Se asume que el mensaje ya viene con estructura { "value","type","metadata":{...} }
    if (msgDoc.is<JsonObject>()) {
        JsonObject in = msgDoc.as<JsonObject>();

        auto copyIfPresent = [&](const char* key) {
            if (in.containsKey(key) && in[key].is<JsonObject>()) {
                JsonObject dest = outDoc.createNestedObject(key);
                dest.set(in[key]); // copia profunda del objeto atributo
            }
        };

        copyIfPresent("latitude");
        copyIfPresent("longitude");
        copyIfPresent("temperature");
        copyIfPresent("humidity");
    }

    // Atributos LoRa y timestamp (con metadatos, siguiendo sucription.json)
    {
        JsonObject rssiObj = outDoc.createNestedObject("lora_received_power");
        rssiObj["value"] = rssi;
        rssiObj["type"]  = "Integer";
        JsonObject rssiMeta = rssiObj.createNestedObject("metadata");
        JsonObject rssiUnitCode = rssiMeta.createNestedObject("unitCode");
        rssiUnitCode["value"] = "DBM";
        rssiUnitCode["type"]  = "Text";
        JsonObject rssiUnit = rssiMeta.createNestedObject("unit");
        rssiUnit["value"] = "decibel_milliwatt";
        rssiUnit["type"]  = "Text";
    }
    {
        JsonObject snrObj = outDoc.createNestedObject("lora_signal_quality");
        snrObj["value"] = (int)snr; // si prefieres Float, cambia "type" y castea
        snrObj["type"]  = "Integer";
        JsonObject snrMeta = snrObj.createNestedObject("metadata");
        JsonObject snrUnitCode = snrMeta.createNestedObject("unitCode");
        snrUnitCode["value"] = "DB";
        snrUnitCode["type"]  = "Text";
        JsonObject snrUnit = snrMeta.createNestedObject("unit");
        snrUnit["value"] = "decibel";
        snrUnit["type"]  = "Text";
    }
    {
        JsonObject ferrObj = outDoc.createNestedObject("lora_frequency_error");
        ferrObj["value"] = error_hz;
        ferrObj["type"]  = "Integer";
        JsonObject ferrMeta = ferrObj.createNestedObject("metadata");
        JsonObject ferrUnitCode = ferrMeta.createNestedObject("unitCode");
        ferrUnitCode["value"] = "HTZ";
        ferrUnitCode["type"]  = "Text";
        JsonObject ferrUnit = ferrMeta.createNestedObject("unit");
        ferrUnit["value"] = "hertz";
        ferrUnit["type"]  = "Text";
    }
    {
        JsonObject tsObj = outDoc.createNestedObject("timestamp_received");
        tsObj["value"] = (int)millis();
        tsObj["type"]  = "Integer";
        JsonObject tsMeta = tsObj.createNestedObject("metadata");
        JsonObject tsUnitCode = tsMeta.createNestedObject("unitCode");
        tsUnitCode["value"] = "MS";
        tsUnitCode["type"]  = "Text";
        JsonObject tsUnit = tsMeta.createNestedObject("unit");
        tsUnit["value"] = "milliseconds";
        tsUnit["type"]  = "Text";
    }

    // Log opcional
    String preview;
    serializeJson(outDoc, preview);
    Serial.println("[Create_orion_package] Payload NGSIv2:");
    Serial.println(preview);

    return outDoc;
}


bool Has_description_and_type(const DynamicJsonDocument &doc, const char *wanted_description, const char *wanted_type)
{
    // En ArduinoJson v7 usa tipos "Const" cuando el documento es const
    if (!doc.is<JsonArray>()) return false;

    JsonArrayConst arr = doc.as<JsonArrayConst>();
    bool foundDescription = false;
    bool foundType = false;

    for (JsonVariantConst v : arr) {
        if (!v.is<JsonObject>()) continue;
        JsonObjectConst sub = v.as<JsonObjectConst>();

        // Validar description
        if (!foundDescription && sub["description"].is<const char*>()) {
            const char* desc = sub["description"].as<const char*>();
            if (desc && strcmp(desc, wanted_description) == 0) {
                foundDescription = true;
            }
        }

        // Validar subject.entities[].type
        if (!foundType) {
            JsonObjectConst subject = sub["subject"].as<JsonObjectConst>();
            if (!subject.isNull()) {
                JsonArrayConst entities = subject["entities"].as<JsonArrayConst>();
                if (!entities.isNull()) {
                    for (JsonVariantConst ev : entities) {
                        JsonObjectConst ent = ev.as<JsonObjectConst>();
                        if (!ent.isNull() && ent["type"].is<const char*>()) {
                            const char* type = ent["type"].as<const char*>();
                            if (type && strcmp(type, wanted_type) == 0) {
                                foundType = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (foundDescription && foundType) return true;
    }

    return false;
}