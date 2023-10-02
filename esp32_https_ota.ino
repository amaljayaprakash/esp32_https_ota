#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "root_ca.h"

const char* ssid = "esp";
const char* password = "esp32pass";

String current_FW_VER = { "1.0" };

#define URL_FW_VER "https://raw.githubusercontent.com/amaljayaprakash/esp32_https_ota/main/fw_version.txt"
#define URL_FW_BIN "https://raw.githubusercontent.com/amaljayaprakash/esp32_https_ota/main/firmware_esp.bin"

void Update_Firmware();
int Check_FW_Version();

void setup() {
  Serial.begin(115200);
  Serial.print("Current firmware version:");
  Serial.println(current_FW_VER);

  pinMode(2, OUTPUT);
  Serial.println("Waiting for WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() {
  delay(3000);

  if (Check_FW_Version()) {
    Update_Firmware();
  }
}


void Update_Firmware(void) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  httpUpdate.setLedPin(2, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_FW_BIN);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}
int Check_FW_Version(void) {
  String https_payload;
  int httpCode;
  String ota_url = "";
  ota_url += URL_FW_VER;
  ota_url += "?";
  ota_url += String(rand());

  WiFiClientSecure* client = new WiFiClientSecure;

  if (client) {
    client->setCACert(rootCACertificate);
    HTTPClient https;

    if (https.begin(*client, ota_url)) {
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK)
      {
        https_payload = https.getString();
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK) 
  {
    https_payload.trim();
    if (https_payload.equals(current_FW_VER)) {
      Serial.printf("\nCurrent Firmware version: %s\nNo new updates are there..!!\n", current_FW_VER);
      return 0;
    } else {
      Serial.println(https_payload);
      Serial.println("New firmware is available for update");
      return 1;
    }
  }
  return 0;
}