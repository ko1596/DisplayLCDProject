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
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>
#include "gpio_dev.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define GPIO_DCX		85
#define GPIO_RESX		86
#define GPIO_D0			87
#define GPIO_D1			88
#define GPIO_D2			89
#define GPIO_CS0		8

static const char *device = "/dev/spidev1.0";
static uint32_t mode;
static uint8_t bits = 8;
static char *input_file;
static char *output_file;
static uint32_t speed = 500000;
static uint16_t delay;
static int verbose;
static int transfer_size;
static int iterations;
static int interval = 5; /* interval in seconds for showing transfer rate */


static void pabort(const char *s)
{
	perror(s);
	abort();
}

void init_gpio(void)
{
	gpio_Export(GPIO_DCX);
	gpio_Export(GPIO_RESX);
	gpio_Export(GPIO_D0);
	gpio_Export(GPIO_D1);
	gpio_Export(GPIO_D2);
	gpio_Export(GPIO_CS0);

	gpio_SetDirection(GPIO_DCX, GPIO_DIRECTION_OUT);
	gpio_SetDirection(GPIO_RESX, GPIO_DIRECTION_OUT);
	gpio_SetDirection(GPIO_D0, GPIO_DIRECTION_OUT);
	gpio_SetDirection(GPIO_D1, GPIO_DIRECTION_OUT);
	gpio_SetDirection(GPIO_D2, GPIO_DIRECTION_OUT);
	gpio_SetDirection(GPIO_CS0, GPIO_DIRECTION_OUT);

	
}

void unexport_gpio(void)
{
	gpio_Unexport(GPIO_DCX);
	gpio_Unexport(GPIO_RESX);
	gpio_Unexport(GPIO_D0);
	gpio_Unexport(GPIO_D1);
	gpio_Unexport(GPIO_D2);
	gpio_Unexport(GPIO_CS0);
}

/*
 *  Unescape - process hexadecimal escape character
 *      converts shell input "\x23" -> 0x23
 */
static int unescape(char *_dst, char *_src, size_t len)
{
	int ret = 0;
	int match;
	char *src = _src;
	char *dst = _dst;
	unsigned int ch;

	while (*src) {
		if (*src == '\\' && *(src+1) == 'x') {
			match = sscanf(src + 2, "%2x", &ch);
			if (!match)
				pabort("malformed input string");

			src += 4;
			*dst++ = (unsigned char)ch;
		} else {
			*dst++ = *src++;
		}
		ret++;
	}
	return ret;
}

static void hex_dump(const void *src, size_t length, size_t line_size,
		     char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" |");
			while (line < address) {
				c = *line++;
				printf("%c", (c < 32 || c > 126) ? '.' : c);
			}
			printf("|\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	hex_dump(tx, len, 32, "TX");
}

void LCD_WrCmd(unsigned char cmd)
{
	int ret = 0;
	int fd;

	uint8_t tx[] = {cmd};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	gpio_SetValue(GPIO_DCX, GPIO_VALUE_LOW);
	gpio_SetValue(GPIO_CS0, GPIO_VALUE_LOW);
	
	fd = open(device, O_RDWR);

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");


	transfer(fd, tx, rx, sizeof(tx));
	gpio_SetValue(GPIO_DCX, GPIO_VALUE_HIGH);
	gpio_SetValue(GPIO_CS0, GPIO_VALUE_HIGH);
}

void LCD_WrDat(uint8_t dat)
{
	int ret = 0;
	int fd;

	uint8_t tx[] = {dat};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	gpio_SetValue(GPIO_CS0, GPIO_VALUE_LOW);

	fd = open(device, O_RDWR);

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");


	transfer(fd, tx, rx, sizeof(tx));
	gpio_SetValue(GPIO_CS0, GPIO_VALUE_HIGH);
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
	gpio_SetValue(GPIO_CS0, GPIO_VALUE_HIGH);
	gpio_SetValue(GPIO_RESX, GPIO_VALUE_LOW);
	usleep( 50000 );   //  50ms
	gpio_SetValue(GPIO_RESX, GPIO_VALUE_HIGH);
	usleep( 200000 );   //  200ms
	gpio_SetValue(GPIO_DCX, GPIO_VALUE_HIGH);

	LCD_SetCmd1(0x35, 0x00);
	LCD_SetCmd1(0x38, 0x00);

	//LCD_SetCmd1(0x39, 0x1F);
	//LCD_SetCmd1(0x39, 0x00);
	LCD_SetCmd1(0x39, 0x0F);

	LCD_SetCmd1(0x36, 0x08);
	LCD_SetCmd1(0x3A, 0x01);
	LCD_SetCmd2(0x2A, 0x00, 0x85);
	LCD_SetCmd4(0x2B, 0x00, 0x00, 0x04, 0xAF);

	LCD_SetCmd3(0xFF,0x21,0x71,0x45);
	LCD_SetCmd1(0x00,0x04);
	LCD_SetCmd1(0x01,0xB0);
	LCD_SetCmd1(0x07,0x10);
	LCD_SetCmd1(0x32,0x2C);
	LCD_SetCmd1(0x33,0x57);
	LCD_SetCmd1(0x34,0x00);
	LCD_SetCmd1(0x35,0x2B);
	LCD_SetCmd1(0x36,0x58);
	LCD_SetCmd1(0x37,0x85);
	LCD_SetCmd1(0x42,0x10);
	LCD_SetCmd1(0x43,0x3b);
	LCD_SetCmd1(0x44,0x10);
	LCD_SetCmd1(0x45,0x3b);
	LCD_SetCmd1(0x46,0x00);
	LCD_SetCmd1(0x47,0x2D);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd3(0xFF,0x21,0x71,0x11);
	LCD_SetCmd1(0x02,0xB2);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd1(0xF0,0x01);

	LCD_SetCmd3(0xFF,0x21,0x71,0x28);
	LCD_SetCmd1(0x0C,0x04);
	LCD_SetCmd1(0x0D,0x74);
	LCD_SetCmd1(0x0E,0x04);
	LCD_SetCmd1(0x0F,0xAF);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd1(0xF0,0x02);

	LCD_SetCmd3(0xFF,0x21,0x71,0x28);
	LCD_SetCmd1(0x01,0x80);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd1(0xF0,0x03);

	LCD_SetCmd3(0xFF,0x21,0x71,0x28);
	LCD_SetCmd1(0x01,0x40);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd1(0xF0,0x00);


	LCD_SetCmd3(0xFF,0x21,0x71,0x12);
	LCD_SetCmd1(0x87,0xA9);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd3(0xFF,0x21,0x71,0xA0);
	LCD_SetCmd1(0x04,0x17);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd3(0xFF,0x21,0x71,0x2D);
	LCD_SetCmd1(0x00,0x01);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd3(0xFF,0x21,0x71,0x43);
	LCD_SetCmd1(0x03,0x04);
	LCD_SetCmd3(0xFF,0x21,0x71,0x00);

	LCD_SetCmd3(0xFF,0x21,0x71,0x11);
	LCD_SetCmd1(0x0A,0x00);
	LCD_SetCmd1(0x0B,0x18);
	LCD_SetCmd1(0x0C,0x18);

	LCD_SetCmd3(0xFF,0x21,0x71,0x00);
}

void main(int argc, char **argv)
{
	unsigned char i=0;    
	unsigned char rgb[3] = {0x00, 0x00, 0xFF};
	unsigned char *buf;
	FILE *fp;
	unsigned char str[30];
	//mode |= SPI_CS_HIGH;
	init_gpio();

	LCD_Init();
	usleep( 250000 );   //  250ms


	unexport_gpio();

}