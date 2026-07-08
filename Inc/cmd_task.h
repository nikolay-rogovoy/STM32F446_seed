#ifndef CMD_TASK_H_
#define CMD_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

/* Размер кольцевого буфера приёма (степень двойки!) */
#define CMD_RX_RING_SIZE    1024
/* Максимальная длина одной команды, включая '\n' */
#define CMD_MAX_LINE_LEN    128

void cmd_task_init(TaskHandle_t xTaskHandle);
void CmdTask(void *pvParameters);

#endif /* CMD_TASK_H_ */
