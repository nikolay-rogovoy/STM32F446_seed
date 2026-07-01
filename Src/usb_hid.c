#include <string.h>
#include "stm32f4xx_hal.h"
#include "tusb.h"
#include "usb_hid.h"

static volatile blue_hid_report_t current_hid_report = {
    .buttons = 0,
    .padding = 0,
};

void usb_device_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};

    /* PA11 = D-,  PA12 = D+  (AF10 OTG_FS) */
    gpio.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* PA9 = VBUS (sense input) */
    gpio.Pin  = GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* PA10 = OTG_ID (AF10 OTG_FS, open-drain) */
    gpio.Pin       = GPIO_PIN_10;
    gpio.Mode      = GPIO_MODE_AF_OD;
    gpio.Pull      = GPIO_PULLUP;
    gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &gpio);

    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Priority must be <= configMAX_SYSCALL_INTERRUPT_PRIORITY (0x50 >> 4 = 5).
       NVIC_EnableIRQ is handled internally by the TinyUSB DWC2 driver. */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);

    tusb_rhport_init_t dev_init = {
        .role  = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_FULL,
    };
    tusb_init(0, &dev_init);
}

void usb_hid_set_buttons(uint32_t buttons)
{
    current_hid_report.buttons = buttons;
    current_hid_report.padding = 0;
}

uint32_t usb_hid_get_buttons(void)
{
    return current_hid_report.buttons;
}

uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t *buffer,
                               uint16_t reqlen)
{
    (void)instance;
    (void)report_id;

    if (report_type != HID_REPORT_TYPE_INPUT || buffer == NULL)
    {
        return 0;
    }

    uint16_t const len = reqlen < sizeof(current_hid_report) ? reqlen : sizeof(current_hid_report);
    memcpy(buffer, (const void *)&current_hid_report, len);
    return len;
}

void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer,
                           uint16_t bufsize)
{
    (void)instance;
    (void)report_id;

    if (report_type != HID_REPORT_TYPE_OUTPUT || buffer == NULL || bufsize == 0)
    {
        return;
    }

    // В данной реализации контроллера выходные отчёты не используются.
    // Если понадобится поддержка светодиодов или других эффектов,
    // обработку можно добавить здесь.
}
