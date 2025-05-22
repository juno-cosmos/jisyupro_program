#include <Arduino.h>

#include <ESP32Servo.h>
#include <IcsHardSerialClass.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const char ssid[] = "ESP32AP-TEST";
const char pass[] = "12345678";
const IPAddress ip(192, 168, 123, 45);
const IPAddress subnet(255, 255, 255, 0);

const char html[] =
  R"rawliteral(
<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="UTF-8">
  <title>Guardian Mayfes Controller</title>
  <style>
    * { box-sizing: border-box; }
    body {
      margin: 0;
      height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      background: #222;
      color: #ddd;
      font-family: sans-serif;
    }
    .container {
      display: flex;
      gap: 40px;
    }
    .joystick-wrapper {
      display: flex;
      flex-direction: column;
      align-items: center;
    }
    .joystick-area {
      position: relative;
      width: 200px;
      height: 200px;
      background: #333;
      border-radius: 50%;
      touch-action: none;
      user-select: none;
    }
    .stick {
      position: absolute;
      width: 60px;
      height: 60px;
      background: #888;
      border-radius: 50%;
      top: 70px;
      left: 70px;
      pointer-events: none;
      transition: 0.05s;
    }
    .label {
      margin-top: 12px;
      font-size: 1.1em;
      color: #0f0;
    }
  </style>
</head>
<body>
  <div class="container">
    <!-- 左側：Xdirのみ -->
    <div class="joystick-wrapper">
      <div id="joyL" class="joystick-area">
        <div id="stickL" class="stick"></div>
      </div>
      <div id="outL" class="label">X: 0</div>
    </div>
    <!-- 右側：6dir -->
    <div class="joystick-wrapper">
      <div id="joyR" class="joystick-area">
        <div id="stickR" class="stick"></div>
      </div>
      <div id="outR" class="label">dir: none</div>
    </div>
  </div>

  <script>
    // 共通設定
    const SIZE = 200, R = 60, THRESH = 10;
    const centers = { x: SIZE/2, y: SIZE/2 };

    // 要素取得
    const joyL = document.getElementById('joyL');
    const joyR = document.getElementById('joyR');
    const stickL = document.getElementById('stickL');
    const stickR = document.getElementById('stickR');
    const outL = document.getElementById('outL');
    const outR = document.getElementById('outR');

    // ポインターID → which joystick
    const active = {};

    // 現在の値を保持する変数
    let currentL = 0;
    let currentR = 'none';

    // 角度→6dir取得
    function getDir6(angle) {
      const dirs = ['front','front-right','back-right','back','back-left','front-left'];
      let deg = angle * 180/Math.PI + 90; // -90度回転
      deg = deg%360;
      return dirs[Math.round(deg/60)%6];
    }

    // 左スティック更新（Xのみ）
    function updateL(dx) {
        const d = Math.max(-R, Math.min(R, dx));
        stickL.style.left = `${centers.x - 30 + d}px`;

        const ratio = Math.abs(d / R);  // 移動量の割合（0〜1）

        let x = 0;
        if (ratio > 0.3) {
            x = d > 0 ? 1: -1;
        }
        outL.textContent = `X: ${x}`;
        currentL = x;
    }
    // 左リセット
    function resetL() {
        stickL.style.left = '70px';
        outL.textContent = 'X: 0';
        currentL = 0;
    }

    // 右スティック更新（6dir）
    function updateR(dx, dy) {
        const dist = Math.min(Math.hypot(dx, dy), R);
        const ang = Math.atan2(dy, dx);

        const ratio = dist / R;  // 移動割合（0〜1）

        const x = dist * Math.cos(ang);
        const y = dist * Math.sin(ang);
        stickR.style.left = `${centers.x - 30 + x}px`;
        stickR.style.top = `${centers.y - 30 + y}px`;

        if (ratio > 0.3) {
            outR.textContent = 'dir: ' + getDir6(ang);
            currentR = getDir6(ang);
        } else {
            outR.textContent = 'dir: none';
            currentR = 'none';
        }
    }
    // 右リセット
    function resetR() {
        stickR.style.left = '70px';
        stickR.style.top  = '70px';
        outR.textContent = 'dir: none';
        currentR = 'none';
    }

    // ポインタダウン共通
    function onPointerDown(e, side) {
        e.target.setPointerCapture(e.pointerId);
        active[e.pointerId] = side;
        // handleMove(e);
        onPointerMove(e);
    }

    // ポインタアップ
    function onPointerUp(e) {
      const side = active[e.pointerId];
      delete active[e.pointerId];
      if (side==='L') resetL();
      if (side==='R') resetR();
      e.target.releasePointerCapture(e.pointerId);
    }

    // ポインタ移動
    function onPointerMove(e) {
      const side = active[e.pointerId];
      if (!side) return;
      const area = side==='L'? joyL: joyR;
      const rect = area.getBoundingClientRect();
      const dx = e.clientX - (rect.left + centers.x);
      const dy = e.clientY - (rect.top  + centers.y);
      if (side==='L') updateL(dx);
      else          updateR(dx, dy);
    }

    // イベント登録
    joyL.addEventListener('pointerdown', e => onPointerDown(e,'L'));
    joyR.addEventListener('pointerdown', e => onPointerDown(e,'R'));
    document.addEventListener('pointermove', onPointerMove);
    joyL.addEventListener('pointerup',   onPointerUp);
    joyR.addEventListener('pointerup',   onPointerUp);
    joyL.addEventListener('pointercancel', onPointerUp);
    joyR.addEventListener('pointercancel', onPointerUp);

    // 500msごとに現在の値をconsole.logで表示
    setInterval(() => {
        fetch(`/?left_dir=${currentL}&right_dir=${currentR}`).catch(()=>{});
        // console.log(`joy-left: ${currentL}, joy-right: ${currentR}`);
    }, 100);
  </script>
