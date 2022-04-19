/*
 * @Author       : Leon Lee
 * @Date         : 2022-03-20 15:24:35
 * @LastEditors  : Leon
 * @LastEditTime : 2022-04-19 10:14:08
 * @Description  : file content
 * @FilePath     : \nst1001\src\nst1001.h
 */
#ifndef nst1001_h
#define nst1001_h
#include <Arduino.h>

class NST1001
{
public:
    NST1001(uint8_t dq_pin, uint8_t en_pin = 0);
    float readNst1001(uint16_t num = 3);
    void nst1001PowerDown();
    void nst1001PowerOn();

private:
    void startOnceConvert();
    uint32_t readOnceNst1001Count();
    float calculateTemp(uint32_t pCount);
    uint8_t _dq_pin;
    uint8_t _en_pin;
};
#endif