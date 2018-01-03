#include <white12864.h>

#define lcd_clk_high	HAL_GPIO_WritePin(LCD_SCK_GPIO_Port,LCD_SCK_Pin,GPIO_PIN_SET)
#define lcd_clk_low		HAL_GPIO_WritePin(LCD_SCK_GPIO_Port,LCD_SCK_Pin,GPIO_PIN_RESET)

#define lcd_sda_high	HAL_GPIO_WritePin(LCD_SDA_GPIO_Port,LCD_SDA_Pin,GPIO_PIN_SET)
#define lcd_sda_low		HAL_GPIO_WritePin(LCD_SDA_GPIO_Port,LCD_SDA_Pin,GPIO_PIN_RESET)

#define lcd_rs_high		HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_SET)
#define lcd_rs_low		HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_RESET)

#define	lcd_rst_high	HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_SET)
#define	lcd_rst_low		HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_RESET)

#define lcd_cs_high		HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)
#define lcd_cs_low		HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)

#define rom_in_high		HAL_GPIO_WritePin(ROM_IN_GPIO_Port,ROM_IN_Pin,GPIO_PIN_SET)
#define rom_in_low		HAL_GPIO_WritePin(ROM_IN_GPIO_Port,ROM_IN_Pin,GPIO_PIN_RESET)

#define rom_out_high	HAL_GPIO_WritePin(ROM_OUT_GPIO_Port,ROM_OUT_Pin,GPIO_PIN_SET)
#define rom_out_low		HAL_GPIO_WritePin(ROM_OUT_GPIO_Port,ROM_OUT_Pin,GPIO_PIN_RESET)
#define rom_out_read	HAL_GPIO_ReadPin(ROM_OUT_GPIO_Port, ROM_OUT_Pin)


#define rom_sck_high	HAL_GPIO_WritePin(ROM_SCK_GPIO_Port,ROM_SCK_Pin,GPIO_PIN_SET)
#define	rom_sck_low		HAL_GPIO_WritePin(ROM_SCK_GPIO_Port,ROM_SCK_Pin,GPIO_PIN_RESET)

#define rom_cs_high		HAL_GPIO_WritePin(ROM_CS_GPIO_Port,ROM_CS_Pin,GPIO_PIN_SET)
#define rom_cs_low		HAL_GPIO_WritePin(ROM_CS_GPIO_Port,ROM_CS_Pin,GPIO_PIN_RESET)


static uint8_t fac_us = 0;
/**
* @brief Delay on us level
* @func  void BSP_Delay_us()
* @param uint32_t nus
* @return   None
* @note  nus <= 798915us(2^24/fac_us @fac_us=21)
**/
void delay_us(uint64_t nus)
{
	while (nus--)
	{
		for (uint8_t i = 0; i < 4; i++)
		{

		}
	}
}


void write_command(uint16_t data)
{
	lcd_cs_low;
	lcd_rs_low;
	for (uint8_t i = 0; i < 8; i++)
	{
		lcd_clk_low;
		if (data&0x80) lcd_sda_high;
		else lcd_sda_low;
		lcd_clk_high;
		data = data << 1;
	}
	lcd_cs_high;
}

void write_data(uint16_t data)
{
	lcd_cs_low;
	lcd_rs_high;
	for (uint8_t i = 0; i < 8; i++)
	{
		lcd_clk_low;
		if (data & 0x80) lcd_sda_high;
		else lcd_sda_low;
		lcd_clk_high;
		data = data << 1;
	}
	lcd_cs_high;
}

void init_lcd(void)
{
	lcd_rst_low;
	HAL_Delay(100);
	lcd_rst_high;
	HAL_Delay(100);
	write_command(0xe2);	//软复位
	HAL_Delay(5);
	write_command(0x2c);	//升压步骤1
	HAL_Delay(50);
	write_command(0x2e);	//升压步骤2
	HAL_Delay(50);
	write_command(0x2f);	//升压步骤3
	HAL_Delay(5);
	write_command(0x23);	//粗调对比度，可设置范围0x20～0x27
	write_command(0x81);	//微调对比度
	write_command(0x28);	//微调对比度的值，可设置范围0x00～0x3f
	write_command(0xa2);	//1/9偏压比（bias）
	write_command(0xc8);	//行扫描顺序：从上到下
	write_command(0xa0);	//列扫描顺序：从左到右
	write_command(0x40);	//起始行：第一行开始
	write_command(0xaf);	//开显示
}

void lcd_address(uint16_t page, uint16_t column)
{
	column = column - 0x01;
	page = page - 1;
	write_command(0xb0 + page);						//设置页地址，每8行为一页，全屏共64行，被分成8页
	write_command(0x10 + ((column >> 4) & 0x0f));		//设置列地址的高4位
	write_command(column & 0x0f);					//设置列地址的低4位
}

void clear_screen(void)
{
	for (uint8_t i = 0; i < 9; i++)
	{
		lcd_address(1 + i, 1);
		for (uint8_t j = 0; j < 132; j++)
		{
			write_data(0x00);
		}
	}
}

void display_pic(uint8_t *dp)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		lcd_address(i + 1, 1);
		for (uint8_t j = 0; j < 128; j++)
		{
			write_data(*dp);
			dp++;
		}
	}
}

void display_graphic_16x16(uint8_t page, uint8_t column, uint8_t reverse, uint8_t *dp)
{
	uint8_t i, j;
	for (j = 0; j<2; j++)
	{
		lcd_address(page + j, column);
		for (i = 0; i<16; i++)
		{
			if (reverse == 1)
			{
				
				write_data(*dp); //写数据到 LCD,每写完一个 8 位的数据后列地址自动加 1
			}
			else
				write_data(~*dp);
			dp++;
		}
	}
}

