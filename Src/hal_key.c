/**
************************************************************
* @file         hal_key.c
* @brief        按键驱动

* 按键模块采用定时器 + GPIO状态读取机制，GPIO的配置要根据STM32的相关手册来配置

* 本驱动支持 0 ~ 12 个GPIO按键扩展，支持跨平台移植。
* @author       Gizwits
* @date         2016-09-05
* @version      V03010101
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include "hal_key.h"
#include <math.h>
#include <usart.h>
#include "dataProcessing.h"

uint8_t                 keyCountTime;
static uint8_t          keyTotolNum = 0;

#define GPIO_KEY_NUM 4                                  ///< 定义按键成员总数
keyTypedef_t singleKey[GPIO_KEY_NUM];                   ///< 定义单个按键成员数组指针
keysTypedef_t keys;                                     ///< 定义总的按键模块结构体指针

/**
* @brief Read the GPIO state
* @param [in] keys 按键功能全局结构体指针
* @return uint16_t型的GPIO状态值
*/
uint16_t getKey(keysTypedef_t *keyS)
{
    uint8_t i = 0; 
    uint16_t readKey = 0;              //必要,初始化
    
    //GPIO Cyclic scan
    for(i = 0; i < keys.keyTotolNum; i++)
    {
        if(!HAL_GPIO_ReadPin(keyS->singleKey[i].keyPort,keyS->singleKey[i].keyGpio))
        {
            G_SET_BIT(readKey, keyS->singleKey[i].keyNum);
        }
    }
    
    return readKey;
}


/**
* @brief Read the KEY value
* @param [in] keys 按键功能全局结构体指针
* @return uint16_t型的按键状态值
*/
uint16_t readKeyValue(keysTypedef_t *keyS)
{
    static uint8_t keyCheck = 0;
    static uint8_t keyState = 0;
    static uint16_t keyLongCheck = 0;
    static uint16_t keyPrev = 0;      //上一次按键

    uint16_t keyPress = 0;
    uint16_t keyReturn = 0;
    
    keyCountTime ++;
    
    if(keyCountTime >= (DEBOUNCE_TIME / KEY_TIMER_MS))          //keyCountTime 1MS+1  按键消抖10MS
    {
        keyCountTime = 0;
        keyCheck = 1;
    }
    if(1 == keyCheck)
    {
        keyCheck = 0;
        keyPress = getKey(keyS);
        switch (keyState)
        {
            case 0:
                if(keyPress != 0)
                {
                    keyPrev = keyPress;
                    keyState = 1;
                }
                break;
                
            case 1:
                if(keyPress == keyPrev)
                {
                    keyState = 2;
                    keyReturn= keyPrev | KEY_DOWN;
                }
                else                //按键抬起,是抖动,不响应按键
                {
                    keyState = 0;
                }
                break;
                
            case 2:

                if(keyPress != keyPrev)
                {
                    keyState = 0;
                    keyLongCheck = 0;
                    keyReturn = keyPrev | KEY_UP;
                    return keyReturn;
                }
                if(keyPress == keyPrev)
                {
                    keyLongCheck++;
                    if(keyLongCheck >= (PRESS_LONG_TIME / DEBOUNCE_TIME))    //长按3S有效
                    {
                        keyLongCheck = 0;
                        keyState = 3;
                        keyReturn= keyPress | KEY_LONG;
                        return keyReturn;
                    }
                }
                break;

            case 3:
                if(keyPress != keyPrev)
                {
                    keyState = 0;
                }
                break;
        }
    }
    return  NO_KEY;
}

/**
* @brief 按键回调函数

* 在该函数内完成按键状态监测后调用对应的回调函数
* @param [in] keys 按键功能全局结构体指针
* @return none
*/
void keyHandle(keysTypedef_t *keyS)
{
    uint8_t i = 0;
    uint16_t key_value = 0;

    key_value = readKeyValue(keyS);

    if(!key_value) return;
    
    //Check short press button
    if(key_value & KEY_UP)
    {
        //Valid key is detected
        for(i = 0; i < keyS->keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keyS->singleKey[i].keyNum)) 
            {
                //key callback function of short press
                if(keyS->singleKey[i].shortPress) 
                {
                    keyS->singleKey[i].shortPress(); 

                }
            }
        }
    }

    //Check short long button
    if(key_value & KEY_LONG)
    {
        //Valid key is detected
        for(i = 0; i < keyS->keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keyS->singleKey[i].keyNum))
            {
                //key callback function of long press
                if(keyS->singleKey[i].longPress) 
                {
                    keyS->singleKey[i].longPress(); 

                }
            }
        }
    }
} 

/**
* @brief 单按键初始化

* 在该函数内完成单个按键的初始化，这里需要结合STM32 GPIO寄存器配置设置参数
* @param [in] keyRccPeriph APB2_peripheral
* @param [in] keyPort Peripheral_declaration
* @param [in] keyGpio GPIO_pins_define 
* @param [in] short_press 短按状态的回调函数地址
* @param [in] long_press 长按状态的回调函数地址
* @return 单按键结构体指针
*/

keyTypedef_t keyInitOne(GPIO_TypeDef * keyPort, uint16_t keyGpio, gokitKeyFunction shortPress, gokitKeyFunction longPress)
{
    static int8_t key_total = -1;

    keyTypedef_t singleKey;
    
    //Platform-defined GPIO
    singleKey.keyPort = keyPort;
    singleKey.keyGpio = keyGpio;
    singleKey.keyNum = ++key_total;
    
    //Button trigger callback type
    singleKey.longPress = longPress;
    singleKey.shortPress = shortPress;
    
    keyTotolNum++;
    
    return singleKey;
}

/**
* @brief 按键驱动初始化

* 在该函数内完成所有的按键GPIO初始化，并开启一个1ms定时器开始按键状态监测
* @param [in] keys 按键功能全局结构体指针
* @return none
*/
void keyParaInit(keysTypedef_t *keys)
{
    //uint8_t temI = 0; 
    
    if(NULL == keys)
    {
        return ;
    }
    
    keys->keyTotolNum = keyTotolNum;
    
    //Limit on the number keys (Allowable number: 0~12)
    if(KEY_MAX_NUMBER < keys->keyTotolNum) 
    {
        keys->keyTotolNum = KEY_MAX_NUMBER; 
    }
}


void key1ShortPress(void)
{
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	if (key[0] == 0)
	{
		key[0] = 1;
	}
	else {
		key[0] = 0;
	}
}

void key2ShortPress(void)
{
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	if (key[1] == 0)
	{
		key[1] = 1;
	}
	else {
		key[1] = 0;
	}
}

void key3ShortPress(void)
{
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	if (key[2] == 0)
	{
		key[2] = 1;
	}
	else {
		key[2] = 0;
	}
}

void key4ShortPress(void)
{
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	if (key[3] == 0)
	{
		key[3] = 1;
	}
	else {
		key[3] = 0;
	}
}

void keyInit(void)
{
	singleKey[0] = keyInitOne(KEY1_GPIO_Port, KEY1_Pin, key1ShortPress, NULL);
	singleKey[1] = keyInitOne(KEY2_GPIO_Port, KEY2_Pin, key2ShortPress, NULL);
	singleKey[2] = keyInitOne(KEY3_GPIO_Port, KEY3_Pin, key3ShortPress, NULL);
	singleKey[3] = keyInitOne(KEY4_GPIO_Port, KEY4_Pin, key4ShortPress, NULL);
	keys.singleKey = (keyTypedef_t *)&singleKey;
	keyParaInit(&keys);
}