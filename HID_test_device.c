#include <stdio.h>
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"

const uint row_pins[4] = {10, 15, 14, 12};
const uint col_pins[3] = {11, 9, 13};

const uint8_t keymap[4][3] = {
    {HID_KEY_1, HID_KEY_1, HID_KEY_3},
    {HID_KEY_4, HID_KEY_5, HID_KEY_6},
    {HID_KEY_7, HID_KEY_8, HID_KEY_9},
    {HID_KEY_KEYPAD_MULTIPLY, HID_KEY_0, HID_KEY_ENTER}
};

void matrix_init(void) {
    for (int r = 0; r < 4; r++) {
        gpio_init(row_pins[r]);
        gpio_set_dir(row_pins[r], false);
        gpio_pull_up(row_pins[r]);
    }
    for (int c = 0; c < 3; c++) {
        gpio_init(col_pins[c]);
        gpio_set_dir(col_pins[c], false);
        gpio_pull_up(col_pins[c]);
    }
}

void matrix_scan(uint8_t *keycode) {
    bool pressed[4][3] = {0};
    int total_keys = 0;

    for (int c = 0; c < 3; c++) {
        gpio_set_dir(col_pins[c], true); 
        gpio_put(col_pins[c], 0); 
        sleep_us(5);                     

        for (int r = 0; r < 4; r++) {
            if (!gpio_get(row_pins[r])) {
                pressed[r][c] = true;
                total_keys++;
            }
        }

        gpio_set_dir(col_pins[c], false);
        gpio_pull_up(col_pins[c]);
    }
    bool rollover = false;

    int row_count[4] = {0};
    int col_count[3] = {0};

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 3; c++) {
            if (pressed[r][c]) {
                row_count[r]++;
                col_count[c]++;
            }
        }
    }

    for (int r1 = 0; r1 < 4; r1++) {
        if (row_count[r1] < 2) continue;
        for (int r2 = r1 + 1; r2 < 4; r2++) {
            if (row_count[r2] < 1) continue;
            for (int c1 = 0; c1 < 3; c1++) {
                if (!pressed[r1][c1] && !pressed[r2][c1]) continue;
                for (int c2 = 0; c2 < 3; c2++) {
                    if (c2 == c1) continue;
                    if (!pressed[r1][c2] && !pressed[r2][c2]) continue;
                    rollover = true;
                    goto done;
                }
            }
        }
    }

    done:

    if (rollover || total_keys > 6) {
        for (int i = 0; i < 6; i++) keycode[i] = 1;
        return;
    }

    int k = 0;
    for (int r = 0; r < 4 && k < 6; r++) {
        for (int c = 0; c < 3 && k < 6; c++) {
            if (pressed[r][c]) keycode[k++] = keymap[r][c];
        }
    }
    for (; k < 6; k++) keycode[k] = 0;
}

int main(void) {
    board_init();
    tusb_init();

    matrix_init();

    while (1) {
        tud_task();
        if (tud_hid_ready()) {
            uint8_t report[8]={0};
            uint8_t* keycodes = &report[2];
            matrix_scan(keycodes);
            tud_hid_report(0,report,8);
        }
    }
}


void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer,
                           uint16_t bufsize)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}

uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer,
                               uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}