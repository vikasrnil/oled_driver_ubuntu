#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "i2c.h"
#include "ssd1306.h"
#include "font.h"

static uint8_t data_buf[1024];
static uint8_t max_lines = 64;
static uint8_t max_columns = 128;
static uint8_t global_x = 0;
static uint8_t global_y = 0;



int ssd1306_oled_init(){
	sleep(1);
	oled_cmd(0xAE);
	oled_cmd(0xD5);
	oled_cmd(0x80);
	oled_cmd(0xA8);
	oled_cmd(0x3F);
	oled_cmd(0xD3);
	oled_cmd(0x00);
	oled_cmd(0x40);
	oled_cmd(0x8D);
	oled_cmd(0x14);
	oled_cmd(0x20);
	oled_cmd(0x00);
	oled_cmd(0xA1);
	oled_cmd(0xC8);
	oled_cmd(0xDA);
	oled_cmd(0x12);
	oled_cmd(0x81);
	oled_cmd(0x80);
	oled_cmd(0xD9);
	oled_cmd(0xF1);
	oled_cmd(0xDB);
	oled_cmd(0x20);
	oled_cmd(0xA4);
	oled_cmd(0xA6);
	oled_cmd(0x2E);
	oled_cmd(0xAF);
}

void oled_cmd(unsigned char cmd){
	unsigned char buf[2]={SSD1306_COMM_CONTROL_BYTE,cmd};
	_i2c_write(buf,2);
}

void oled_data(unsigned char data){
	unsigned char buf[2]={SSD1306_DATA_CONTROL_BYTE,data};
	_i2c_write(buf,2);
}

void ssd1306_oled_nextline(){
	global_x=0;
	global_y=global_y+1;
	ssd1306_oled_set_XY(global_x,global_y);
}

uint8_t ssd1306_oled_onoff(uint8_t onoff)
{
    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    if (onoff == 0)
        data_buf[1] = SSD1306_COMM_DISPLAY_OFF;
    else
        data_buf[1] = SSD1306_COMM_DISPLAY_ON;

    return _i2c_write(data_buf, 2);
}

uint8_t ssd1306_oled_horizontal_flip(uint8_t flip)
{
    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    if (flip == 0)
        data_buf[1] = SSD1306_COMM_HORIZ_NORM;
    else
        data_buf[1] = SSD1306_COMM_HORIZ_FLIP;

    return _i2c_write(data_buf, 2);
}

uint8_t ssd1306_oled_display_flip(uint8_t flip)
{
    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    if (flip == 0)
        data_buf[1] = SSD1306_COMM_DISP_NORM;
    else
        data_buf[1] = SSD1306_COMM_DISP_INVERSE;

    return _i2c_write(data_buf, 2);
}

uint8_t ssd1306_oled_set_constrast(uint8_t value)
{
    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    data_buf[1] = SSD1306_COMM_CONTRAST;
    data_buf[2] = value;
    
    return _i2c_write(data_buf, 3);
}

uint8_t ssd1306_oled_scroll_onoff(uint8_t onoff)
{
    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    if (onoff == 0)
        data_buf[1] = SSD1306_COMM_DISABLE_SCROLL;
    else
        data_buf[1] = SSD1306_COMM_ENABLE_SCROLL;
    
    return _i2c_write(data_buf, 2);
}

uint8_t ssd1306_oled_set_X(uint8_t x)
{
    if (x >= max_columns)
        return 1;

    global_x = x;
    
    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    data_buf[1] = SSD1306_COMM_LOW_COLUMN | (x & 0x0f);
    data_buf[2] = SSD1306_COMM_HIGH_COLUMN | ((x >> 4) & 0x0f);

    return _i2c_write(data_buf, 3);
}

uint8_t ssd1306_oled_set_Y(uint8_t y)
{
    if (y >= (max_lines / 8))
        return 1;

    global_y = y;

    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    data_buf[1] = SSD1306_COMM_PAGE_NUMBER | (y & 0x0f);

    return _i2c_write(data_buf, 2);
}

uint8_t ssd1306_oled_set_XY(uint8_t x, uint8_t y)
{
    if (x >= max_columns || y >= (max_lines / 8))
        return 1;

    global_x = x;
    global_y = y;

    data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
    data_buf[1] = SSD1306_COMM_PAGE_NUMBER | (y & 0x0f);

    data_buf[2] = SSD1306_COMM_LOW_COLUMN | (x & 0x0f);

    data_buf[3] = SSD1306_COMM_HIGH_COLUMN | ((x >> 4) & 0x0f);

    return _i2c_write(data_buf, 4);
}

