from evdev import InputDevice, ecodes, list_devices
import select

class KeyboardManager:
    def __init__(self):
        self.devices = []
        self.device_paths = set()
        self.update_devices()

    def update_devices(self):
        current_paths = set(list_devices())
        # Only rebuild if devices changed
        if current_paths != self.device_paths:
            self.devices = []
            for path in current_paths:
                dev = InputDevice(path)
                caps = dev.capabilities()
                # Filter keyboards (has EV_KEY and KEY_A)
                if ecodes.EV_KEY in caps and ecodes.KEY_A in caps[ecodes.EV_KEY]:
                    self.devices.append(dev)
            self.device_paths = current_paths

    def get_inputs(self):
        # Update devices if something changed
        self.update_devices()
        inputs = []
        if not self.devices:
            return inputs

        r, _, _ = select.select(self.devices, [], [], 0)
        for dev in r:
            for event in dev.read():
                if event.type == ecodes.EV_KEY and event.value == 1:  # Key down
                    inputs.append((dev.name, ecodes.KEY[event.code]))
        return inputs

# Usage example
kb_manager = KeyboardManager()

while True:
    pressed_keys = kb_manager.get_inputs()
    for device_name, key in pressed_keys:
        print(f"[{device_name}] {key}")
