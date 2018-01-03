#pragma once
#include "stm32f1xx_hal.h"
#include "main.h"


void delay_us(uint64_t nus);
void init_lcd(void);
void clear_screen(void);
void display_string_5x8(uint8_t page, uint8_t column, uint8_t *text);
void display_graphic_16x16(uint8_t page, uint8_t column, uint8_t reverse, uint8_t *dp);
void display_GB2312_string(uint8_t page, uint8_t column, uint8_t *text);