/**
  ******************************************************************************
  * File Name          : gpio_dev.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2021 Joey Ke
  *
  ******************************************************************************
  */
 /* Define to prevent recursive inclusion -------------------------------------*/
 #ifndef GPIO_DEV_H
 #define GPIO_DEV_H
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <fcntl.h>  
#include <stdint.h>
#include <stdlib.h> 
#include <unistd.h> 

#define MAX_BUF 64
#define SYSFS_GPIO_DIR "/sys/class/gpio"  

#define   INT8       int8_t
#define   INT16      int16_t
#define   INT32      int32_t

/** @defgroup GPIO_define_Error_group Error and Warning code returned by API
 *  The following DEFINE are used to identify the PAL ERROR
 *  @{
 */
typedef int8_t GPIO_Error;

#define GPIO_ERROR_NONE                        ((GPIO_ERROR)  0)
#define GPIO_ERROR_INVALID_STATU               ((GPIO_ERROR) - 1)       /*!< Warning invalid status*/
/** @} GPIO_define_Error_group */

/** @defgroup GPIO_Direction_group Defines Prediction Status
 *  Defines all possible prediction status mode
 *  @{
 */
typedef uint8_t GPIO_Direction;

#define GPIO_DIRECTION_IN        ((GPIO_Direction)  0)
#define GPIO_DIRECTION_OUT       ((GPIO_Direction)  1)
/** @} GPIO_Direction_group */

/** @defgroup GPIO_Value_group Defines Prediction Status
 *  Defines all possible prediction status mode
 *  @{
 */
typedef uint8_t GPIO_Value;

#define GPIO_VALUE_LOW        ((GPIO_Value)  0)
#define GPIO_VALUE_HIGH       ((GPIO_Value)  1)
/** @} GPIO_Value_group */

/**
 * @brief Export GPIO
 * 
 * @param gpio_num                number of GPIO to export
 * @return GPIO_Error 
 */
GPIO_Error gpio_Export(unsigned int gpio_num);

/**
 * @brief Unexport GPIO
 * 
 * @param gpio_num                number of GPIO to unexport
 * @return GPIO_Error 
 */
GPIO_Error gpio_Unexport(unsigned int gpio_num);

/**
 * @brief Set GPIO direction
 * 
 * @param gpio_num                number of GPIO to set direction
 * @param out_flag                direction of GPIO
 * @return GPIO_Error 
 */
GPIO_Error gpio_SetDirection(unsigned int gpio_num, GPIO_Direction out_flag);

/**
 * @brief Set GPIO potential 
 * 
 * @param gpio_num                number of GPIO to set potential
 * @param value                   potential od GPIO
 * @return GPIO_Error 
 */
GPIO_Error gpio_SetValue(unsigned int gpio_num, GPIO_Value value);

/**
 * @brief Open fd
 * 
 * @param gpio_num                number of GPIO to set potential
 * @return fd
 */
int gpio_OpenFD(unsigned int gpio_num);

/**
 * @brief Close fd
 * 
 * @param fd                fd
 * @return GPIO_Error 
 */
GPIO_Error gpio_CloseFD(int fd);


 #endif  /* GPIO_DEV_H */
/************************ (C) COPYRIGHT Joey Ke *****END OF FILE****/