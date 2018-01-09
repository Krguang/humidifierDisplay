#include "display.h"
#include "white12864.h"
#include "dataProcessing.h"



uint8_t* intToFloat(uint16_t data) {
	static uint8_t floatTemp[5];
	floatTemp[0] = (data / 100) % 10 + 48;
	floatTemp[1] = (data / 10) % 10 + 48;
	floatTemp[2] = '.';
	floatTemp[3] = data % 10 + 48;
	floatTemp[4] = '\0';
	return floatTemp;
}

void display() {
	display_GB2312_string(1, 1, "当前电流:");
	display_GB2312_string(3, 1, "加湿开度:");
	display_GB2312_string(1, 80, intToFloat(humiCurrent));
	display_GB2312_string(3, 80, intToFloat(humiOpening));
	display_GB2312_string(1, 112, "A");
	display_GB2312_string(3, 112, "%");
}