/**
******************************************************************************
* File Name          : gpio_dev.c
* Description        : Algorithm that allows the camera to determine whether to shoot.
******************************************************************************
*
* COPYRIGHT(c) 2021 Joey Ke
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "gpio_dev.h"

GPIO_Error gpio_Export(unsigned int gpio_num)
{
    int fd, len;  
    char buf[MAX_BUF];  
   
    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);  
    if (fd < 0) {  
        perror("gpio/export");  
        abort();
        return fd;  
    }  
   
    len = snprintf(buf, sizeof(buf), "%d", gpio_num);  
    write(fd, buf, len);  
    close(fd);  
    printf("pin:\t%3d exported\n", gpio_num); 
    return 0;  
}

GPIO_Error gpio_Unexport(unsigned int gpio_num)
{  
    int fd, len;  
    char buf[MAX_BUF];  
   
    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);  
    if (fd < 0) {  
        perror("gpio/export");  
        return fd;  
    }  
   
    len = snprintf(buf, sizeof(buf), "%d", gpio_num);  
    write(fd, buf, len);  
    close(fd);
    printf("pin:\t%3d unexported\n", gpio_num); 
    return 0;  
} 

GPIO_Error gpio_SetDirection(unsigned int gpio_num, GPIO_Direction out_flag)
{
    int fd, len;  
    char buf[MAX_BUF];  
   
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio_num);  
   
    fd = open(buf, O_WRONLY);  
    if (fd < 0) {  
        perror("gpio/direction");  
        return fd;  
    }  
    
    out_flag ? write(fd, "out", 4) : write(fd, "in", 3); 
   
    close(fd);  
    return 0;  
}

GPIO_Error gpio_SetValue(unsigned int gpio_num, GPIO_Value value)
{  
    int fd, len;  
    char buf[MAX_BUF];  
   
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio_num);  
   
    fd = open(buf, O_WRONLY);  
    if (fd < 0) {  
        perror("gpio/set-value");  
        return fd;  
    }  
   
    value ?   write(fd, "1", 2) : write(fd, "0", 2);
   
    close(fd);  
    return 0;  
}  

int gpio_OpenFD(unsigned int gpio_num)
{
    int fd, len;  
    char buf[MAX_BUF];  
  
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio_num);  
   
    fd = open(buf, O_RDONLY | O_NONBLOCK );  
    if (fd < 0) {  
        perror("gpio/fd_open");  
    }  
    return fd;  
}

GPIO_Error gpio_CloseFD(int fd)
{  
    return close(fd);  
}  

/************************ (C) COPYRIGHT Joey Ke *****END OF FILE****/