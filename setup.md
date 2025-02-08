環境構築・動作手順書
==

## PlatformIO IDE
![](./pictures/pio.png)

### 環境構築
[参考](https://qiita.com/nextfp/items/f54b216212f08280d4e0)
* VScodeのインストール
* PlatformIO IDE プラグインをインストール

![](./pictures/pio_plugin.png)

### 使い方
#### projectの作成

#### コンパイル・書き込み

#### ライブラリの追加

#### htmlに書き込む

## krsシリーズをICS変換基板を用いてArduino/ESP32で動かす
* 公式ドキュメントを参照する
  * [準備編](https://kondo-robot.com/faq/ics_board_-tutorial1)
  * [sampleコードでの実践編](https://kondo-robot.com/faq/ics_board_-tutorial2)
* 注意事項
  * ICS変換基板は半田つけが必要
  * サンプルコードはHardSerialのほうで問題ない
  * ライブラリを入れる必要がある→[ICSライブラリ](https://kondo-robot.com/faq/ics-library-a2)
    * クラスなどが定義されている他、sampleコードもその中に入っている
    * Arduino IDEでのライブラリのインストールは Sketch → Include Libraries → Add .ZIP Library ... でダウンロードしたzipファイルを選択 
    * PlatformIOを利用している場合、<code>libs</code>ディレクトリ内などに手動でライブラリを入れる必要がある
  * ボーレートをすべてにおいて統一する必要がある
    * Arduinoでは115200がMaxであるため注意が必要
    * ESP32では1250000で問題なく動作する
    * (私の場合は) (1)srcコード内のボーレート指定 (2)krs自体のボーレート指定 (3)<code>platform.ini</code>内の<code>monitor speed</code>のボーレート指定 の3つの値を揃えないとできなかった
  * krsサーボの設定
    * **WindowsのICSManager**で行うこと(演習でやったubuntu版だとIDの書き換えしかできないかつボーレートが勝手に1250000になるという罠がある)
    * [ics-manager](https://kondo-robot.com/faq/ics35mag)と[usbを認識する用のドライバ](https://kondo-robot.com/faq/ko-driver-2023)を入れる必要がある
      * ダウンロード→解凍したのち、デバイスマネージャーで設定する必要がある(pdfに書いてある)
      * zipファイル内に入っているpdfをよく読む
  * serial0を使用している場合、書き込む際に、RXTXのジャンパピンを抜く必要がある。(していない場合は以下のエラーが出るはず)
```bash
avrdude: stk500_recv(): programmer is not responding 
avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x00
```

## ESP32でSerial0, Serial1, Serial2を使う方法
* 該当する部分のコード(<code>./esp_servo/src/main.cpp</code>)
```cpp
// ######################## servo example ########################

#include <IcsHardSerialClass.h>

const byte EN_PIN0 = 2;
const byte EN_PIN1 = 4;
const byte EN_PIN2 = 18;
const long BAUDRATE = 1250000;
const int TIMEOUT = 1000;

IcsHardSerialClass krs0(&Serial, EN_PIN0, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs1(&Serial1, EN_PIN1, BAUDRATE, TIMEOUT);
IcsHardSerialClass krs2(&Serial2, EN_PIN2, BAUDRATE, TIMEOUT);

```
  * krsクラスをそれぞれ定義
  * このようにsampleコードの<code>&Serial</code>をそれぞれ<code>&Serial1</code>, <code>&Serial2</code>とする
* 注意点
  * Serial0を使う場合は、書き込みの際にRXTXの接続を切る
  * Serial1を使う場合、デフォルトで指定されているpinは使用することができない
    * [参考](https://qiita.com/Ninagawa123/items/8ce2d55728fd5973087d)
    * platformio内のpackageを直接編集して、デフォルトのピンを書き換えた
    * [memo.txt](./memo.md)を参照のこと
  * **guardian_control_wifiではSerial1がうまく動かなかった**
    * **WiFiを接続した場合、うまく動かない現象が観測された**
    * 原因究明

## 線形補完
* 歩行させるためには足先をゆっくり動かし、床をつかめないといけない
* 現在値から指令値への移動をn分割し、次の分割値に行くまでの時間を指定することで、トルクを保ちつつ速度をゆっくりにすることができる。
* [ICS変換基板の使用方法（6-2）　直線補間でサーボを滑らかに動かす](https://kondo-robot.com/faq/ics_board_-tutorial6-2)

## wifi関連