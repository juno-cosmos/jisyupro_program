#include <Arduino.h>
#include <IcsHardSerialClass.h>

const byte EN_PIN0 = 2;
const byte EN_PIN1 = 4;
const byte EN_PIN2 = 18;
const long BAUDRATE = 1250000;
const int TIMEOUT = 1000;
int pos[3][6];
const int hpos[3][6] = {{0, 0, 0, 0, 0, 0},
                        {-1000, -500, -1200, -1000, -1000, -400},
                        {0, 0, 0, 0, 0, 0}}; // home position
int dpos[3][6];

IcsHardSerialClass krs0(&Serial, EN_PIN0, BAUDRATE, TIMEOUT);
// IcsHardSerialClass krs1(&Serial1, EN_PIN1, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs2(&Serial2, EN_PIN2, BAUDRATE, TIMEOUT);

// put function declarations here:
void setLegPos(int id, int dpos0, int dpos1, int dpos2);
void setLegPosLinear(int id, int dpos0, int dpos1, int dpos2, int d_time,
                     int hokan);
void linearControl(int dpos0, int dpos1, int dpos2, int dpos3, int dpos4,
                   int dpos5, int dpos6, int dpos7, int dpos8, int dpos9,
                   int dpos10, int dpos11, int dpos12, int dpos13, int
                   dpos14, int dpos15, int dpos16, int dpos17, int d_time,
                   int hokan);
void setup() {
  // put your setup code here, to run once:
  // Serial.begin(115200);
  krs0.begin();
  // krs1.begin();
  krs2.begin();
  setLegPos(0, -3500, 3000, 0);
  setLegPos(1, -3500, 3000, 0);
  setLegPos(2, -3500, 3000, 0);
  setLegPos(3, -3500, 3000, 0);
  setLegPos(4, -3500, 3000, 0);
  setLegPos(5, -3500, 3000, 0);
  // Serial.println("start");
  delay(5000); // 5秒待つ
  // linearControl(4000, 4000, 4000, 4000, 4000, 4000, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //               0, 0, 0, 20, 60);
  // delay(5000); // 5秒待つ
}

void loop() {
  // // put your main code here, to run repeatedly:
  // // 雑歩行フィードフォワード, id0先頭の場合
  // // 長: 4000, 0 短: -4000, 1000 上: 0, 2000
  // // N: 0 左: -600, 右: 600
  // // Step1
  // //setLegPosLinear(2, -4000, 1500, -600, 20, 60);
  // linearControl(4000, -3500, -4000, -3500, -4000, -3500, 3000, 2500, 3000, 2500, 3000, 2500, 0, 600, -600, 0, 600, -600, 20, 60);
  // linearControl(4000, -3500, -4000, -3500, -4000, -3500, 0, 2500, 1500, 2500, 1500, 2500, 0,
  //               600, -600, 0, 600, -600, 20, 60);
  // delay(5000); // 1秒待つ

  // // Step2
  // //setLegPosLinear(2, 4000, 500, 600, 20, 60);
  // linearControl(-4000, -3500, 4000, -3500, 4000, -3500, 0, 2500, 0, 2500, 0, 2500, 0,
  //               -600, 600, 0, -600, 600, 20, 60);
  // delay(5000); // 1秒待つ

  // // Step3
  // //setLegPosLinear(2, 0, 2500, 600, 20, 60);
  // linearControl(-3500, -4000, -3500, -4000, -3500, -4000, 2500, 1500, 2500, 1500, 2500, 1500, 0,
  //               -600, 600, 0, -600, 600, 20, 60);
  // delay(5000); // 1秒待つ

  // // Step4
  // //setLegPosLinear(2, 0, 2500, -600, 20, 60);
  // linearControl(-3500, -4000, -3500, 4000, -3500, -4000, 2500, 1500, 2500, 0, 2500, 1500, 0,
  //               600, -600, 0, 600, -600, 20, 60);
  // delay(5000); // 1秒待つ

  // // debug用
  // setLegPosLinear(0, 4000, 0, 0, 20, 60);
  // delay(5000);
  // setLegPosLinear(0, -4000, 1500, 0, 20, 60);
  // delay(5000);

  // // 雑歩行2
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000, 1500, 1500, 3000, 1500, -600, 600, -600, 600, -600, 600, 20, 60);
  // delay(1000); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000, 1500, 1500, 3000, 1500, 600, -600, 600, -600, 600, -600, 20, 60);
  // delay(1000); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500, 3000, 3000, 1500, 3000, 600, -600, 600, -600, 600, -600, 20, 60);
  // delay(1000); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500, 3000, 3000, 1500, 3000, -600, 600, -600, 600, -600, 600, 20, 60);
  // delay(1000); // 1秒待つ
