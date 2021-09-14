#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>
#include "gpio_dev.h"
#include "spi_8MM_driver.h"



#define ROW_BYTE_NUM (1600 * 3)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define GPIO_DCX 6
#define GPIO_RESX 7


void LCD_WrCmd(unsigned char cmd)
{
	gpio_SetValue(GPIO_DCX, GPIO_VALUE_LOW);

	transfer_data(cmd);

	gpio_SetValue(GPIO_DCX, GPIO_VALUE_HIGH);
}

void LCD_WrDat(unsigned char dat)
{
	transfer_data(dat);
}

void LCD_SetCmd1(unsigned char cmd, unsigned char dat)
{
	LCD_WrCmd(cmd);
	LCD_WrDat(dat);
}

void LCD_SetCmd2(unsigned char cmd, unsigned char dat1, unsigned char dat2)
{
	LCD_WrCmd(cmd);
	LCD_WrDat(dat1);
	LCD_WrDat(dat2);
}

void LCD_SetCmd3(unsigned char cmd, unsigned char dat1, unsigned char dat2, unsigned char dat3)
{
	LCD_WrCmd(cmd);
	LCD_WrDat(dat1);
	LCD_WrDat(dat2);
	LCD_WrDat(dat3);
}

void LCD_SetCmd4(unsigned char cmd, unsigned char dat1, unsigned char dat2, unsigned char dat3, unsigned char dat4)
{
	LCD_WrCmd(cmd);
	LCD_WrDat(dat1);
	LCD_WrDat(dat2);
	LCD_WrDat(dat3);
	LCD_WrDat(dat4);
}

void LCD_Init(void)
{
	gpio_SetValue(GPIO_RESX, GPIO_VALUE_LOW);
	usleep(50000); //  50ms
	gpio_SetValue(GPIO_RESX, GPIO_VALUE_HIGH);
	usleep(50000); //  50ms
	gpio_SetValue(GPIO_DCX, GPIO_VALUE_HIGH);

	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x41);
	LCD_SetCmd1(0x05, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x11);
	LCD_SetCmd1(0x0A, 0x00);
	LCD_SetCmd1(0x0B, 0x18);
	LCD_SetCmd1(0x0C, 0x18);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd1(0x38, 0x00);
	LCD_SetCmd1(0x39, 0x1F);
	LCD_SetCmd1(0x36, 0x08);
	LCD_SetCmd1(0x3A, 0x01);
	LCD_SetCmd2(0x2A, 0x00, 0x85);
	LCD_SetCmd4(0x2B, 0x00, 0x00, 0x04, 0xAF);

	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x45);
	LCD_SetCmd1(0x00, 0x04);
	LCD_SetCmd1(0x01, 0xB0);
	LCD_SetCmd1(0x07, 0x10);
	LCD_SetCmd1(0x32, 0x2C);
	LCD_SetCmd1(0x33, 0x57);
	LCD_SetCmd1(0x34, 0x00);
	LCD_SetCmd1(0x35, 0x2B);
	LCD_SetCmd1(0x36, 0x58);
	LCD_SetCmd1(0x37, 0x85);
	LCD_SetCmd1(0x42, 0x10);
	LCD_SetCmd1(0x43, 0x3b);
	LCD_SetCmd1(0x44, 0x10);
	LCD_SetCmd1(0x45, 0x3b);
	LCD_SetCmd1(0x46, 0x00);
	LCD_SetCmd1(0x47, 0x2D);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x11);
	LCD_SetCmd1(0x02, 0xB2);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd1(0xF0, 0x01);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x28);
	LCD_SetCmd1(0x0C, 0x04);
	LCD_SetCmd1(0x0D, 0x74);
	LCD_SetCmd1(0x0E, 0x04);
	LCD_SetCmd1(0x0F, 0xAF);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd1(0xF0, 0x02);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x28);
	LCD_SetCmd1(0x01, 0x80);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd1(0xF0, 0x03);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x28);
	LCD_SetCmd1(0x01, 0x40);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd1(0xF0, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x12);
	LCD_SetCmd1(0x87, 0xA9);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0xA0);
	LCD_SetCmd1(0x04, 0x17);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x2D);
	LCD_SetCmd1(0x00, 0x01);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x43);
	LCD_SetCmd1(0x03, 0x04);
	LCD_SetCmd3(0xFF, 0x21, 0x71, 0x00);

	usleep(50000); //  50ms
	LCD_WrCmd(0x11);

	usleep(120000); //  	120ms
	LCD_WrCmd(0x29);
}

void LCD_Image(unsigned char data[])
{
	int x, y, i, j;
	unsigned char rgb[3] = {0xFF, 0x00, 0x00};
	unsigned char r, g, b;
	unsigned char buf;
	unsigned char trd[482400];
	int count = 0;
	LCD_WrCmd(0x2C);

	for (y = 1199; y >= 0; y--)
	{

		for (x = 0; x < 1600 / 4; x++)
		{
			buf = 0;
			for (i = 0; i < 4; i++)
			{
				b = data[y * ROW_BYTE_NUM + (x)*3 * 4 + i * 3];
				g = data[y * ROW_BYTE_NUM + (x)*3 * 4 + i * 3 + 1];
				r = data[y * ROW_BYTE_NUM + (x)*3 * 4 + i * 3 + 2];

				if (b > 0x7F && g > 0x7F && r > 0x7F)			//白
					buf |= 0x1 << ((i)*2);

				if (b <= 0x7F && g <= 0x7F && r > 0x7F)			//紅
					buf |= 0x2 << ((i)*2);

				if (b <= 0x7F && g <= 0x7F && r <= 0x7F)		//黑
					buf |= 0x3 << ((i)*2);
			}
			// trd[count++] = buf;
			LCD_WrDat(buf);
		}
		LCD_WrDat(0x00);
		LCD_WrDat(0x00);
	}
	// transfer_pixel(&trd[0]);
}

int main(int argc, char **argv)
{
	unsigned char i = 0;
	unsigned char rgb[3] = {0x00, 0x00, 0xFF};
	unsigned char *buf;
	int fd;
	FILE *fp;
	unsigned char str[30];
	struct input_event event;

	spidev_init();
	LCD_Init();

	fd = open("/dev/input/event1", O_RDWR | O_NONBLOCK);
	if (fd < 0)
    {
        printf("open %s err\n", argv[1]);
        return -1;
    }

	buf = malloc(sizeof(unsigned char) * 1600 * 1200 * 3);
	if (buf == NULL)
	{
		printf("malloc error\n");
		return 0;
	}

	while (!(event.type == EV_KEY && event.value == 1))
	{		
		read(fd, &event, sizeof(event));
		system("python3 bus.py");
		fp = fopen("time.bmp", "rb");
		if (fp == NULL)
		{
			printf("open %s file error\n", str);
			return 0;
		}
		fseek(fp, 54, SEEK_SET);
		fread(buf, sizeof(unsigned char), 1600 * 1200 * 3, fp);
		fclose(fp);
		printf("printing time.bmp...\n");
		LCD_Image(buf);
		usleep(10000000); //  	10s
	}

	free(buf);
}