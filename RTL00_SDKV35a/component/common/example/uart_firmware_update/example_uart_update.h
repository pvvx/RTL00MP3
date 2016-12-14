#ifndef UART_UPDATE_H
#define UART_UPDATE_H

#include "PinNames.h"
#include "gpio_api.h"
#include "hal_gpio.h"
#include "osdep_api.h"

#define PIN_NAME		PC_2
#define MAX_WAIT_TIME	100

void example_uart_update();
int is_update_image_enable(gpio_t *gpio_uart_update_eable);
extern int uart_ymodem(void);

#endif