// 雑歩行2
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000, 1500, 1500, 3000, 1500, -600, 600, -600, 600, -600, 600, 20, 60);
  // delay(1000); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000, 1500, 1500, 3000, 1500, 600, -600, 600, -600, 600, -600, 20, 60);
  // delay(1000); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500, 3000, 3000, 1500, 3000, 600, -600, 600, -600, 600, -600, 20, 60);
  // delay(1000); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500, 3000, 3000, 1500, 3000, -600, 600, -600, 600, -600, 600, 20, 60);
  // delay(1000); // 1秒待つ
  
  // // 雑歩行3
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000, 1500, 3000, 1500, 3000, -600, 600, -600, -600, 600, -600, 20, 30);
  // delay(200); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 1500, 3000, 1500, 3000, 1500, 3000, 600, -600, 600, 600, -600, 600, 20, 30);
  // delay(200); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500, 3000, 1500, 3000, 1500, 600, -600, 600, 600, -600, 600, 20, 30);
  // delay(200); // 1秒待つ
  // linearControl(-4000, -4000, -4000, -4000, -4000, -4000, 3000, 1500, 3000, 1500, 3000, 1500, -600, 600, -600, -600, 600, -600, 20, 30);
  // delay(200); // 1秒待つ

  // 雑歩行4
  linearControl(0, 0, 0, 0, 0, 0, 0, 3000, 0, 3000, 0, 3000, -600, 600, -600, -600, 600, -600, 20, 30);
  linearControl(0, -3500, 0, -3500, 0, -3500, 0, 3000, 0, 3000, 0, 3000, -600, 600, -600, -600, 600, -600, 20, 30);
  delay(200); // 1秒待つ
  linearControl(0, -3500, 0, -3500, 0, -3500, 0, 3000, 0, 3000, 0, 3000, 600, -600, 600, 600, -600, 600, 20, 30);
  delay(200); // 1秒待つ
  linearControl(0, 0, 0, 0, 0, 0, 3500, 0, 3000, 0, 3000, 0, 600, -600, 600, 600, -600, 600, 20, 30);
  linearControl(-3500, 0, -3500, 0, -3500, 0, 3500, 0, 3000, 0, 3000, 0, 600, -600, 600, 600, -600, 600, 20, 30);
  delay(200); // 1秒待つ
  linearControl(-3500, 0, -3500, 0, -3500, 0, 3500, 0, 3000, 0, 3000, 0, -600, 600, -600, -600, 600, -600, 20, 30);
  delay(200); // 1秒待つ
}

// put function definitions here:

// 脚1本を動かす
void setLegPos(

    int id, int dpos0, int dpos1,
    int dpos2) { // id: 0-5, の次の脚のservoの位置を規定する,
                 // 最終的には組み合わせる or この中身を線形補間実装にする
  krs0.setPos(id, 7500+hpos[0][id]+dpos0);
  // delay(100);
  // krs1.setPos(id, 7500+hpos[1][id]+dpos1);
  krs0.setPos(6+id, 7500+hpos[1][id]+dpos1);
  // delay(100);
  krs2.setPos(id, 7500+hpos[2][id]+dpos2);
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
  pos[1][id] = krs0.getPos(6+id);
  Serial.println(pos[1][id]);
  pos[2][id] = krs2.getPos(id);
  Serial.println(pos[2][id]);

  tpos[0] = (tpos[0] - pos[0][id]) / hokan;
  tpos[1] = (tpos[1] - pos[1][id]) / hokan;
  tpos[2] = (tpos[2] - pos[2][id]) / hokan;

  for (int i = 0; i < hokan; i++) {
    krs0.setPos(id, pos[0][id] + tpos[0] * i);
    // krs1.setPos(id, pos[1][id] + tpos[1] * i);
    krs0.setPos(6+id, pos[1][id] + tpos[1] * i);
    krs2.setPos(id, pos[2][id] + tpos[2] * i);
    delay(d_time);
  }

  //setLegPos(id, dpos0, dpos1, dpos2);
}

// 6本の脚を線形補間で動かす
void linearControl(int dpos0, int dpos1, int dpos2, int dpos3, int dpos4, int
dpos5,
                  int dpos6, int dpos7, int dpos8, int dpos9, int dpos10, int
                  dpos11, int dpos12, int dpos13, int dpos14, int dpos15, int
                  dpos16, int dpos17, int d_time, int hokan) {
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
    pos[1][i] = krs0.getPos(6+i);
    pos[2][i] = krs2.getPos(i);

    tpos[0][i] = (tpos[0][i] - pos[0][i]) / hokan;
    tpos[1][i] = (tpos[1][i] - pos[1][i]) / hokan;
    tpos[2][i] = (tpos[2][i] - pos[2][i]) / hokan;
  }

  for (int i = 0; i < hokan; i++) {
    for (int j = 0; j < 6; j++) {
      
      krs0.setPos(j, pos[0][j] + tpos[0][j]*i);
      // krs1.setPos(j, pos[1][j]);
      krs0.setPos(6+j, pos[1][j] + tpos[1][j]*i);
      krs2.setPos(j, pos[2][j] + tpos[2][j]*i);
    }
    delay(d_time);
  }
}