</body>
</html>
)rawliteral";

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

void handleRequest(String req);
void headTask(void *pvParameters);
void legTask(void *pvParameters);
void activateTask();
void stop();
void walk1Control();
void setLegPos(int id, int dpos0, int dpos1, int dpos2);
void setLegPosLinear(int id, int dpos0, int dpos1, int dpos2, int d_time,
                     int hokan);
void linearControl(int dpos0, int dpos1, int dpos2, int dpos3, int dpos4,
                   int dpos5, int dpos6, int dpos7, int dpos8, int dpos9,
                   int dpos10, int dpos11, int dpos12, int dpos13, int dpos14,
                   int dpos15, int dpos16, int dpos17, int d_time, int hokan);


Servo head_yaw;
#define HEAD_YAW_PIN 33

// global変数でcontrol_stateとwalk_stateを管理
int control_state = 0;      // 0:停止, 1~6が方向, 7~8が回転
int prev_control_state = 0; // 前回のcontrol_state
int walk_state = 1;         // 0:停止, 1~4が歩容の各状態
// int rotate_state = 0;       // 0:停止, 1~4が回転の各状態(4じゃない可能性あり)
volatile float leftX = 0, leftY = 0;   // 左スティックのX,Y
volatile float rightX = 0, rightY = 0; // 右スティックのX,Y

// 各種閾値の設定
#define YAW_SLOW 0.2
#define YAW_FAST 0.6
#define YAW_MAX 1.0
#define WALK_SLOW 0.2
#define WALK_FAST 0.6
#define WALK_MAX 1.0

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
  if (!client) {
    return;
  }

  // ---- ヘッダ読み飛ばし ----
  String req = client.readStringUntil('\r');
  client.flush();
  handleRequest(req);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print(html);
  client.stop();
}

