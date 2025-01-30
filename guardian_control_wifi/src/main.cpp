#include <Arduino.h>

#include <ESP32Servo.h>
#include <IcsHardSerialClass.h>
#include <WiFi.h>

const char ssid[] = "ESP32AP-TEST";
const char pass[] = "12345678";
const IPAddress ip(192, 168, 123, 45);
const IPAddress subnet(255, 255, 255, 0);

const char html[] =
    "<!DOCTYPE html><html lang='ja'><head><meta charset='UTF-8'>\
<style>input {margin:8px;width:80px;}\
div {font-size:16pt;color:red;text-align:center;width:400px;border:groove 40px orange;}</style>\
<title>WiFi Guardian Controller</title></head>\
<body><div><p>Guardian Controller</p>\
<form method='get'>\
<input type='submit' name='le' value='左前' />\
<input type='submit' name='fo' value='前' />\
<input type='submit' name='ri' value='右前' /><br>\
<input type='submit' name='st' value='停止' /><br>\
<input type='submit' name='bl' value='左後' />\
<input type='submit' name='ba' value='後ろ' />\
<input type='submit' name='br' value='右後' /><br><br>\
<input type='submit' name='rr' value='右旋回' />\
<input type='submit' name='rl' value='左旋回' /><br><br>\
<input type='submit' name='rh' value='頭右回転' />\
<input type='submit' name='lh' value='頭左回転' /><br><br>\
<input type='submit' name='w2' value='歩行mode2(前進のみ)' /><br><br>\
<input type='submit' name='ws' value='歩行ゆっくり(前進のみ)' /><br><br>\
</form></div></body></html>";

WiFiServer server(80);

const byte EN_PIN0 = 2;
const byte EN_PIN1 = 4;
const byte EN_PIN2 = 18;
const long BAUDRATE = 1250000;
const int TIMEOUT = 1000;
const int D_TIME = 20;
const int HOKAN = 30;
const int DELAY_TIME = 200;
int pos[3][6];
const int hpos[3][6] = {{0, 0, 0, 0, 0, 0},
                        {-1000, -500, -1200, -1000, -1000, -400},
                        {0, 0, 0, 0, 0, 0}}; // home position
int dpos[3][6];

