#include <string.h>
#include "tusb.h"
#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// USB Device Descriptor
//--------------------------------------------------------------------+

#define USB_VID 0xCafe
#define USB_PID 0x4002
#define USB_BCD 0x0200

static const tusb_desc_device_t desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = USB_BCD,

        .bDeviceClass = 0x00,
        .bDeviceSubClass = 0x00,
        .bDeviceProtocol = 0x00,

        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = USB_VID,
        .idProduct = USB_PID,
        .bcdDevice = 0x0100,

        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,

        .bNumConfigurations = 0x01};

uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

// В этом дескрипторе описывается один входной HID-репорт для игрового контроллера.
// Он содержит 32 кнопки по 1 биту каждая, после чего добавляется один байт заполнения
// (HID_CONSTANT), чтобы размер репорта выровнять до целого байта и избежать
// неоднозначностей при чтении на хосте.
uint8_t const desc_hid_report[] =
    {
        // Общая область использования: Desktop Controls
        HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),
        // Устройство как игровой контроллер (Gamepad)
        HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),
        HID_COLLECTION(HID_COLLECTION_APPLICATION),

        // Теперь описываем кнопки
        HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),
        // Кнопки нумеруются от 1 до 32
        HID_USAGE_MIN(1),
        HID_USAGE_MAX(32),
        // Логические значения кнопки: 0 = не нажата, 1 = нажата
        HID_LOGICAL_MIN(0),
        HID_LOGICAL_MAX(1),
        // 32 полей, по 1 биту на каждую кнопку
        HID_REPORT_COUNT(32),
        HID_REPORT_SIZE(1),
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

        // Дополнительный байт заполнения (padding).
        // Он не содержит полезных данных и объявляется как HID_CONSTANT.
        // Без этого байта в дескрипторе был бы неполный байт, хотя 32 бита это ровно 4 байта.
        // Этот padding оставлен здесь для явной структуры и возможного расширения.
        HID_REPORT_COUNT(1),
        HID_REPORT_SIZE(8),
        HID_INPUT(HID_CONSTANT),

        HID_COLLECTION_END};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
    ITF_NUM_HID = 0,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID_IN 0x81
#define HID_POLL_INTERVAL 2

uint8_t const desc_configuration[] =
    {
        TUD_CONFIG_DESCRIPTOR(
            1,
            ITF_NUM_TOTAL,
            0,
            CONFIG_TOTAL_LEN,
            TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
            100),

        TUD_HID_DESCRIPTOR(
            ITF_NUM_HID,
            0,
            HID_ITF_PROTOCOL_NONE,
            sizeof(desc_hid_report),
            EPNUM_HID_IN,
            CFG_TUD_HID_EP_BUFSIZE,
            HID_POLL_INTERVAL)};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

enum
{
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};

static const char *string_desc_arr[] =
    {
        (const char[]){0x09, 0x04},
        "Nikolay Rogovoy",
        "F446 Gamepad",
        // TODO: заменить на серийный номер, сгенерированный из уникального ID чипа (UID).
        // STM32F4: три 32-битных слова по адресу 0x1FFF7A10 (96 бит).
        // tud_descriptor_string_cb должна формировать строку динамически через HAL_GetUIDw0/1/2()
        // и возвращать её напрямую, минуя string_desc_arr.
        "0001",
};

static uint16_t _desc_str[32];

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;

    uint8_t chr_count;

    if (index == STRID_LANGID)
    {
        _desc_str[1] = 0x0409;
        chr_count = 1;
    }
    else
    {
        if (index >= (sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
        {
            return NULL;
        }

        const char *str = string_desc_arr[index];
        chr_count = (uint8_t)strlen(str);

        if (chr_count > 31)
        {
            chr_count = 31;
        }

        for (uint8_t i = 0; i < chr_count; i++)
        {
            _desc_str[1 + i] = str[i];
        }
    }

    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
    return _desc_str;
}