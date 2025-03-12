<h1 align="center">🎮 | ESP-NOW Joybridge Receiver| 🎮</h1>

<p align="center">
  Arduino library to receive joystick commands from the ESP-NOW Joybridge
</p>

---

## Table of Contents

- [Table of Contents](#table-of-contents)
- [🎯 Project Objectives](#-project-objectives)
- [Installation](#installation)
- [Example Usage](#example-usage)

---

## 🎯 Project Objectives

JoyBridge provides a simple, efficient, and reliable way to wirelessly send joystick commands to ESP32-based projects, such as MotorGo or other compatible platforms. It emphasizes ease of use, real-time responsiveness, and minimal resource usage.

For full documentation, go to the [main JoyBridge repository](https://github.com/Every-Flavor-Robotics/esp-now-joybridge).

## Installation

To use the receiver, simple add the  JoyBridge Receiver Library to your ESP32 project by updating your `platformio.ini`:

```ini
lib_deps =
    https://github.com/Every-Flavor-Robotics/esp-now-joybridge-receiver.git
```

## Example Usage

Here's a minimal example demonstrating the JoyBridge Receiver usage:

```cpp
#include <Arduino.h>
#include <joybridge_receiver.h>

JoyBridge::JoyBridgeReceiver receiver;

void setup()
{
  Serial.begin(115200);

  if (!receiver.begin("MyJoystickService"))
  {
    Serial.println("Receiver initialization failed!");
    while (true)
      delay(1000);
  }
}

void loop()
{
  receiver.loop();

  if (receiver.isConnected())
  {
    JoyBridge::JoystickData data = receiver.getJoystickData();
    Serial.println("Joystick Data Received:");
    Serial.printf(
        "A:%d B:%d X:%d Y:%d LB:%d RB:%d DU:%d DD:%d DL:%d DR:%d START:%d BACK:%d\n",
        data.a, data.b, data.x, data.y, data.lb, data.rb, data.dpad_up,
        data.dpad_down, data.dpad_left, data.dpad_right, data.start, data.back);
    Serial.printf("LT:%.2f RT:%.2f LX:%.2f LY:%.2f RX:%.2f RY:%.2f\n",
                  data.lt, data.rt, data.left_x, data.left_y, data.right_x, data.right_y);
  }
  else
  {
    Serial.println("Master not connected.");
  }

  delay(100);
}
```
