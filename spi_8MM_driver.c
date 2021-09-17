/**
******************************************************************************
* File Name          : spi_8MM_driver.c
* Description        : Algorithm that allows the camera to determine whether to shoot.
******************************************************************************
*
* COPYRIGHT(c) 2021 Joey Ke
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "spi_8MM_driver.h"

void spidev_init()
{
	spi_dev.device = SPI_DEVICE;
	spi_dev.bits = SPI_BITS_PER_WORD;
	spi_dev.speed = SPI_MAX_SPEED_HZ;
	spi_dev.cs_change = SPI_CS_CHANGE;
}

void pabort(const char *s)
{
	perror(s);
	abort();
}

void hex_dump(const void *src, size_t length, size_t line_size,
			  char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0)
	{
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size))
		{
			if (length == 0)
			{
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" |");
			while (line < address)
			{
				c = *line++;
				printf("%c", (c < 32 || c > 126) ? '.' : c);
			}
			printf("|\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}

void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.speed_hz = spi_dev.speed,
		.bits_per_word = spi_dev.bits,
		.cs_change = spi_dev.cs_change,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
}

void transfer_data(uint8_t data)
{
	int ret = 0;
	int fd;
	uint8_t tx[] = {data};
	uint8_t rx[ARRAY_SIZE(tx)] = { 0,};

	fd = open(spi_dev.device, O_RDWR);

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &spi_dev.mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_dev.bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_dev.speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	transfer(fd, tx, rx, sizeof(tx));
	close(fd);
}

void transfer_pixel(unsigned char *data)
{
	unsigned char buff[SPI_TRANSFER_BYTE] = {0};
	int ret = 0;
	int fd;
	int ct = 0;
	uint8_t rx[ARRAY_SIZE(buff)] = { 0,	};

	fd = open(spi_dev.device, O_RDWR);

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &spi_dev.mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_dev.bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_dev.speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	// while(data[ct * SPI_TRANSFER_BYTE] != '\n')
	// {
	// 	strncpy(buff, data+ct * SPI_TRANSFER_BYTE, SPI_TRANSFER_BYTE);
	// 	transfer(fd, buff, rx, sizeof(buff));
	// 	ct++;
	// }

	for(int i=0; i<482400/SPI_TRANSFER_BYTE; i++) {
		for(int j=0; j<SPI_TRANSFER_BYTE;j++) {
			buff[j] = data[i*SPI_TRANSFER_BYTE+j];
		}
		transfer(fd, buff, rx, sizeof(buff));
	}
	
	close(fd);
}

/************************ (C) COPYRIGHT Joey Ke *****END OF FILE****/