IcsHardSerialClass krs0(&Serial, EN_PIN0, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs2(&Serial2, EN_PIN2, BAUDRATE, TIMEOUT);

void stop();
void setLegPos(int id, int dpos0, int dpos1, int dpos2);
void setLegPosLinear(int id, int dpos0, int dpos1, int dpos2, int d_time,
                     int hokan);
void linearControl(int dpos0, int dpos1, int dpos2, int dpos3, int dpos4,
                   int dpos5, int dpos6, int dpos7, int dpos8, int dpos9,
                   int dpos10, int dpos11, int dpos12, int dpos13, int dpos14,
                   int dpos15, int dpos16, int dpos17, int d_time, int hokan);
void walk1Control(int id);
void rotateControl(int id);

Servo head_yaw;
#define HEAD_YAW_PIN 33

void setup() {
  Serial.begin(1250000);

  WiFi.softAP(ssid, pass);
  delay(100);
  WiFi.softAPConfig(ip, ip, subnet);

  IPAddress myIP = WiFi.softAPIP();

  pinMode(5, OUTPUT); // set the LED pin mode
  delay(10);

  server.begin();

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("Server start!");

  digitalWrite(5, HIGH);

  krs0.begin();
  // krs1.begin();
  krs2.begin();

  head_yaw.attach(HEAD_YAW_PIN);

  setLegPos(0, -3500, 3000, 0);
  setLegPos(1, -3500, 3000, 0);
  setLegPos(2, -3500, 3000, 0);
  setLegPos(3, -3500, 3000, 0);
  setLegPos(4, -3500, 3000, 0);
  setLegPos(5, -3500, 3000, 0);
  delay(1000);
  linearControl(-3500, -3500, -3500, -3500, -3500, -3500, 3000, 3000, 3000,
                3000, 3000, 3000, 400, 400, 400, 400, 400, 400, D_TIME, HOKAN);
  delay(1000);
  linearControl(0, 0, 0, 0, 0, 0, 3000, 3000, 3000, 3000, 3000, 3000, -400,
                -400, -400, -400, -00, -400, D_TIME, HOKAN);
  delay(1000);
  linearControl(-3500, -3500, -3500, -3500, -3500, -3500, 3000, 3000, 3000,
                3000, 3000, 3000, 0, 0, 0, 0, 0, 0, D_TIME, HOKAN);
  // Serial.println("start");
  delay(5000); // 5秒待つ
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    Serial.println("New Client.");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print(html);
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /?fo")) {
          walk1Control(0);
        }
        if (currentLine.endsWith("GET /?le")) {
          walk1Control(5);
        }
        if (currentLine.endsWith("GET /?ri")) {
          walk1Control(1);
        }
        if (currentLine.endsWith("GET /?ba")) {
          walk1Control(3);
        }
        if (currentLine.endsWith("GET /?bl")) {
          walk1Control(4);
        }
        if (currentLine.endsWith("GET /?br")) {
          walk1Control(2);
        }
        if (currentLine.endsWith("GET /?rl")) {
          rotateControl(0);
        }
        if (currentLine.endsWith("GET /?rr")) {
          rotateControl(1);
        }
        if (currentLine.endsWith("GET /?ws")) {
          stop();
          for (int i = 0; i < 3; i++) {
            // 雑歩行3
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000,
                          1500, 3000, 1500, 3000, -400, 400, -400, -400, 400,
                          -400, 40, 60);
            delay(1000); // 1秒待つ
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000,
                          1500, 3000, 1500, 3000, 400, -400, 400, 400, -400,
                          400, 40, 60);
            delay(1000); // 1秒待つ
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500,
                          3000, 1500, 3000, 1500, 400, -400, 400, 400, -400,
                          400, 40, 60);
            delay(1000); // 1秒待つ
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500,
                          3000, 1500, 3000, 1500, -400, 400, -400, -400, 400,
                          -400, 40, 60);
            delay(1000); // 1秒待つ
          }
        }
        if (currentLine.endsWith("GET /?lh")) {
          head_yaw.write(45);
        }
        if (currentLine.endsWith("GET /?rh")) {
          head_yaw.write(135);
        }
        if (currentLine.endsWith("GET /?w2")) {
          stop();
          for (int i = 0; i < 3; i++) {
            // 雑歩行3
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000,
                          1500, 3000, 1500, 3000, -400, 400, -400, -400, 400,
                          -400, 20, 30);
            delay(200); // 1秒待つ
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000,
                          1500, 3000, 1500, 3000, 400, -400, 400, 400, -400,
                          400, 20, 30);
            delay(200); // 1秒待つ
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500,
                          3000, 1500, 3000, 1500, 400, -400, 400, 400, -400,
                          400, 20, 30);
            delay(200); // 1秒待つ
            linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500,
                          3000, 1500, 3000, 1500, -400, 400, -400, -400, 400,
                          -400, 20, 30);
            delay(200); // 1秒待つ
          }
        }
        if (currentLine.endsWith("GET /?st")) {
          stop();
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void stop() {
  // setLegPos(0, -3500, 3000, 0);
  // setLegPos(1, -3500, 3000, 0);
  // setLegPos(2, -3500, 3000, 0);
  // setLegPos(3, -3500, 3000, 0);
  // setLegPos(4, -3500, 3000, 0);
  // setLegPos(5, -3500, 3000, 0);
  head_yaw.write(90);
  linearControl(-3500, -3500, -3500, -3500, -3500, -3500, 3000, 3000, 3000,
                3000, 3000, 3000, 0, 0, 0, 0, 0, 0, D_TIME, HOKAN);
  delay(DELAY_TIME);
}

// 脚1本を動かす
void setLegPos(
    int id, int dpos0, int dpos1,
    int dpos2) { // id: 0-5, の次の脚のservoの位置を規定する,
                 // 最終的には組み合わせる or この中身を線形補間実装にする
  krs0.setPos(id, 7500 + hpos[0][id] + dpos0);
  // delay(100);
  // krs1.setPos(id, 7500+hpos[1][id]+dpos1);
  krs0.setPos(6 + id, 7500 + hpos[1][id] + dpos1);
  // delay(100);
  krs2.setPos(id, 7500 + hpos[2][id] + dpos2);
  // delay(100);
}

void setLegPosLinear(int id, int dpos0, int dpos1, int dpos2, int d_time,
                     int hokan) {
  int tpos[3];
  dpos[0][id] = dpos0;
  dpos[1][id] = dpos1;
  dpos[2][id] = dpos2;

  tpos[0] = 7500 + hpos[0][id] + dpos[0][id];
  tpos[1] = 7500 + hpos[1][id] + dpos[1][id];
  tpos[2] = 7500 + hpos[2][id] + dpos[2][id];

  pos[0][id] = krs0.getPos(id);
  Serial.println(pos[0][id]);
  // pos[1][id] = krs1.getPos(id);
  pos[1][id] = krs0.getPos(6 + id);
  Serial.println(pos[1][id]);
  pos[2][id] = krs2.getPos(id);
  Serial.println(pos[2][id]);

  tpos[0] = (tpos[0] - pos[0][id]) / hokan;
  tpos[1] = (tpos[1] - pos[1][id]) / hokan;
  tpos[2] = (tpos[2] - pos[2][id]) / hokan;

  for (int i = 0; i < hokan; i++) {
    krs0.setPos(id, pos[0][id] + tpos[0] * i);
    // krs1.setPos(id, pos[1][id] + tpos[1] * i);
    krs0.setPos(6 + id, pos[1][id] + tpos[1] * i);
    krs2.setPos(id, pos[2][id] + tpos[2] * i);
    delay(d_time);
  }

  // setLegPos(id, dpos0, dpos1, dpos2);
}

// 6本の脚を線形補間で動かす
void linearControl(int dpos0, int dpos1, int dpos2, int dpos3, int dpos4,
                   int dpos5, int dpos6, int dpos7, int dpos8, int dpos9,
                   int dpos10, int dpos11, int dpos12, int dpos13, int dpos14,
                   int dpos15, int dpos16, int dpos17, int d_time, int hokan) {
  int tpos[3][6];

  dpos[0][0] = dpos0;
  dpos[0][1] = dpos1;
  dpos[0][2] = dpos2;
  dpos[0][3] = dpos3;
  dpos[0][4] = dpos4;
  dpos[0][5] = dpos5;
  dpos[1][0] = dpos6;
  dpos[1][1] = dpos7;
  dpos[1][2] = dpos8;
  dpos[1][3] = dpos9;
  dpos[1][4] = dpos10;
  dpos[1][5] = dpos11;
  dpos[2][0] = dpos12;
  dpos[2][1] = dpos13;
  dpos[2][2] = dpos14;
  dpos[2][3] = dpos15;
  dpos[2][4] = dpos16;
  dpos[2][5] = dpos17;

  // 少しずつ足していくtposを求める
  for (int i = 0; i < 6; i++) {
    // tposに目標値を代入
    tpos[0][i] = 7500 + hpos[0][i] + dpos[0][i];
    tpos[1][i] = 7500 + hpos[1][i] + dpos[1][i];
    tpos[2][i] = 7500 + hpos[2][i] + dpos[2][i];

    pos[0][i] = krs0.getPos(i);
    // pos[1][i] = krs1.getPos(i);
    pos[1][i] = krs0.getPos(6 + i);
    pos[2][i] = krs2.getPos(i);

    tpos[0][i] = (tpos[0][i] - pos[0][i]) / hokan;
    tpos[1][i] = (tpos[1][i] - pos[1][i]) / hokan;
    tpos[2][i] = (tpos[2][i] - pos[2][i]) / hokan;
  }

  for (int i = 0; i < hokan; i++) {
    for (int j = 0; j < 6; j++) {

      krs0.setPos(j, pos[0][j] + tpos[0][j] * i);
      // krs1.setPos(j, pos[1][j]);
      krs0.setPos(6 + j, pos[1][j] + tpos[1][j] * i);
      krs2.setPos(j, pos[2][j] + tpos[2][j] * i);
    }
    delay(d_time);
  }
}

void walk1Control(int id) {
  stop();

  // 歩容行列
  int s1_w[6] = {0, -3500, 0, -3500, 0, -3500};
  int s1_p[6] = {0, 3000, 0, 3000, 0, 3000};
  int s1_y[6] = {-400, 400, -400, -400, 400, -400};
  int s2_w[6] = {0, -3500, 0, -3500, 0, -3500};
  int s2_p[6] = {0, 3000, 0, 3000, 0, 3000};
  int s2_y[6] = {400, -400, 400, 400, -400, 400};
  int s3_w[6] = {-3500, 0, -3500, 0, -3500, 0};
  int s3_p[6] = {3000, 0, 3000, 0, 3000, 0};
  int s3_y[6] = {400, -400, 400, 400, -400, 400};
  int s4_w[6] = {-3500, 0, -3500, 0, -3500, 0};
  int s4_p[6] = {3000, 0, 3000, 0, 3000, 0};
  int s4_y[6] = {-400, 400, -400, -400, 400, -400};
  int s1_up_p[6] = {0, 3000, 0, 3000, 0, 3000};
  int s3_up_p[6] = {3000, 0, 3000, 0, 3000, 0};

  int n0 = (0 - id) % 6;
  int n1 = (1 - id) % 6;
  int n2 = (2 - id) % 6;
  int n3 = (3 - id) % 6;
  int n4 = (4 - id) % 6;
  int n5 = (5 - id) % 6;

  for (int i = 0; i < 3; ++i) { // 5回繰り返す
    linearControl(0, 0, 0, 0, 0, 0, s1_up_p[n0], s1_up_p[n1], s1_up_p[n2],
                  s1_up_p[n3], s1_up_p[n4], s1_up_p[n5], s4_y[n0], s4_y[n1],
                  s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5], D_TIME, HOKAN);
    linearControl(s1_w[n0], s1_w[n1], s1_w[n2], s1_w[n3], s1_w[n4], s1_w[n5],
                  s1_p[n0], s1_p[n1], s1_p[n2], s1_p[n3], s1_p[n4], s1_p[n5],
                  s1_y[n0], s1_y[n1], s1_y[n2], s1_y[n3], s1_y[n4], s1_y[n5],
                  D_TIME, HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
    linearControl(s2_w[n0], s2_w[n1], s2_w[n2], s2_w[n3], s2_w[n4], s2_w[n5],
                  s2_p[n0], s2_p[n1], s2_p[n2], s2_p[n3], s2_p[n4], s2_p[n5],
                  s2_y[n0], s2_y[n1], s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5],
                  D_TIME, HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
    linearControl(0, 0, 0, 0, 0, 0, s3_up_p[n0], s3_up_p[n1], s3_up_p[n2],
                  s3_up_p[n3], s3_up_p[n4], s3_up_p[n5], s2_y[n0], s2_y[n1],
                  s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5], D_TIME, HOKAN);
    linearControl(s3_w[n0], s3_w[n1], s3_w[n2], s3_w[n3], s3_w[n4], s3_w[n5],
                  s3_p[n0], s3_p[n1], s3_p[n2], s3_p[n3], s3_p[n4], s3_p[n5],
                  s3_y[n0], s3_y[n1], s3_y[n2], s3_y[n3], s3_y[n4], s3_y[n5],
                  D_TIME, HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
    linearControl(s4_w[n0], s4_w[n1], s4_w[n2], s4_w[n3], s4_w[n4], s4_w[n5],
                  s4_p[n0], s4_p[n1], s4_p[n2], s4_p[n3], s4_p[n4], s4_p[n5],
                  s4_y[n0], s4_y[n1], s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5],
                  D_TIME, HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
  }
}

void rotateControl(int id) {
  stop();

  int s1_w[6] = {0, -3500, 0, -3500, 0, -3500};
  int s1_p[6] = {0, 3000, 0, 3000, 0, 3000};
  int s1_y[6] = {-400, 400, -400, 400, -400, 400};
  int s2_w[6] = {0, -3500, 0, -3500, 0, -3500};
  int s2_p[6] = {0, 3000, 0, 3000, 0, 3000};
  int s2_y[6] = {400, -400, 400, -400, 400, -400};
  int s3_w[6] = {-3500, 0, -3500, 0, -3500, 0};
  int s3_p[6] = {3000, 0, 3000, 0, 3000, 0};
  int s3_y[6] = {400, -400, 400, -400, 400, -400};
  int s4_w[6] = {-3500, 0, -3500, 0, -3500, 0};
  int s4_p[6] = {3000, 0, 3000, 0, 3000, 0};
  int s4_y[6] = {-400, 400, -400, 400, -400, 400};
  int s1_up_p[6] = {0, 3000, 0, 3000, 0, 3000};
  int s3_up_p[6] = {3000, 0, 3000, 0, 3000, 0};

  int n0 = (0 - id) % 6;
  int n1 = (1 - id) % 6;
  int n2 = (2 - id) % 6;
  int n3 = (3 - id) % 6;
  int n4 = (4 - id) % 6;
  int n5 = (5 - id) % 6;

  for (int i = 0; i < 1; ++i) {
    linearControl(0, 0, 0, 0, 0, 0, s1_up_p[0], s1_up_p[1], s1_up_p[2],
                  s1_up_p[3], s1_up_p[4], s1_up_p[5], s4_y[n0], s4_y[n1],
                  s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5], D_TIME, HOKAN);
    linearControl(s1_w[0], s1_w[1], s1_w[2], s1_w[3], s1_w[4], s1_w[5], s1_p[0],
                  s1_p[1], s1_p[2], s1_p[3], s1_p[4], s1_p[5], s1_y[n0],
                  s1_y[n1], s1_y[n2], s1_y[n3], s1_y[n4], s1_y[n5], D_TIME,
                  HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
    linearControl(s2_w[0], s2_w[1], s2_w[2], s2_w[3], s2_w[4], s2_w[5], s2_p[0],
                  s2_p[1], s2_p[2], s2_p[3], s2_p[4], s2_p[5], s2_y[n0],
                  s2_y[n1], s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5], D_TIME,
                  HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
    linearControl(0, 0, 0, 0, 0, 0, s3_up_p[0], s3_up_p[1], s3_up_p[2],
                  s3_up_p[3], s3_up_p[4], s3_up_p[5], s2_y[n0], s2_y[n1],
                  s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5], D_TIME, HOKAN);
    linearControl(s3_w[0], s3_w[1], s3_w[2], s3_w[3], s3_w[4], s3_w[5], s3_p[0],
                  s3_p[1], s3_p[2], s3_p[3], s3_p[4], s3_p[5], s3_y[n0],
                  s3_y[n1], s3_y[n2], s3_y[n3], s3_y[n4], s3_y[n5], D_TIME,
                  HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
    linearControl(s4_w[0], s4_w[1], s4_w[2], s4_w[3], s4_w[4], s4_w[5], s4_p[0],
                  s4_p[1], s4_p[2], s4_p[3], s4_p[4], s4_p[5], s4_y[n0],
                  s4_y[n1], s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5], D_TIME,
                  HOKAN);
    delay(DELAY_TIME); // 0.2秒待つ
  }
}