#ifndef USB_HID_H_
#define USB_HID_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed))
{
    uint32_t buttons;
    uint8_t padding;
} blue_hid_report_t;

void usb_device_init(void);
void usb_hid_set_buttons(uint32_t buttons);
uint32_t usb_hid_get_buttons(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_HID_H_ */
