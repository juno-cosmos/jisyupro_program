#include <Arduino.h>

// ######################## servo example ########################

// #include <IcsHardSerialClass.h>

// const byte EN_PIN0 = 2;
// const byte EN_PIN1 = 4;
// const byte EN_PIN2 = 18;
// const long BAUDRATE = 1250000;
// const int TIMEOUT = 1000;
// const int hpos[3][6] = {{7500, 7500, 7500, 7500, 7500, 7500}, {6600, 6900, 6700, 6600, 6500, 7300}, {7500, 7500, 7500, 7500, 7500, 7500}}; // home position


// IcsHardSerialClass krs0(&Serial, EN_PIN0, BAUDRATE, TIMEOUT);
// IcsHardSerialClass krs1(&Serial1, EN_PIN1, BAUDRATE, TIMEOUT);
// IcsHardSerialClass krs2(&Serial2, EN_PIN2, BAUDRATE, TIMEOUT);

// void setLegPos(int id, int dpos0, int dpos1, int dpos2) { // id: 0-5, の次の脚のservoの位置を規定する, 最終的には組み合わせる or この中身を線形補間実装にする
//   krs0.setPos(id, hpos[0][id] + dpos0);
//   // delay(100);
//   krs1.setPos(id, hpos[1][id] + dpos1);
//   // delay(100);
//   krs2.setPos(id, hpos[2][id] + dpos2);
//   // delay(100);
// }

// void setLegPosLinear(int id, int dpos0, int dpos1, int dpos2) { // 線形補間実装
// }

// void setup() {
//   krs0.begin();
//   krs1.begin();
//   krs2.begin();
//   setLegPos(0, 0, 2500, 0);
//   setLegPos(1, 0, 2500, 0);
//   setLegPos(2, 0, 2500, 0);
//   setLegPos(3, 0, 2500, 0);
//   setLegPos(4, 0, 2500, 0);
//   setLegPos(5, 0, 2500, 0);
//   delay(5000); //5秒待つ
//   setLegPos(0, -4000, 1500, 0);
//   setLegPos(1, -4000, 1500, 0);
//   setLegPos(2, -4000, 1500, 0);
//   setLegPos(3, -4000, 1500, 0);
//   setLegPos(4, -4000, 1500, 0);
//   setLegPos(5, -4000, 1500, 0);
//   delay(5000); //5秒待つ
// }

// void loop() {
//   // // 雑歩行フィードフォワード, id0先頭の場合
//   // // 長: 4000, 0 短: -4000, 1000 上: 0, 2000
//   // // N: 0 左: -700, 右: 700
//   // // Step1
//   // // 0: 長N, 1: 上右, 2: 短左, 3 上N, 4: 短右, 5: 上左 
//   // setLegPos(0, 4000, 0, 0);
//   // setLegPos(1, 0, 2000, 700);
//   // setLegPos(2, -4000, 1000, -700);
//   // setLegPos(3, 0, 2000, 0);
//   // setLegPos(4, -4000, 1000, 700);
//   // setLegPos(5, 0, 2000, -700);
//   // delay(1000); //1秒待つ

//   // // Step2
//   // // 0: 短N, 1: 上左, 2: 長右, 3 上N, 4: 長左, 5: 上右
//   // setLegPos(0, -4000, 1000, 0);
//   // setLegPos(1, 0, 2000, -700);
//   // setLegPos(2, 4000, 0, 700);
//   // setLegPos(3, 0, 2000, 0);
//   // setLegPos(4, 4000, 0, -700);
//   // setLegPos(5, 0, 2000, 700);
//   // delay(1000); //1秒待つ
  
//   // // Step3
//   // // 0: 上N, 1: 長左, 2: 上右, 3 短N, 4: 上左, 5: 長右
//   // setLegPos(0, 0, 2000, 0);
//   // setLegPos(1, 4000, 0, -700);
//   // setLegPos(2, 0, 2000, 700);
//   // setLegPos(3, -4000, 1000, 0);
//   // setLegPos(4, 0, 2000, -700);
//   // setLegPos(5, 4000, 0, 700);
//   // delay(1000); //1秒待つ
  
