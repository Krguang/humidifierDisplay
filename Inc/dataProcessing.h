#pragma once
#include "stm32f1xx_hal.h"
#include "main.h"

extern uint8_t key[];
extern uint16_t localArray[];

extern uint16_t humiCurrent;
extern uint16_t humiOpening;
extern uint16_t powerProportion;
void dataProcessing();