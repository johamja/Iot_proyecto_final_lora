#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "repository.h"
#include "config.h"

/*
JsonDocument Submit_an_incident(const Incident &incident) {
    preferences.begin("my-app", false);
    const String url_pref = preferences.getString("url");
    const String url = url_pref + "/go_rutines";
    Serial.println("\n[HTTP] [Submit_an_incident] begin... " + url);

    HTTPClient http;
    http.setReuse(true);
    http.setTimeout(60000);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Crear el payload JSON usando el struct
    JsonDocument payloadDoc;
    payloadDoc["id"] = incident.id;
    payloadDoc["action"] = incident.action;

    String payload;
    serializeJson(payloadDoc, payload);

    Serial.println("[HTTP] [Submit_an_incident] POST...");
    Serial.println("[HTTP] [Submit_an_incident] Payload: " + payload);

    int httpCode = http.POST(payload);

    Serial.println("[HTTP] [Submit_an_incident] CODE : " + String(httpCode));

    if (httpCode > 0) {
        Serial.printf("[HTTP] [Submit_an_incident] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
            String response = http.getString();
            Serial.println("[HTTP] [Submit_an_incident] Response received:");
            Serial.println(response);

            // Parsear la respuesta a JSON
            JsonDocument responseDoc;
            DeserializationError error = deserializeJson(responseDoc, response);

            if (!error) {
                http.end();
                return responseDoc;
            }
            Serial.println("[HTTP] [Submit_an_incident] Error parsing JSON response");
        }
    } else {
        Serial.printf("[HTTP] [Submit_an_incident] POST... failed, error: %s\n",
                      HTTPClient::errorToString(httpCode).c_str());
    }

    http.end(); // Cerramos el servicio
    JsonDocument emptyDoc;
    return emptyDoc; // Retorna un JSON vacío si algo falló
}
*/