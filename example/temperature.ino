/*
 * @Author       : Leon Lee
 * @Date         : 2022-4-9 16:16:16
 * @LastEditors  : Leon
 * @LastEditTime : 2022-04-19 11:12:26
 * @Description  :
 * @FilePath     : \nst1001\src\temperature.ino
 */
#include <GDBStub.h>
#include "ticker.h"
#include "nst1001.h"
#define DQ 7
NST1001 nst1001(DQ);

//
#define led D4 // gpio2,low active
Ticker ticker; 
Ticker tempReadTimer;
bool readTempFlag = 0;
uint8_t tempReadInteval = 5; //定义读温度的时间间隔

// toggle led state
void tick()
{
  digitalWrite(led, !digitalRead(led)); // set pin to the opposite state
}

void setup()
{
  Serial.begin(115200);
  gdbstub_init();
  pinMode(led, OUTPUT);
  ticker.attach(0.3, tick); 
  tempReadTimer.attach(tempReadInteval, readTemp);
}

/**
 * @brief 每隔一定时间读取一次温度
 * @param {*}
 * @return {*}
 */
void IRAM_ATTR readTemp()
{
  readTempFlag = 1;
}

void loop()
{
  float tempC = 0;
  if (readTempFlag)
  {
    readTempFlag = 0;
    tempC = nst1001.readNst1001();
    Serial.printf("Temp:%5.2f\n", tempC);
  }
}
