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
#include <stdint.h>
#include <unistd.h>
#include <linux/spi/spidev.h>

/**
 * @struct  Radar_ObjectSpeedData_t
 * @brief   One Speed measurement data for each target.
 *
 */
typedef struct spidev_t
{
  uint32_t mode;
  uint8_t bits;
  uint32_t speed;
} spidev_t;

void spidev_init(spidev_t *spidev);
void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len);

#endif  /* SPI_8MM_DRIVER_H */
/************************ (C) COPYRIGHT Joey Ke *****END OF FILE****/