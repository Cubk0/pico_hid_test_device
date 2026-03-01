#include <stdint.h>
#include "tusb.h"
// HID report descriptor (keyboard)
uint8_t const desc_hid_report[] = {
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    ,
    /* 8 bits Modifier Keys (Shift, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,
      HID_USAGE_MIN    ( 224                                    )  ,
      HID_USAGE_MAX    ( 231                                    )  ,
      HID_LOGICAL_MIN  ( 0                                      )  ,
      HID_LOGICAL_MAX  ( 1                                      )  ,
      HID_REPORT_COUNT ( 8                                      )  ,
      HID_REPORT_SIZE  ( 1                                      )  ,
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,
      /* 8 bit reserved */ \
      HID_REPORT_COUNT ( 1                                      )  ,
      HID_REPORT_SIZE  ( 8                                      )  ,
      HID_INPUT        ( HID_CONSTANT                           )  ,
    /* Output 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   )       ,
      HID_USAGE_MIN    ( 1                                       ) ,
      HID_USAGE_MAX    ( 5                                       ) ,
      HID_REPORT_COUNT ( 5                                       ) ,
      HID_REPORT_SIZE  ( 1                                       ) ,
      HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,
      /* led padding */ \
      HID_REPORT_COUNT ( 1                                       ) ,
      HID_REPORT_SIZE  ( 3                                       ) ,
      HID_OUTPUT       ( HID_CONSTANT                            ) ,
    /* 6-byte Keycodes */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,
      HID_USAGE_MIN    ( 0                                   )     ,
      HID_USAGE_MAX_N  ( 255, 2                              )     ,
      HID_LOGICAL_MIN  ( 0                                   )     ,
      HID_LOGICAL_MAX_N( 255, 2                              )     ,
      HID_REPORT_COUNT ( 6                                   )     ,
      HID_REPORT_SIZE  ( 8                                   )     ,
      HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )     ,
  HID_COLLECTION_END 
};

uint8_t const desc_device[] = {
    0x12,                       // bLength
    TUSB_DESC_DEVICE,            // bDescriptorType
    0x00, 0x02,                 // bcdUSB (2.00)
    0x00,                       // bDeviceClass
    0x00,                       // bDeviceSubClass
    0x00,                       // bDeviceProtocol
    CFG_TUD_ENDPOINT0_SIZE,     // bMaxPacketSize0
    0xCA, 0xFE,                 // idVendor
    0x00, 0x40,                 // idProduct
    0x01, 0x00,                 // bcdDevice
    0x01,                       // iManufacturer
    0x02,                       // iProduct
    0x03,                       // iSerialNumber
    0x01                        // bNumConfigurations
};

uint8_t const* tud_descriptor_device_cb(void)
{
    return desc_device;
}

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) { return desc_hid_report; }

uint8_t const desc_configuration[] = {
    0x09,                           // bLength
    TUSB_DESC_CONFIGURATION,        // bDescriptorType
    0x09 + TUD_HID_DESC_LEN, 0x00,  // wTotalLength (configuration + HID descriptor)
    0x01,                           // bNumInterfaces
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration
    0x80,                           // bmAttributes: bus powered
    50,                             // bMaxPower: 100 mA
    TUD_HID_DESCRIPTOR(
        0,                          // interface number
        0,                          // string index
        HID_ITF_PROTOCOL_NONE,      // no boot protocol
        sizeof(desc_hid_report),    // report descriptor length
        0x81,                       // IN endpoint 1 (0x80 | 1)
        8,                          // endpoint size
        10                          // polling interval 10ms
    )
};

uint8_t const* tud_descriptor_configuration_cb(uint8_t index)
{
    (void) index;
    return desc_configuration;
}

char const* string_desc_arr[] = {
    (const char[]) {0},     // 0: reserved
    "IPZ",                  // 1: Manufacturer
    "Matrix Keyboard",      // 2: Product
    "123456",               // 3: Serial
};

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    static uint16_t _desc_str[32];
    static uint16_t langid_desc[1] = {0x0409}; // English language ID
    uint8_t chr_count;

    (void) langid;

    // Index 0: return language ID
    if (index == 0) return langid_desc;

    // Out of bounds
    if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) return NULL;

    const char* str = string_desc_arr[index];

    // Convert ASCII string to UTF-16
    for (chr_count = 0; str[chr_count] && chr_count < 31; chr_count++) {
        _desc_str[1 + chr_count] = str[chr_count];
    }

    // First byte is descriptor header: length + type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2*chr_count + 2);

    return _desc_str;
}
