memo.md
==

* RX1を22に、TX1を23に変更。
* ファイルの場所は<code>~/.platformio/packages/framework-arduinoespressif32/cores/esp32/HardwareSerial.h</code>

```h
// Default pins for UART1 are arbitrary, and defined here for convenience.
#if SOC_UART_NUM > 1
#ifndef RX1
    #if CONFIG_IDF_TARGET_ESP32
      #define RX1 (gpio_num_t)22//default: 9 ←変更
    #elif CONFIG_IDF_TARGET_ESP32S2
      #define RX1 (gpio_num_t)18
    #elif CONFIG_IDF_TARGET_ESP32C3
      #define RX1 (gpio_num_t)18
    #elif CONFIG_IDF_TARGET_ESP32S3
      #define RX1 (gpio_num_t)15
    #endif
  #endif

  #ifndef TX1
    #if CONFIG_IDF_TARGET_ESP32
      #define TX1 (gpio_num_t)23//default: 10 ←変更
    #elif CONFIG_IDF_TARGET_ESP32S2
      #define TX1 (gpio_num_t)17
    #elif CONFIG_IDF_TARGET_ESP32C3
      #define TX1 (gpio_num_t)19
    #elif CONFIG_IDF_TARGET_ESP32S3
      #define TX1 (gpio_num_t)16
    #endif
  #endif
#endif /* SOC_UART_NUM > 1 */
```