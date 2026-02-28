#pragma once

#define CFG_TUSB_MCU             OPT_MCU_RP2040
#define CFG_TUSB_RHPORT0_MODE    OPT_MODE_DEVICE

#define CFG_TUD_CDC              0
#define CFG_TUD_HID              1
#define CFG_TUD_MSC              0

#define CFG_TUD_ENDPOINT0_SIZE   64
