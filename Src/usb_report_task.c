#include "usb_report_task.h"
#include "usb_hid.h"
#include "tusb.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t xUSBReportSemaphore = NULL;

void usb_report_init(void)
{
    xUSBReportSemaphore = xSemaphoreCreateBinary();
}

void usb_report_notify(uint32_t buttons)
{
    if (xUSBReportSemaphore == NULL)
    {
        return;
    }

    usb_hid_set_buttons(buttons);
    xSemaphoreGive(xUSBReportSemaphore);
}

void USBReportTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        if (xUSBReportSemaphore != NULL && xSemaphoreTake(xUSBReportSemaphore, portMAX_DELAY) == pdTRUE)
        {
            blue_hid_report_t report = {
                .buttons = usb_hid_get_buttons(),
                .padding = 0,
            };

            if (tud_hid_ready())
            {
                tud_hid_report(0, &report, sizeof(report));
            }
        }
    }
}