//送指令到晶联讯字库IC
void send_command_to_ROM(uint8_t datu)
{
	uint8_t i;
	for (i = 0; i<8; i++)
	{
		rom_sck_low;
		delay_us(10);
		if (datu & 0x80) rom_in_high;
		else rom_in_low;
		datu = datu << 1;
		rom_sck_high;
		delay_us(10);
	}
}

//从晶联讯字库IC中取汉字或字符数据（1个字节）
static uint8_t get_data_from_ROM()
{
	uint8_t i;
	uint8_t ret_data = 0;
	for (i = 0; i<8; i++)
	{
		rom_out_high;
		rom_sck_low;
		delay_us(1);
		ret_data = ret_data << 1;
		if (rom_out_read)
			ret_data = ret_data + 1;
		else
			ret_data = ret_data + 0;
		rom_sck_high;
		delay_us(1);
	}
	return(ret_data);
}

//从指定地址读出数据写到液晶屏指定（page,column)座标中
void get_and_write_16x16(ulong fontaddr, uint8_t page, uint8_t column)
{
	uint8_t i, j, disp_data;
	rom_cs_low;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr & 0xff0000) >> 16);	 //地址的高8位,共24位
	send_command_to_ROM((fontaddr & 0xff00) >> 8);		 //地址的中8位,共24位
	send_command_to_ROM(fontaddr & 0xff);				 //地址的低8位,共24位
	for (j = 0; j<2; j++)
	{
		lcd_address(page + j, column);
		for (i = 0; i<16; i++)
		{
			disp_data = get_data_from_ROM();
			write_data(disp_data);	//写数据到LCD,每写完1字节的数据后列地址自动加1
		}
	}
	rom_cs_high;
}


//从指定地址读出数据写到液晶屏指定（page,column)座标中
void get_and_write_8x16(ulong fontaddr, uint8_t page, uint8_t column)
{
	uint8_t i, j, disp_data;
	rom_cs_low;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr & 0xff0000) >> 16);	 //地址的高8位,共24位
	send_command_to_ROM((fontaddr & 0xff00) >> 8);		 //地址的中8位,共24位
	send_command_to_ROM(fontaddr & 0xff);				 //地址的低8位,共24位
	for (j = 0; j<2; j++)
	{
		lcd_address(page + j, column);
		for (i = 0; i<8; i++)
		{
			disp_data = get_data_from_ROM();
			write_data(disp_data);	//写数据到LCD,每写完1字节的数据后列地址自动加1
		}
	}
	rom_cs_high;
}


//从指定地址读出数据写到液晶屏指定（page,column)座标中
void get_and_write_5x8(ulong fontaddr, uint8_t page, uint8_t column)
{
	uint8_t i, disp_data;
	rom_cs_low;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr & 0xff0000) >> 16);	 //地址的高8位,共24位
	send_command_to_ROM((fontaddr & 0xff00) >> 8);		 //地址的中8位,共24位
	send_command_to_ROM(fontaddr & 0xff);				 //地址的低8位,共24位
	lcd_address(page, column);
	for (i = 0; i<5; i++)
	{
		disp_data = get_data_from_ROM();
		write_data(disp_data);	//写数据到LCD,每写完1字节的数据后列地址自动加1
	}
	rom_cs_high;
}
ulong  fontaddr = 0;

void display_string_5x8(uint8_t page, uint8_t column, uint8_t *text)
{
	unsigned char i = 0;
	while ((text[i]>0x00))
	{

		if ((text[i] >= 0x20) && (text[i] <= 0x7e))
		{
			fontaddr = (text[i] - 0x20);
			fontaddr = (unsigned long)(fontaddr * 8);
			fontaddr = (unsigned long)(fontaddr + 0x3bfc0);


			get_and_write_5x8(fontaddr, page, column);	 //从指定地址读出数据写到液晶屏指定（page,column)座标中

			i += 1;
			column += 6;
		}
		else
			i++;
	}
}


void display_GB2312_string(uint8_t page, uint8_t column, uint8_t *text)
{
	uint8_t i = 0;
	while ((text[i]>0x00))
	{
		if (((text[i] >= 0xb0) && (text[i] <= 0xf7)) && (text[i + 1] >= 0xa1))
		{
			//国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//由于担心8位单片机有乘法溢出问题，所以分三部取地址
			fontaddr = (text[i] - 0xb0) * 94;
			fontaddr += (text[i + 1] - 0xa1) + 846;
			fontaddr = (ulong)(fontaddr * 32);

			get_and_write_16x16(fontaddr, page, column);	 //从指定地址读出数据写到液晶屏指定（page,column)座标中
			i += 2;
			column += 16;
		}
		else if (((text[i] >= 0xa1) && (text[i] <= 0xa3)) && (text[i + 1] >= 0xa1))
		{
			//国标简体（GB2312）15x16点的字符在晶联讯字库IC中的地址由以下公式来计算：
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//由于担心8位单片机有乘法溢出问题，所以分三部取地址
			fontaddr = (text[i] - 0xa1) * 94;
			fontaddr += (text[i + 1] - 0xa1);
			fontaddr = (ulong)(fontaddr * 32);

			get_and_write_16x16(fontaddr, page, column);	 //从指定地址读出数据写到液晶屏指定（page,column)座标中
			i += 2;
			column += 16;
		}
		else if ((text[i] >= 0x20) && (text[i] <= 0x7e))
		{
			fontaddr = (text[i] - 0x20);
			fontaddr = (unsigned long)(fontaddr * 16);
			fontaddr = (unsigned long)(fontaddr + 0x3cf80);

			get_and_write_8x16(fontaddr, page, column);	 //从指定地址读出数据写到液晶屏指定（page,column)座标中
			i += 1;
			column += 8;
		}
		else
			i++;
	}

}
