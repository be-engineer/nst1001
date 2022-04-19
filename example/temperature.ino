/*
 * @Author       : Leon Lee
 * @Date         : 2021-4-9 16:16:16
 * @LastEditors  : Leon
 * @LastEditTime : 2022-04-12 20:01:46
 * @Description  :
 * @FilePath     : \Refrigerator_temperature\src\Refrigerator_temperature.ino
 */
#include "nst1001.h"
#include <Ticker.h>

#define DQ D7
#define EN D6
NST1001 nst1001(DQ);
Ticker ticker; // wifi配置时的LED指示状态
Ticker tempReadTimer;
bool readTempFlag = 0;
uint8_t tempReadInteval = 3; //定义读温度的时间间隔
float tempC = 0;

void setup()
{
  Serial.begin(115200);
  tempReadTimer.attach(tempReadInteval, readTemp);
}

/**
 * @brief 每隔一定时间读取一次温度,不能在这里读取温度,否则会导致反复重启
 * @param {*}
 * @return {*}
 */
void IRAM_ATTR readTemp()
{
  readTempFlag = 1;
}

void loop()
{
  if (readTempFlag)
  {
    readTempFlag = 0; //避免重复读取温度
    tempC = nst1001.readNst1001(3);
    Serial.printf("Temp:%5.2f\n", tempC);
  }
}
