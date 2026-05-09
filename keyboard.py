import select
from evdev import InputDevice, list_devices, ecodes


class KeyboardManager:
    def __init__(self):
        self.devices = []
        self.device_paths = set()
        self.update_devices()

    def update_devices(self):
        current_paths = set(list_devices())
        # Only rebuild if devices changed
        if current_paths != self.device_paths:
            for dev in self.devices:
                try:
                    dev.close()
                except OSError:
                    pass

            self.devices = []
            for path in current_paths:
                try:
                    dev = InputDevice(path)
                    caps = dev.capabilities()

                    if ecodes.EV_KEY in caps:
                        key_codes = set(caps[ecodes.EV_KEY])
                        keyboard_codes = {
                            code for code in key_codes
                            if code < ecodes.BTN_MISC and code != ecodes.KEY_RESERVED
                        }

                        if keyboard_codes:
                            self.devices.append(dev)
                        else:
                            dev.close()
                    else:
                        dev.close()

                except OSError:
                    pass

            self.device_paths = current_paths

    def get_inputs(self):
        # Update devices if something changed
        self.update_devices()
        inputs = []
        if not self.devices:
            return inputs

        try:
            r, _, _ = select.select(self.devices, [], [], 0)
        except OSError:
            self.device_paths = set()
            return inputs

        for dev in r:
            try:
                for event in dev.read():
                    if event.type == ecodes.EV_KEY:  # Key down
                        inputs.append((dev.name, ecodes.KEY[event.code], event.value))
            except OSError:
                self.device_paths = set()

        return inputs


# Usage example
kb_manager = KeyboardManager()

while True:
    pressed_keys = kb_manager.get_inputs()
    # value: 1 for key down, 0 for key up, 2 for key hold
    for device_name, key, value in pressed_keys:
        print(f"[{device_name}] {key} {value}")