#ifndef LED_TASK_H_
#define LED_TASK_H_

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN                     GPIO_PIN_5
#define LED_PORT                    GPIOA

#define LED_TOGGLE_INTERVAL_MS      1000

void vLEDTask(void *pvParameters);

#endif /* LED_TASK_H_ */