void stop() {
  head_yaw.write(90);
  linearControl(-3500, -3500, -3500, -3500, -3500, -3500, 3000, 3000, 3000,
                3000, 3000, 3000, 0, 0, 0, 0, 0, 0, D_TIME, HOKAN);
  walk_state = 0;
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
  // Serial.println(pos[0][id]);
  // pos[1][id] = krs1.getPos(id);
  pos[1][id] = krs0.getPos(6 + id);
  // Serial.println(pos[1][id]);
  pos[2][id] = krs2.getPos(id);
  // Serial.println(pos[2][id]);

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

void walk1Control() {
  prev_control_state = control_state;
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

  // 1indexになったので変更
  int n0 = ((1 - id) % 6 + 6) % 6;
  int n1 = ((2 - id) % 6 + 6) % 6;
  int n2 = ((3 - id) % 6 + 6) % 6;
  int n3 = ((4 - id) % 6 + 6) % 6;
  int n4 = ((5 - id) % 6 + 6) % 6;
  int n5 = ((6 - id) % 6 + 6) % 6;

  switch (walk_state) {
  case 1:
    linearControl(0, 0, 0, 0, 0, 0, s1_up_p[n0], s1_up_p[n1], s1_up_p[n2],
                  s1_up_p[n3], s1_up_p[n4], s1_up_p[n5], s4_y[n0], s4_y[n1],
                  s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5], D_TIME, HOKAN);
    linearControl(s1_w[n0], s1_w[n1], s1_w[n2], s1_w[n3], s1_w[n4], s1_w[n5],
                  s1_p[n0], s1_p[n1], s1_p[n2], s1_p[n3], s1_p[n4], s1_p[n5],
                  s1_y[n0], s1_y[n1], s1_y[n2], s1_y[n3], s1_y[n4], s1_y[n5],
                  D_TIME, HOKAN);
    break;
  case 2:
    linearControl(s2_w[n0], s2_w[n1], s2_w[n2], s2_w[n3], s2_w[n4], s2_w[n5],
                  s2_p[n0], s2_p[n1], s2_p[n2], s2_p[n3], s2_p[n4], s2_p[n5],
                  s2_y[n0], s2_y[n1], s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5],
                  D_TIME, HOKAN);
    delay(D_TIME);
    break;
  case 3:
    linearControl(0, 0, 0, 0, 0, 0, s3_up_p[n0], s3_up_p[n1], s3_up_p[n2],
                  s3_up_p[n3], s3_up_p[n4], s3_up_p[n5], s2_y[n0], s2_y[n1],
                  s2_y[n2], s2_y[n3], s2_y[n4], s2_y[n5], D_TIME, HOKAN);
    linearControl(s3_w[n0], s3_w[n1], s3_w[n2], s3_w[n3], s3_w[n4], s3_w[n5],
                  s3_p[n0], s3_p[n1], s3_p[n2], s3_p[n3], s3_p[n4], s3_p[n5],
                  s3_y[n0], s3_y[n1], s3_y[n2], s3_y[n3], s3_y[n4], s3_y[n5],
                  D_TIME, HOKAN);
    break;
  case 4:
    linearControl(s4_w[n0], s4_w[n1], s4_w[n2], s4_w[n3], s4_w[n4], s4_w[n5],
                  s4_p[n0], s4_p[n1], s4_p[n2], s4_p[n3], s4_p[n4], s4_p[n5],
                  s4_y[n0], s4_y[n1], s4_y[n2], s4_y[n3], s4_y[n4], s4_y[n5],
                  D_TIME, HOKAN);
    delay(D_TIME);
    break;
  default:
    stop();
    delay(D_TIME);
    break;
  }
  walk_state += 1;
  if (walk_state > 4) {
    walk_state = 1;
  }
}

void handleRequest(String req) {
  if (req.startsWith("GET /?")) {
    // joy パラメータ取得
    // Serial.println(req);
    int iLeft = req.indexOf("left_dir=");
    int iRight = req.indexOf("&right_dir=", iLeft);
    if (iLeft >= 0 && iRight > iLeft) {
      String left = req.substring(iLeft + 10, iRight);
      String right = req.substring(iRight + 12);
      // Serial.println(left);
      // Serial.println(right);
      if (right == "front") {
        control_state = 1;
      } else if (right == "front-right") {
        control_state = 2;
      } else if (right == "back-right") {
        control_state = 3;
      } else if (right == "back") {
        control_state = 4;
      } else if (right == "back-left") {
        control_state = 5;
      } else if (right == "front-left") {
        control_state = 6;
      } else {
        control_state = 0;
      }
      if (left == "1") {
        leftX = 1.0;
      } else if (left == "-1") {
        leftX = -1.0;
      } else {
        leftX = 0.0;
      }
    }
    Serial.println("control_state: " + String(control_state) + " leftX: " + String(leftX));
  }
}

void headTask(void *pvParameters) {
  while (1) {
    Serial.println("a");
    float x = leftX;
    if (abs(x) < YAW_SLOW) { // 停止状態 control_state: 0
      head_yaw.write(90);
    } else {
      head_yaw.write(90 + (x * 80));
    }
    delay(100); // 10Hz 更新
  }
}

void legTask(void *pvParameters) {
  while (1) {
    // 歩容の状態を更新
    if (control_state == 0) { // 
      stop();
      delay(D_TIME);
    } else {
      walk1Control();
    }
    delay(1000-2*D_TIME);
  }
}
