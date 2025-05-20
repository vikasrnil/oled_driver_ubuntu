#include<stdint.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<string.h>
#include<unistd.h>

#include "i2c.h"
#include "ssd1306.h"

int main(){

	if(_i2c_init(SSD1306_I2C_ADDR) ==0 ){
	printf("i2c initialized!!!\n");
	}

	ssd1306_oled_init();
	ssd1306_oled_clear_screen();

	ssd1306_oled_set_XY(0,0);
	ssd1306_oled_write_string(0x00,"hello my name is escorts kubota limited\ni am from india\n");
//	ssd1306_oled_puts(0x00,"Hello i am ram \n what about you?\n");
//	ssd1306_oled_puts(0x00,"Greetings !!!\n Good morning \n escorts kubota limited!!\n abc \n def\n Now on new page");

	return 1;
}

