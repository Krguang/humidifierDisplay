#include "dataProcessing.h"

uint8_t key[4];
uint16_t localArray[256];

uint16_t humiCurrent;
uint16_t humiOpening;
uint16_t powerProportion;

void dataProcessing() {
	if (key[0]==0)
	{
		localArray[0] &= ~(1 << 0);
	}
	else {
		localArray[0] |= (1 << 0);
	}

	if (key[1] == 0)
	{
		localArray[0] &= ~(1 << 1);
	}
	else {
		localArray[0] |= (1 << 1);
	}

	if (key[2] == 0)
	{
		localArray[0] &= ~(1 << 2);
	}
	else {
		localArray[0] |= (1 << 2);
	}

	if (key[3] == 0)
	{
		localArray[0] &= ~(1 << 3);
	}
	else {
		localArray[0] |= (1 << 3);
	}

	humiCurrent = localArray[5];
	humiOpening = localArray[6];
	powerProportion = localArray[7];
}