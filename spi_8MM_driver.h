/**
  ******************************************************************************
  * File Name          : spi_8MM_driver.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2021 Joey Ke
  *
  ******************************************************************************
  */
 /* Define to prevent recursive inclusion -------------------------------------*/
 #ifndef SPI_8MM_DRIVER_H
 #define SPI_8MM_DRIVER_H
/* Includes ------------------------------------------------------------------*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

/* Define ------------------------------------------------------------------*/
#define SPI_DEVICE              "/dev/spidev1.0"
#define SPI_BITS_PER_WORD       8
#define SPI_MAX_SPEED_HZ        2000000       // 4 MHz
#define SPI_CS_CHANGE           1

#define SPI_TRANSFER_BYTE       2048          //Byte

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/**
 * @struct  Radar_ObjectSpeedData_t
 * @brief   One Speed measurement data for each target.
 *
 */
typedef struct spidev_t
{
  const char *device;
  uint32_t mode;
  uint8_t bits;
  uint32_t speed;
  uint8_t cs_change;
} spidev_t;

/* Global variables -----------------------------------------------------------*/
spidev_t spi_dev;

void pabort(const char *s);
void hex_dump(const void *src, size_t length, size_t line_size, char *prefix);
void spidev_init();
void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len);
void transfer_data(uint8_t data);
void transfer_pixel(unsigned char *data);

#endif  /* SPI_8MM_DRIVER_H */
/************************ (C) COPYRIGHT Joey Ke *****END OF FILE****/