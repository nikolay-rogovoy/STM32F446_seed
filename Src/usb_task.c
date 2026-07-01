#include "usb_task.h"
#include "tusb.h"

void USBTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        tud_task();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
