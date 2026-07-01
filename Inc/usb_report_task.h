#ifndef USB_REPORT_TASK_H_
#define USB_REPORT_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

void usb_report_init(void);
void usb_report_notify(uint32_t buttons);
void USBReportTask(void *pvParameters);

#endif /* USB_REPORT_TASK_H_ */
