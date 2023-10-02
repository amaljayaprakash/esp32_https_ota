# ESP32 HTTPS OTA (Over-The-Air) Firmware Update

This README file provides a guide on implementing HTTPS OTA (Over-The-Air) firmware updates for ESP32 devices. HTTPS OTA allows you to update the firmware on your ESP32 microcontroller securely over the internet. This process involves using SSL/TLS encryption to ensure the integrity and confidentiality of the firmware update.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Setting up the HTTPS Server](#setting-up-the-https-server)
- [Preparing the Firmware](#preparing-the-firmware)
- [ESP32 Code](#esp32-code)
- [Uploading Firmware Over HTTPS](#uploading-firmware-over-https)
- [Security Considerations](#security-considerations)
- [Troubleshooting](#troubleshooting)

## Prerequisites

Before implementing HTTPS OTA on your ESP32 device, make sure you have the following:

- An ESP32 microcontroller.
- A computer with the Arduino IDE and ESP32 board support installed.
- Access to a web server with HTTPS capabilities.
- A valid SSL/TLS certificate for your server's domain.

## Setting up the HTTPS Server

1. **Obtain an SSL/TLS certificate:** You need a valid SSL/TLS certificate for your server's domain. You can obtain one from a trusted certificate authority (CA) or use a self-signed certificate for testing purposes (not recommended for production use).

2. **Configure your web server:** Set up your web server to serve the firmware files over HTTPS. Make sure the server is accessible from the ESP32 device.

3. **Organize firmware files:** Store your firmware files on the web server, making sure to have the correct file structure for easy OTA updates.

## Preparing the Firmware

1. **Compile the firmware:** Build the firmware for your ESP32 device using the Arduino IDE or PlatformIO. In arduino IDE goto **Sketch -> Export Compiled Binary**

2. **Generate the firmware binary:** After compiling/generating, locate the firmware binary file (usually a .bin file) in the build directory of your project.

3. **Upload the firmware to the server:** Copy the firmware binary to the directory on your web server where you plan to host firmware updates (in our case github).

## ESP32 Code

Now let's prepare the ESP32 code to enable OTA updates over HTTPS.

1. **Include necessary libraries:** In your Arduino sketch, include the required libraries at the top of your code. The "root_ca.h" file contains the Baltimore rootCA which is required for SSL/TLS AUTH:

```cpp
    #include <WiFi.h>
    #include <HTTPClient.h>
    #include <HTTPUpdate.h>
    #include <WiFiClientSecure.h>
    #include "root_ca.h"
```

2. **Set up Wi-Fi:** Configure your target ESP32 to connect to your Wi-Fi network:

```cpp
   const char* ssid = "YourSSID";
   const char* password = "YourPassword";
```

3. **Define the URLs of firmware file and firmware version file :**

```cpp
    #define URL_FW_VER "https://<your-github-raw-file-location>/fw_version.txt"
    #define URL_FW_BIN "https://<your-github-raw-file-location>/firmware.bin"
```

4. **Update function:**
This function simply downloads the bin file from the url given in **URL_FW_BIN** and will start updating the firmware.

```cpp
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
```

5. **The `setup()` function, connect to Wi-Fi and perform the firmware update:**

```cpp
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
```

6. **The `loop()` function**

In the loop function it waits for 10 Seconds and checks for the update. if there is no new updates then the system continues executing the `loop()` function

 ```cpp
    void loop() {
        delay(10000);

        if (Check_FW_Version()) {
            Update_Firmware();
        }
    }
```

7. **Check_FW_Version() function.**

This function is responsible for comparing the running firmware and the firmware present in the server. It pings the server to read the contents in **fw_version.txt** file and compares it with the running code. if there is a higher version availablke at the server then it initiates the firmwate update.


## Uploading Firmware Over HTTPS

1. **Upload the code:** Upload your ESP32 code to the device using the Arduino IDE or PlatformIO.

2. **Open the Serial Monitor:** Monitor the serial output to check the status of the firmware update process.

3. **Ensure Wi-Fi connectivity:** Make sure the ESP32 is connected to Wi-Fi and can reach the server hosting the firmware update.

4. **Trigger the update:** When the ESP32 runs the code, it should automatically check for and perform the firmware update over HTTPS.

## Security Considerations

- Always use a valid SSL/TLS certificate for your server in production environments to ensure secure communication.
- Implement authentication and authorization mechanisms to prevent unauthorized firmware updates.
- Secure the firmware storage on the server to prevent unauthorized access.

## Troubleshooting

If you encounter issues during HTTPS OTA updates, consider checking the following:

- Wi-Fi connectivity.
- SSL/TLS certificate validity.
- Firmware binary location and accessibility on the server.
- Serial output for debugging information.

By following this guide and considering security best practices, you can implement HTTPS OTA firmware updates for your ESP32 device securely.
