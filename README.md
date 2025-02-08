自主プロ ガーディアン ワイヤ駆動(多関節)多脚ロボット
==

## 開発環境
Ubuntu22.04
PlatformIO IDE

## PlatformIO IDEの環境構築
* [参考](https://qiita.com/nextfp/items/f54b216212f08280d4e0)
* 個人的にもまとめる→[環境構築・動作手順書](./setup.md)

## projectの概要
* LED_Arduino_sample
  * Arduino Nanoを使ったLチカ用のコード
  * Arduinoのexampleコードをそのまま持ってきてpioで書き込んだ
* simple_wifi_server2
  * 単純なwifiサーバーを構築するコード
  * ESP32のexampleコードをそのまま持ってきてpioで書き込んだ
  * 仕様
    * ESP32がサーバーを立てる
    * 外部のwifiに接続する
    * client側は、そのwifiを通じてIPアドレスを指定することで、サーバーに接続できる
    * サーバーではLEDを点灯/消灯を切り替えることができる
* guardian_control_wifi
  * 2号機に書き込んだコード
  * [参考](https://mukujii.sakura.ne.jp/esp2.html)
  * 仕様
    * 線形補完を実装したガーディアンの各種移動動作を記述している
    * ESP32本体をSoftAPモードでもってサーバー/Wifiのアクセスポイント化し、そのアクセスポイントにクライアント側が接続することでコントローラにアクセスできる
    * コントローラのHTMLも中に記述してある
  * 使用したライブラリ
    * [IcsClass_V210](https://kondo-robot.com/faq/ics-library-a2)
      * [krs3301](https://kondo-robot.com/product/krs-3301-ics)を動かすために利用
      * どのように利用するかは[環境構築・動作手順書](./setup.md)に記述
    * ESP32Servo
      * pwmサーボを動かすために利用
* その他のproject
  * 未整理
