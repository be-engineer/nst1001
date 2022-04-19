/*
 * @Author       : Leon Lee
 * @Date         : 2021-04-01 15:28:08
 * @LastEditors  : Leon
 * @LastEditTime : 2022-04-19 10:06:25
 * @Description  : 用NST1001测量温度,中断方式计数值波动很大，查询方式很稳定
 * @FilePath     : \nst1001\src\nst1001.cpp
 */
#include <nst1001.h>

/**
 * @brief NST1001初始化
 * @param {uint8_t} dq_pin,接收nst1001温度脉冲的pin
 * @param {uint8_t} en_pin,控制nst1001的powerdown，0为不使用en功能，不能指定GPIO0
 * @return {*}
 */
NST1001::NST1001(uint8_t dq_pin, uint8_t en_pin)
{
    if (en_pin)
    {
        pinMode(en_pin, OUTPUT); //设置为输出
        digitalWrite(en_pin, 1);
    }
    pinMode(dq_pin, INPUT_PULLUP); //设置为输入，准备接收脉冲
    _dq_pin = dq_pin;
    _en_pin = en_pin;
}

/**
 * @brief NST1001上电，如果不指定en_pin，则始终为poweron
 * @return {*}
 */
void NST1001::nst1001PowerOn()
{
    if (_en_pin)
    {
        digitalWrite(_en_pin, 1);
    }
}

/**
 * @brief NST1001断电，如果不指定en_pin，则始终为poweron，该函数无效
 * @return {*}
 */
void NST1001::nst1001PowerDown()
{
    if (_en_pin)
    {
        digitalWrite(_en_pin, 0);
    }
}

/**
 * @brief 复位NST1001进行一次转换
 * @param {*}
 * @return {*}
 */
void NST1001::startOnceConvert()
{
    pinMode(_dq_pin, OUTPUT);
    digitalWrite(_dq_pin, 0); //复位NST1001
    delay(10);
    digitalWrite(_dq_pin, 1);
    pinMode(_dq_pin, INPUT_PULLUP);
}

/**
 * @brief 读取1次NST1001的脉冲数
 * @param {*}
 * @return {uint32_t} 返回计数值
 */
uint32_t NST1001::readOnceNst1001Count()
{
    uint32_t pulseCount = 0; //脉冲计数
#define nst1001FrameTime 90  // NST1001发送一帧脉冲的时间最大为70ms，这里90
    startOnceConvert();
    uint32_t startTime = millis(), deltaTime = 0;
    while (deltaTime < nst1001FrameTime)
    {
        deltaTime = millis() - startTime;
        while (digitalRead(_dq_pin)) //检测是否有下降沿,如果nst1001无响应会卡死
        {
        };
        pulseCount++;                 //检测到低电平则脉冲加1
        while (!digitalRead(_dq_pin)) //检测是否变为高电平
        {
        };
    };
    return pulseCount;
}

/**
 * @brief 读取12次NST1001的温度,去掉最大最小值做平均
 * @param {uint16_t} num,采样次数，最小为3
 * @return {float} 返回平均温度值
 */
float NST1001::readNst1001(uint16_t num)
{
    uint16_t count, i, j;
    uint16_t value_buf[num];
    uint16_t sum = 0, temp;
    for (count = 0; count < num; count++)
    {
        value_buf[count] = readOnceNst1001Count();
        // Serial.println(value_buf[count]);
    }
    //大小排序
    for (j = 0; j < num - 1; j++)
    {
        for (i = 0; i < num - j - 1; i++)
        {
            if (value_buf[i] > value_buf[i + 1])
            {
                temp = value_buf[i];
                value_buf[i] = value_buf[i + 1];
                value_buf[i + 1] = temp;
            }
        }
    }
    //计算平均值
    for (count = 1; count < num - 1; count++)
    {
        sum += value_buf[count];
    }
    sum = sum / (num - 2);
    float t = calculateTemp(sum);
    Serial.printf("Temp:%5.2f\n", t);
    return t;
}

/**
 * @brief 根据脉冲计数计算温度并分段进行补偿
 * @param {*}
 * @return {float} 返回读取的温度值
 */
float NST1001::calculateTemp(uint32_t pCount)
{
    float temp = pCount * 0.0625 - 50.0625;
    //分段补偿
    if (temp < 30)
    {
        temp = temp + (temp - 30) * 0.005;
    }
    else if (temp <= 100)
    {
        temp = temp;
    }
    else if (temp <= 150)
    {
        temp = temp + (100 - temp) * 0.012;
    }
    return temp;
}