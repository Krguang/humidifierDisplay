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
	write_command(0xe2);	//��λ
	HAL_Delay(5);
	write_command(0x2c);	//��ѹ����1
	HAL_Delay(50);
	write_command(0x2e);	//��ѹ����2
	HAL_Delay(50);
	write_command(0x2f);	//��ѹ����3
	HAL_Delay(5);
	write_command(0x23);	//�ֵ��Աȶȣ������÷�Χ0x20��0x27
	write_command(0x81);	//΢���Աȶ�
	write_command(0x28);	//΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f
	write_command(0xa2);	//1/9ƫѹ�ȣ�bias��
	write_command(0xc8);	//��ɨ��˳�򣺴��ϵ���
	write_command(0xa0);	//��ɨ��˳�򣺴�����
	write_command(0x40);	//��ʼ�У���һ�п�ʼ
	write_command(0xaf);	//����ʾ
}

void lcd_address(uint16_t page, uint16_t column)
{
	column = column - 0x01;
	page = page - 1;
	write_command(0xb0 + page);						//����ҳ��ַ��ÿ8��Ϊһҳ��ȫ����64�У����ֳ�8ҳ
	write_command(0x10 + ((column >> 4) & 0x0f));		//�����е�ַ�ĸ�4λ
	write_command(column & 0x0f);					//�����е�ַ�ĵ�4λ
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
				
				write_data(*dp); //д���ݵ� LCD,ÿд��һ�� 8 λ�����ݺ��е�ַ�Զ��� 1
			}
			else
				write_data(~*dp);
			dp++;
		}
	}
}

//��ָ�����Ѷ�ֿ�IC
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

//�Ӿ���Ѷ�ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�
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

//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_16x16(ulong fontaddr, uint8_t page, uint8_t column)
{
	uint8_t i, j, disp_data;
	rom_cs_low;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr & 0xff0000) >> 16);	 //��ַ�ĸ�8λ,��24λ
	send_command_to_ROM((fontaddr & 0xff00) >> 8);		 //��ַ����8λ,��24λ
	send_command_to_ROM(fontaddr & 0xff);				 //��ַ�ĵ�8λ,��24λ
	for (j = 0; j<2; j++)
	{
		lcd_address(page + j, column);
		for (i = 0; i<16; i++)
		{
			disp_data = get_data_from_ROM();
			write_data(disp_data);	//д���ݵ�LCD,ÿд��1�ֽڵ����ݺ��е�ַ�Զ���1
		}
	}
	rom_cs_high;
}


//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_8x16(ulong fontaddr, uint8_t page, uint8_t column)
{
	uint8_t i, j, disp_data;
	rom_cs_low;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr & 0xff0000) >> 16);	 //��ַ�ĸ�8λ,��24λ
	send_command_to_ROM((fontaddr & 0xff00) >> 8);		 //��ַ����8λ,��24λ
	send_command_to_ROM(fontaddr & 0xff);				 //��ַ�ĵ�8λ,��24λ
	for (j = 0; j<2; j++)
	{
		lcd_address(page + j, column);
		for (i = 0; i<8; i++)
		{
			disp_data = get_data_from_ROM();
			write_data(disp_data);	//д���ݵ�LCD,ÿд��1�ֽڵ����ݺ��е�ַ�Զ���1
		}
	}
	rom_cs_high;
}


//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_5x8(ulong fontaddr, uint8_t page, uint8_t column)
{
	uint8_t i, disp_data;
	rom_cs_low;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr & 0xff0000) >> 16);	 //��ַ�ĸ�8λ,��24λ
	send_command_to_ROM((fontaddr & 0xff00) >> 8);		 //��ַ����8λ,��24λ
	send_command_to_ROM(fontaddr & 0xff);				 //��ַ�ĵ�8λ,��24λ
	lcd_address(page, column);
	for (i = 0; i<5; i++)
	{
		disp_data = get_data_from_ROM();
		write_data(disp_data);	//д���ݵ�LCD,ÿд��1�ֽڵ����ݺ��е�ַ�Զ���1
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


			get_and_write_5x8(fontaddr, page, column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������

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
			//������壨GB2312�������ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (text[i] - 0xb0) * 94;
			fontaddr += (text[i + 1] - 0xa1) + 846;
			fontaddr = (ulong)(fontaddr * 32);

			get_and_write_16x16(fontaddr, page, column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i += 2;
			column += 16;
		}
		else if (((text[i] >= 0xa1) && (text[i] <= 0xa3)) && (text[i + 1] >= 0xa1))
		{
			//������壨GB2312��15x16����ַ��ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (text[i] - 0xa1) * 94;
			fontaddr += (text[i + 1] - 0xa1);
			fontaddr = (ulong)(fontaddr * 32);

			get_and_write_16x16(fontaddr, page, column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i += 2;
			column += 16;
		}
		else if ((text[i] >= 0x20) && (text[i] <= 0x7e))
		{
			fontaddr = (text[i] - 0x20);
			fontaddr = (unsigned long)(fontaddr * 16);
			fontaddr = (unsigned long)(fontaddr + 0x3cf80);

			get_and_write_8x16(fontaddr, page, column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i += 1;
			column += 8;
		}
		else
			i++;
	}

}
