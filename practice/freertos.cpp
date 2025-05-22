// グローバル（volatile を付けるとタスク間安全性が少し上がります）
volatile float leftX = 0, leftY = 0;
volatile float rightX = 0, rightY = 0;

void handleRequest(String req) {
  // ... joy, x, y をパースしたあと…
  if (joy == "joy-left") {
    leftX  = x;
    leftY  = y;
  }
  else if (joy == "joy-right") {
    rightX = x;
    rightY = y;
  }
}

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void headTask(void *pvParameters) {
  for(;;) {
    // X の値でマッピングして首を動かす
    int angle = map(leftX*100, -100, 100, 45, 135);
    head_yaw.write(angle);
    vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz 更新
  }
}

void legTask(void *pvParameters) {
  for(;;) {
    float x = rightX, y = rightY;
    // 8方向判定（前と逆順でもいいです）
    if (abs(x)<0.3 && y>0.5)        walk1Control(0);
    else if (x>0.5 && y>0.5)        walk1Control(1);
    else if (x>0.5 && abs(y)<0.3)   walk1Control(2);
    else if (x>0.5 && y<-0.5)       walk1Control(3);
    else if (abs(x)<0.3 && y<-0.5)  walk1Control(4);
    else if (x<-0.5 && y<-0.5)      walk1Control(5);
    else if (x<-0.5 && abs(y)<0.3)  walk1Control(6);
    else if (x<-0.5 && y>0.5)       walk1Control(7);
    else                            stop();
    vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz 更新
  }
}

void setup() {
  // … WiFi, server.begin(), head_yaw.attach() など初期化 …

  // タスク生成
  xTaskCreatePinnedToCore(headTask, "HeadTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(legTask,  "LegTask",  4096, NULL, 1, NULL, 1);
}

void loop() {
  // 従来のリクエスト受け取り→handleRequest() 呼び出しだけ
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();
  handleRequest(req);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print(html);
  client.stop();
}