//   // // Step4
//   // // 0: 上N, 1: 短右, 2: 上左, 3 長N, 4: 上右, 5: 短左
//   // setLegPos(0, 0, 2000, 0);
//   // setLegPos(1, -4000, 1000, 700);
//   // setLegPos(2, 0, 2000, -700);
//   // setLegPos(3, 4000, 0, 0);
//   // setLegPos(4, 0, 2000, 700);
//   // setLegPos(5, -4000, 1000, -700);
//   // delay(1000); //1秒待つ

//   // setLegPos(3, 3800, 200, 0);
//   // delay(2000);
//   // setLegPos(3, -3800, 1500, 0);
//   // delay(5000);

  
// }

// ######################## bluetooth mac address example ########################

// void setup(void) {
//   Serial.begin(115200);
//   uint8_t bt_mac[6];
//   esp_read_mac(bt_mac, ESP_MAC_BT);
//   Serial.printf("Bluetooth Mac Address => %02X:%02X:%02X:%02X:%02X:%02X\r\n", bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4], bt_mac[5]);
// }
 
// void loop() {
// }

// ######################## bluetooth serial example ########################

// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Evandro Copercini - 2018
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// and also demonstrate that SerialBT have the same functionalities of a normal Serial
// Note: Pairing is authenticated automatically by this device

// #include "BluetoothSerial.h"

// String device_name = "ESP32-BT-Slave";

// // Check if Bluetooth is available
// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// // Check Serial Port Profile
// #if !defined(CONFIG_BT_SPP_ENABLED)
// #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
// #endif

// BluetoothSerial SerialBT;

// void setup() {
//   Serial.begin(115200);
//   SerialBT.begin(device_name);  //Bluetooth device name
//   //SerialBT.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
//   Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
// }

// void loop() {
//   if (Serial.available()) {
//     SerialBT.write(Serial.read());
//   }
//   if (SerialBT.available()) {
//     Serial.write(SerialBT.read());
//   }
//   delay(20);
// }

/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32
31.01.2017 by Jan Hendrik Berlin

 */

// ######################## simple wifi server example ########################

#include <WiFi.h>

const char *ssid = "cosmos";
const char *password = "j809vvbao12a";

WiFiServer server(80); // NetworkServerという名前の型は存在しないので、WiFiServerにする必要がある

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);  // set the LED pin mode

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.accept();  // listen for incoming clients, NetworkClientという名前の型は存在しないので、WiFiClientにする必要がある

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
     
            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

// ######################## simple wifi client example 2 ########################

// #include "WiFi.h"

// // WiFi credentials.
// const char* WIFI_SSID = "cosmos";
// const char* WIFI_PASS = "j809vvbao12a";

// void setup()
// {
//     Serial.begin(115200);
//     // Giving it a little time because the serial monitor doesn't
//     // immediately attach. Want the firmware that's running to
//     // appear on each upload.
//     delay(2000);

//     Serial.println();
//     Serial.println("Running Firmware.");

//     // Connect to Wifi.
//     Serial.println();
//     Serial.println();
//     Serial.print("Connecting to ");
//     Serial.println(WIFI_SSID);

//     // Set WiFi to station mode and disconnect from an AP if it was previously connected
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect();
//     delay(100);

//     WiFi.begin(WIFI_SSID, WIFI_PASS);
//     Serial.println("Connecting...");

//     while (WiFi.status() != WL_CONNECTED) {
//       // Check to see if connecting failed.
//       // This is due to incorrect credentials
//       if (WiFi.status() == WL_CONNECT_FAILED) {
//         Serial.println("Failed to connect to WIFI. Please verify credentials: ");
//         Serial.println();
//         Serial.print("SSID: ");
//         Serial.println(WIFI_SSID);
//         Serial.print("Password: ");
//         Serial.println(WIFI_PASS);
//         Serial.println();
//       }
//       delay(5000);
//     }

//     Serial.println("");
//     Serial.println("WiFi connected");
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());

//     Serial.println("Hello World, I'm connected to the internets!!");
// }

// void loop()
// {
//     delay(5000);
//     Serial.println("Hello World");
// }