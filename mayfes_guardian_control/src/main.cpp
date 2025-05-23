#include <Arduino.h>

#include <ESP32Servo.h>
#include <IcsHardSerialClass.h>
#include <WiFi.h>

const char ssid[] = "GUARDIAN-AP";
const char pass[] = "12345678";
const IPAddress ip(192, 168, 123, 45);
const IPAddress subnet(255, 255, 255, 0);

const char html[] = R"(
<!DOCTYPE html>
<html lang='ja'>
<head>
  <meta charset='UTF-8' />
  <title>Guardian Controller</title>
  <style>
    body {
      font-family: sans-serif;
      background: #f9f9f9;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 20px;
      font-weight: bold;
    }

    .top-section {
      display: flex;
      justify-content: space-between;
      align-items: flex-start;
      width: 80%;
      margin-bottom: 30px;
    }

    .head, .locomotion {
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .guardian-area {
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .guardian {
      font-size: 60px;
      border: 2px solid black;
      padding: 20px 60px;
      border-radius: 20px;
      text-align: center;
      margin-bottom: 15px;
      font-weight: bold;
    }

    .speed-section {
      display: flex;
      gap: 10px;
    }

    .speed-section form input {
      width: 80px;
      height: 50px;
      background: #ccc;
      border: 2px solid #888;
      border-radius: 5px;
      font-size: 14px;
      font-weight: bold;
    }

    .dpad {
      display: grid;
      grid-template-areas:
        '. up .'
        'left center right'
        '. down .';
      gap: 5px;
    }

    .dpad form input {
      width: 100px;
      height: 60px;
      background: #ddd;
      border: 1px solid #999;
      border-radius: 5px;
      font-size: 14px;
      font-weight: bold;
    }

    .dpad .center {
      opacity: 0;
      pointer-events: none;
    }

    .locomotion-circle {
      display: grid;
      grid-template-columns: repeat(3, 80px);
      grid-template-rows: repeat(3, 80px);
      gap: 5px;
    }

    .locomotion-circle form input {
      background: #ddd;
      border: 1px solid #999;
      border-radius: 50%;
      width: 80px;
      height: 80px;
      font-size: 18px;
      font-weight: bold;
    }

    .debug-section, .task-section {
      width: 80%;
      margin-top: 30px;
    }

    .debug-title, .task-title {
      margin-bottom: 10px;
    }

    .debug-buttons, .task-buttons {
      display: flex;
      gap: 10px;
    }

    .debug-buttons form input,
    .task-buttons form input {
      width: 70px;
      height: 70px;
      background: #eee;
      border: 2px solid #aaa;
      border-radius: 5px;
      font-size: 14px;
      font-weight: bold;
    }
  </style>
</head>
<body>

  <div class='top-section'>
    <div class='head'>
      <div>Head</div>
      <div class='dpad'>
        <!-- <form style='grid-area: up;' method='get'><input type='submit' name='w2' value='歩行mode2' /></form> -->
        <form style='grid-area: left;' method='get'><input type='submit' name='lh' value='rotateL' /></form>
        <!-- <div class='center' style='grid-area: center;'></div> -->
        <form style='grid-area: center;' method='get'><input type='submit' name='sh' value='rotateStop' /></form>
        <form style='grid-area: right;' method='get'><input type='submit' name='rh' value='rotateR' /></form>
        <!-- <form style='grid-area: down;' method='get'><input type='submit' name='ws' value='歩行ゆっくり' /></form> -->
      </div>
    </div>

    <div class='guardian-area'>
      <div class='guardian'>
        Guardian
      </div>
      <div class='speed-section'>
        <form method='get'><input type='submit' name='ms' value='slow' /></form>
        <form method='get'><input type='submit' name='mf' value='fast' /></form>
      </div>
    </div>

    <div class='locomotion'>
      <div>Locomotion</div>
      <div class='locomotion-circle'>
        <form method='get'><input type='submit' name='le' value='↖' /></form>
        <form method='get'><input type='submit' name='fo' value='↑' /></form>
        <form method='get'><input type='submit' name='ri' value='↗' /></form>
        <form method='get'><input type='submit' name='rl' value='←' /></form>
        <form method='get'><input type='submit' name='st' value='||' /></form>
        <form method='get'><input type='submit' name='rr' value='→' /></form>
        <form method='get'><input type='submit' name='bl' value='↙' /></form>
        <form method='get'><input type='submit' name='ba' value='↓' /></form>
        <form method='get'><input type='submit' name='br' value='↘' /></form>
      </div>
    </div>
  </div>

  <div class='task-section'>
      <div class='task-title'>Task</div>
    <div class='task-buttons'>
      <form method='get'><input type='submit' name='t1' value='Dance' /></form>
      <form method='get'><input type='submit' name='t2' value='LEDON' /></form>
      <form method='get'><input type='submit' name='t3' value='LEDOFF' /></form>
    </div>
  </div>

  <div class='debug-section'>
    <div class='debug-title'>Debug</div>
    <div class='debug-buttons'>
      <form method='get'><input type='submit' name='l0' value='leg0' /></form>
      <form method='get'><input type='submit' name='l1' value='leg1' /></form>
      <form method='get'><input type='submit' name='l2' value='leg2' /></form>
      <form method='get'><input type='submit' name='l3' value='leg3' /></form>
      <form method='get'><input type='submit' name='l4' value='leg4' /></form>
      <form method='get'><input type='submit' name='l5' value='leg5' /></form>
    </div>
  </div>

  </body>
</html>
)";

WiFiServer server(80);

const byte EN_PIN0 = 2;
const byte EN_PIN1 = 4;
const byte EN_PIN2 = 18;
const long BAUDRATE = 1250000;
const int TIMEOUT = 1000;
int D_TIME = 20; // fast mode 10 / slow mode 20
int HOKAN = 30; // fast mode 30 / slow mode 30
int DELAY_TIME = 200; // fast mode 100 / slow mode 200
int pos[3][6];
const int hpos[3][6] = {{0, 0, 0, 0, 0, 0},
                        {-1000, -500, -1200, -1000, -1000, -400},
                        {0, 0, 0, 0, 0, 0}}; // home position
int dpos[3][6];

IcsHardSerialClass krs0(&Serial, EN_PIN0, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs2(&Serial2, EN_PIN2, BAUDRATE, TIMEOUT);

void setLegPos(int id, int dpos0, int dpos1, int dpos2);
void setLegPosLinear(int id, int dpos0, int dpos1, int dpos2, int d_time,
                     int hokan);
void linearControl(int dpos0, int dpos1, int dpos2, int dpos3, int dpos4,
                   int dpos5, int dpos6, int dpos7, int dpos8, int dpos9,
                   int dpos10, int dpos11, int dpos12, int dpos13, int dpos14,
                   int dpos15, int dpos16, int dpos17, int d_time, int hokan);
void legStop();
void walk1Control(int id);
void walk2Control(int id);
void rotateControl(int id); // 0:右回転, 1:左回転
void legWave();
void checkLeg(int id);
void headTask(void *pvParameters);
void legTask(void *pvParameters);
void activateTask();

Servo head_yaw;
int head_speed = 45; // fast mode 80 / slow mode 45
#define HEAD_YAW_PIN 33

// global変数でcontrol_stateとwalk_stateを管理
int leg_state = 0;     // 0:停止, 1~6が方向, 7~8が回転, 9~14がcheckLeg, 15がwave
int head_state = 0;    // 0:停止, 1~2が左右
int walk_state = 1;    // 1~4が歩容の各状態

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

  krs0.begin();
  // krs1.begin();
  krs2.begin();
  head_yaw.attach(HEAD_YAW_PIN);

  activateTask();
  delay(1000);

   // タスク生成
  xTaskCreatePinnedToCore(headTask, "HeadTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(legTask, "LegTask", 4096, NULL, 1, NULL, 1);
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
          leg_state = 1;
        }
        if (currentLine.endsWith("GET /?le")) {
          leg_state = 6;
        }
        if (currentLine.endsWith("GET /?ri")) {
          leg_state = 2;
        }
        if (currentLine.endsWith("GET /?ba")) {
          leg_state = 4;
        }
        if (currentLine.endsWith("GET /?bl")) {
          leg_state = 5;
        }
        if (currentLine.endsWith("GET /?br")) {
          leg_state = 3;
        }
        if (currentLine.endsWith("GET /?rl")) {
          leg_state = 8;
        }
        if (currentLine.endsWith("GET /?rr")) {
          leg_state = 7;
        }
        if (currentLine.endsWith("GET /?ms")) {
          head_speed = 45;
          D_TIME = 20;
          HOKAN = 30;
          DELAY_TIME = 200;
        }
        if (currentLine.endsWith("GET /?mf")) {
          head_speed = 80;
          D_TIME = 10;
          HOKAN = 20;
          DELAY_TIME = 100;
        }
        if (currentLine.endsWith("GET /?lh")) {
          head_state = 1;
        }
        if (currentLine.endsWith("GET /?rh")) {
          head_state = 2;
        }
        if (currentLine.endsWith("GET /?sh")) {
          head_state = 0;
        }
        if (currentLine.endsWith("GET /?l0")) {
          leg_state = 9;
        }
        if (currentLine.endsWith("GET /?l1")) {
          leg_state = 10;
        }
        if (currentLine.endsWith("GET /?l2")) {
          leg_state = 11;
        }
        if (currentLine.endsWith("GET /?l3")) {
          leg_state = 12;
        }
        if (currentLine.endsWith("GET /?l4")) {
          leg_state = 13;
        }
        if (currentLine.endsWith("GET /?l5")) {
          leg_state = 14;
        }
        if (currentLine.endsWith("GET /?st")) {
          leg_state = 0;
        }
        if (currentLine.endsWith("GET /?t1")) {
          // TaskParty
          leg_state = 15;
          head_state = 1;
        }
        if (currentLine.endsWith("GET /?t2")) {
          // TaskParty
          digitalWrite(5, HIGH);
        }
        if (currentLine.endsWith("GET /?t3")) {
          // TaskParty
          digitalWrite(5, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

// void stop() {
//   linearControl(-3500, -3500, -3500, -3500, -3500, -3500, 3000, 3000, 3000,
//                 3000, 3000, 3000, 0, 0, 0, 0, 0, 0, D_TIME, HOKAN);
//   delay(DELAY_TIME);
// }

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

void activateTask() {
  digitalWrite(5, HIGH); // LED点灯
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
  head_yaw.write(120); // head_yawが回転
  // Serial.println("start");
  delay(1000);        // 1秒待つ
  head_yaw.write(90); // 停止
}

void walk1Control(int id) {
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
  
  int n0 = ((0 - id) % 6 + 6) % 6;
  int n1 = ((1 - id) % 6 + 6) % 6;
  int n2 = ((2 - id) % 6 + 6) % 6;
  int n3 = ((3 - id) % 6 + 6) % 6;
  int n4 = ((4 - id) % 6 + 6) % 6;
  int n5 = ((5 - id) % 6 + 6) % 6;

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

void walk2Control(int id) {
  // 歩容行列
  int s1_w[6] = {-4000, -4000, -4000, -4000, -4000, -4000};
  int s1_p[6] = {1500, 3000, 1500, 3000, 1500, 3000};
  int s1_y[6] = {-400, 400, -400, -400, 400, -400};
  int s2_w[6] = {-4000, -4000, -4000, -4000, -4000, -4000};
  int s2_p[6] = {1500, 3000, 1500, 3000, 1500, 3000};
  int s2_y[6] = {400, -400, 400, 400, -400, 400};
  int s3_w[6] = {-4000, -4000, -4000, -4000, -4000, -4000};
  int s3_p[6] = {3000, 1500, 3000, 1500, 3000, 1500};
  int s3_y[6] = {400, -400, 400, 400, -400, 400};
  int s4_w[6] = {-4000, -4000, -4000, -4000, -4000, -4000};
  int s4_p[6] = {3000, 1500, 3000, 1500, 3000, 1500};
  int s4_y[6] = {-400, 400, -400, -400, 400, -400};

  int n0 = ((0 - id) % 6 + 6) % 6;
  int n1 = ((1 - id) % 6 + 6) % 6;
  int n2 = ((2 - id) % 6 + 6) % 6;
  int n3 = ((3 - id) % 6 + 6) % 6;
  int n4 = ((4 - id) % 6 + 6) % 6;
  int n5 = ((5 - id) % 6 + 6) % 6;

  if (walk_state == 1) {
    linearControl(s1_w[n0], s1_w[n1], s1_w[n2], s1_w[n3], s1_w[n4], s1_w[n5],
                  s1_p[n0], s1_p[n1], s1_p[n2], s1_p[n3], s1_p[n4], s1_p[n5],
                  s1_y[n0], s1_y[n1], s1_y[n2], s1_y[n3], s1_y[n4], s1_y[n5],
                  D_TIME, HOKAN);
                  walk_state = 2;
  } else if (walk_state == 2) {  
    linearControl(s2_w[n0], s2_w[n1], s2_w[n2], s2_w[n3], s2_w[n4], s2_w[n5],
                  s2_p[n0], s2_p[n1], s2_p[n2], s2_p[n3], s2_p[n4], s2_p[n5],
                  s2_y[n0], s2_y[n1], s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5],
                  D_TIME, HOKAN);
                  walk_state = 3;
  } else if (walk_state == 3) {
    linearControl(s3_w[n0], s3_w[n1], s3_w[n2], s3_w[n3], s3_w[n4], s3_w[n5],
                  s3_p[n0], s3_p[n1], s3_p[n2], s3_p[n3], s3_p[n4], s3_p[n5],
                  s3_y[n0], s3_y[n1], s3_y[n2], s3_y[n3], s3_y[n4], s3_y[n5],
                  D_TIME, HOKAN);
                  walk_state = 4;
  } else if (walk_state == 4) {
    linearControl(s4_w[n0], s4_w[n1], s4_w[n2], s4_w[n3], s4_w[n4], s4_w[n5],
                  s4_p[n0], s4_p[n1], s4_p[n2], s4_p[n3], s4_p[n4], s4_p[n5],
                  s4_y[n0], s4_y[n1], s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5],
                  D_TIME, HOKAN);
                  walk_state = 1;
  } else {
    walk_state = 1;
  }
  delay(DELAY_TIME);
}

void legStop() {
  linearControl(-3500, -3500, -3500, -3500, -3500, -3500, 3000, 3000, 3000,
                3000, 3000, 3000, 0, 0, 0, 0, 0, 0, D_TIME, HOKAN);
}

void rotateControl(int id) {

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

  if (walk_state == 1) {
    linearControl(0, 0, 0, 0, 0, 0, s1_up_p[0], s1_up_p[1], s1_up_p[2],
                  s1_up_p[3], s1_up_p[4], s1_up_p[5], s4_y[n0], s4_y[n1],
                  s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5], D_TIME, HOKAN);
    linearControl(s1_w[0], s1_w[1], s1_w[2], s1_w[3], s1_w[4], s1_w[5], s1_p[0],
                  s1_p[1], s1_p[2], s1_p[3], s1_p[4], s1_p[5], s1_y[n0],
                  s1_y[n1], s1_y[n2], s1_y[n3], s1_y[n4], s1_y[n5], D_TIME,
                  HOKAN);
                  walk_state = 2;
  } else if (walk_state == 2) {
    linearControl(s2_w[0], s2_w[1], s2_w[2], s2_w[3], s2_w[4], s2_w[5], s2_p[0],
                  s2_p[1], s2_p[2], s2_p[3], s2_p[4], s2_p[5], s2_y[n0],
                  s2_y[n1], s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5], D_TIME,
                  HOKAN);
                  walk_state = 3;
  } else if (walk_state == 3) {
    linearControl(0, 0, 0, 0, 0, 0, s3_up_p[0], s3_up_p[1], s3_up_p[2],
                  s3_up_p[3], s3_up_p[4], s3_up_p[5], s2_y[n0], s2_y[n1],
                  s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5], D_TIME, HOKAN);
    linearControl(s3_w[0], s3_w[1], s3_w[2], s3_w[3], s3_w[4], s3_w[5], s3_p[0],
                  s3_p[1], s3_p[2], s3_p[3], s3_p[4], s3_p[5], s3_y[n0],
                  s3_y[n1], s3_y[n2], s3_y[n3], s3_y[n4], s3_y[n5], D_TIME,
                  HOKAN);
                  walk_state = 4;
  } else if (walk_state == 4) {
    linearControl(s4_w[0], s4_w[1], s4_w[2], s4_w[3], s4_w[4], s4_w[5], s4_p[0],
                  s4_p[1], s4_p[2], s4_p[3], s4_p[4], s4_p[5], s4_y[n0],
                  s4_y[n1], s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5], D_TIME,
                  HOKAN);
                  walk_state = 1;
  } else {
    walk_state = 1;
  }
  delay(DELAY_TIME); // 0.2秒待つ
}