uint8_t ssd1306_oled_set_rotate(uint8_t degree)
{
    // only degree 0 and 180
    if (degree == 0)
    {
        data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
        data_buf[1] = SSD1306_COMM_HORIZ_FLIP;
        data_buf[2] = SSD1306_COMM_SCAN_REVS;

        return _i2c_write(data_buf, 3);
    }
    else if (degree == 180)
    {
        data_buf[0] = SSD1306_COMM_CONTROL_BYTE;
        data_buf[1] = SSD1306_COMM_HORIZ_NORM;
        data_buf[2] = SSD1306_COMM_SCAN_NORM;

        return _i2c_write(data_buf, 3);
    }
    else
        return 1;
}

void ssd1306_oled_puts(uint8_t size,char *s){
	while(*s){
	ssd1306_oled_putc(size,*s);
	s++;
	}
}

void ssd1306_oled_putc(uint8_t size,unsigned char c){

	uint8_t* font_table=0;
	uint8_t font_table_width=0;

	if(size == SSD1306_FONT_SMALL){
	font_table=(uint8_t*)font5x7;
	font_table_width=5;
	}

	else if(size==SSD1306_FONT_NORMAL){
	font_table=(uint8_t*)font8x8;
	font_table_width=8;
	}

	else{
	printf("invalid font");
	return;
	}

	if(global_y>=max_lines/8){
	sleep(2);
	ssd1306_oled_clear_screen();
	global_x=0;
	global_y=0;
	ssd1306_oled_set_XY(global_x,global_y);
	}

	if(global_x + font_table_width >=128){
	ssd1306_oled_nextline();
	}

	if(c=='\r' || c=='\n'){
	ssd1306_oled_nextline();
	ssd1306_oled_set_XY(global_x,global_y);
	}


	else{
	uint8_t temp[20];
	int i=0;
	temp[i++]=SSD1306_DATA_CONTROL_BYTE;

	uint8_t* font_ptr= &font_table[(c-0x20)*font_table_width];
	for(int j=0;j<font_table_width;j++){
		temp[i++]=font_ptr[j];
		global_x++;
		}
	//if(size == SSD1306_FONT_SMALL){
	//	temp[i++]=0x00;
	//	global_x++;
	//		}
	_i2c_write(temp,i);
	ssd1306_oled_set_XY(global_x,global_y);
	}
}

uint8_t ssd1306_oled_write_line(uint8_t size, char* ptr)
{
    uint16_t i = 0;
    uint16_t index = 0;
    uint8_t* font_table = 0;
    uint8_t font_table_width = 0;

    if (ptr == 0)
        return 1;

    if (size == SSD1306_FONT_SMALL) // 5x7
    {
        font_table = (uint8_t*)font5x7;
        font_table_width = 5;
    }
    else if (size == SSD1306_FONT_NORMAL) // 8x8
    {
        font_table = (uint8_t*)font8x8;
        font_table_width = 8;
    }
    else
        return 1;

    data_buf[i++] = SSD1306_DATA_CONTROL_BYTE;

    // font table range in ascii table is from 0x20(space) to 0x7e(~)
    while (ptr[index] != 0 && i <= 1024)
    {
        if ((ptr[index] < ' ') || (ptr[index] > '~'))
            return 1;

        uint8_t* font_ptr = &font_table[(ptr[index] - 0x20) * font_table_width];
        uint8_t j = 0;

        for (j = 0; j < font_table_width; j++)
        {
            data_buf[i++] = font_ptr[j];
            if (i > 1024)
                return 1;
        }

        index++;
    }

    return _i2c_write(data_buf, i);
}

uint8_t ssd1306_oled_write_string(uint8_t size, char* ptr)
{
    uint8_t rc = 0;

    if (ptr == 0)
        return 1;

	char buf[26];
	int g_ind=0;

	while(ptr[g_ind]!='\0'){
	memset(buf,0,26);
	uint8_t i=0;

	while(i<25 && ptr[g_ind]!='\0' && ptr[g_ind]!='\n'){
		buf[i++]=ptr[g_ind++];
	}

	buf[i]='\0';

	if(ptr[g_ind]=='\n')
		g_ind++;

        ssd1306_oled_set_XY(global_x, global_y);
        rc += ssd1306_oled_write_line(size, buf);

            global_x = 0;
            global_y++;
            if (global_y >= (max_lines / 8))
                global_y = 0;
        }

    return rc;
}

uint8_t ssd1306_oled_clear_line(uint8_t row)
{
    uint8_t i;
    if (row >= (max_lines / 8))
        return 1;

    ssd1306_oled_set_XY(0, row);
    data_buf[0] = SSD1306_DATA_CONTROL_BYTE;
    for (i = 0; i < max_columns; i++)
        data_buf[i+1] = 0x00;

    return _i2c_write(data_buf, 1 + max_columns);
}

uint8_t ssd1306_oled_clear_screen()
{
    uint8_t rc = 0;
    uint8_t i;

    for (i = 0; i < (max_lines / 8); i++)
    {
        rc += ssd1306_oled_clear_line(i);
    }

    return rc;
}

