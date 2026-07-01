#include "led_task.h"
#include "usb_report_task.h"

#include "FreeRTOS.h"
#include "task.h"

void vLEDTask(void *pvParameters)
{
    (void)pvParameters;
    uint32_t button_mask = 0x00000001u;

    for (;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        usb_report_notify(button_mask);

        button_mask <<= 1;
        if (button_mask == 0u)
        {
            button_mask = 0x00000001u;
        }

        vTaskDelay(pdMS_TO_TICKS(LED_TOGGLE_INTERVAL_MS));
    }
}