void legWave(){
  legStop();
  delay(DELAY_TIME);
  setLegPosLinear(0, 0, 2000, -400, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(1, 0, 2000, -400, D_TIME, HOKAN); 
  setLegPosLinear(0, 0, 2000, 400, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(2, 0, 2000, -400, D_TIME, HOKAN); 
  setLegPosLinear(1, 0, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(0, -4000, 2000, 400, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(3, 0, 2000, -400, D_TIME, HOKAN); 
  setLegPosLinear(2, 0, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(1, -4000, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(0, -4000, 2000, -400, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(4, 0, 2000, -400, D_TIME, HOKAN); 
  setLegPosLinear(3, 0, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(2, -4000, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(1, -4000, 2000, -400, D_TIME, HOKAN);
  setLegPosLinear(0, -3500, 3000, 0, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(5, 0, 2000, -400, D_TIME, HOKAN); 
  setLegPosLinear(4, 0, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(3, -4000, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(2, -4000, 2000, -400, D_TIME, HOKAN);
  setLegPosLinear(1, -3500, 3000, 0, D_TIME, HOKAN);
  delay(DELAY_TIME); 
  setLegPosLinear(5, 0, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(4, -4000, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(3, -4000, 2000, -400, D_TIME, HOKAN);
  setLegPosLinear(2, -3500, 3000, 0, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(5, -4000, 2000, 400, D_TIME, HOKAN);
  setLegPosLinear(4, -4000, 2000, -400, D_TIME, HOKAN);
  setLegPosLinear(3, -3500, 3000, 0, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(5, -4000, 2000, -400, D_TIME, HOKAN);
  setLegPosLinear(4, -3500, 3000, 0, D_TIME, HOKAN);
  delay(DELAY_TIME);
  setLegPosLinear(5, -3500, 3000, 0, D_TIME, HOKAN);
  delay(DELAY_TIME);
  digitalWrite(5, LOW); // LED消灯
  delay(1000);
  digitalWrite(5, HIGH); // LED点灯
  // leg_state = 0; // stop状態に戻す
  delay(DELAY_TIME);
}

void checkLeg(int id) {
  setLegPosLinear(id, 0, 0, -400, 20, 30);
  delay(200);
  setLegPosLinear(id, 0, 0, 400, 20, 30);
  delay(200);
  setLegPosLinear(id, -3500, 3000, 400, 20, 30);
  delay(200);
  setLegPosLinear(id, -3500, 3000, -400, 20, 30);
  delay(200);
  setLegPosLinear(id, -4000, 1500, -400, 20, 30);
  delay(200);
  setLegPosLinear(id, -4000, 1500, 400, 20, 30);
  delay(200);
  setLegPosLinear(id, -4000, 3000, 400, 20, 30);
  delay(200);
  setLegPosLinear(id, -4000, 3000, -400, 20, 30);
  delay(200);
  digitalWrite(5, LOW); // LED消灯
  delay(1000);
  digitalWrite(5, HIGH); // LED点灯
  leg_state = 0; // stop状態に戻す
}

void headTask(void *pvParameters) {
  while (1) {
    if (head_state == 1) {
      head_yaw.write(90 + head_speed);
    } else if (head_state == 2) {
      head_yaw.write(90 - head_speed);
    } else { // head_state == 0
      head_yaw.write(90);
    }
    delay(DELAY_TIME);
  }
}

void legTask(void *pvParameters) {
  while (1) {
    // 歩容の状態を更新
    if (leg_state == 0) {
      legStop();
    } else if (leg_state == 7 || leg_state == 8) {
      rotateControl(leg_state - 7); // 7:右回転, 8:左回転
    } else if (leg_state == 1 || leg_state == 2 || leg_state == 3 ||
               leg_state == 4 || leg_state == 5 || leg_state == 6) {
      walk2Control(leg_state - 1);
    } else if (leg_state == 15) {
      legWave();
    } else if (leg_state == 9 || leg_state == 10 || leg_state == 11 ||
               leg_state == 12 || leg_state == 13 || leg_state == 14) {
      checkLeg(leg_state - 9);
    } else {
      legStop();
      digitalWrite(5, LOW); // LED消灯
      delay(1000);
      digitalWrite(5, HIGH); // LED点灯
      delay(1000);
    }
    delay(DELAY_TIME);
  }
}