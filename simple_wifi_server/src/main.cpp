#include <Arduino.h>
#include "WiFi.h"
#include <IcsHardSerialClass.h>

const char *ssid = "*****";
const char *password = "*******";

WiFiServer server(80);

const byte EN_PIN0 = 2;
const byte EN_PIN1 = 4;
const byte EN_PIN2 = 18;
const long BAUDRATE = 1250000;
const int TIMEOUT = 1000;

IcsHardSerialClass krs0(&Serial, EN_PIN0, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs1(&Serial1, EN_PIN1, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs2(&Serial2, EN_PIN2, BAUDRATE, TIMEOUT);

void setup() {
  Serial.begin(1250000);
  pinMode(5, OUTPUT);  // set the LED pin mode
  digitalWrite(5, LOW);

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

  digitalWrite(5, HIGH);

  krs0.begin();
  krs1.begin();
  krs2.begin();
  for (int i = 0; i < 6; i++) {
    krs0.setPos(i, 7500);
    krs1.setPos(i, 7500);
    krs2.setPos(i, 7500);
  }
}

void loop() {
  WiFiClient client = server.accept();  // listen for incoming clients

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
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the Servos 'HIGH'.<br>"); // TODO いずれボタンにする
            client.print("Click <a href=\"/L\">here</a> to turn the Servos 'LOW'.<br>"); // TODO いずれボタンにする

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
          // digitalWrite(5, HIGH);  // GET /H turns the LED on
          // for (int i=0; i<6; i++) {
          //   krs1.setPos(i, 6700);
          //   krs0.setPos(i, 11500);
          // }
          krs1.setPos(0, 6700);
          krs0.setPos(0, 11500);
        }
        if (currentLine.endsWith("GET /L")) {
          // digitalWrite(5, LOW);  // GET /L turns the LED off
          // digitalWrite(5, HIGH);  // GET /H turns the LED on
          // for (int i=0; i<6; i++) {
          //   krs1.setPos(i, 9000);
          //   krs0.setPos(i, 3500);
          // }
          krs1.setPos(0, 9500);
          krs0.setPos(0, 3500);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}