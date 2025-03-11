#include <Arduino.h>
#include <joybridge_receiver.h>

// Create an instance of the Receiver.
JoyBridge::JoyBridgeReceiver receiver;

void setup()
{
  Serial.begin(115200);
  // Initialize the receiver. If initialization fails, print an error and halt.
  if (!receiver.begin("JoystickService"))
  {
    Serial.println("Receiver failed to initialize!");
    while (true)
    {
      delay(1000);
    }
  }
}

void loop()
{
  // Process ESP-NOW events and timeouts.
  receiver.loop();

  // For demonstration, if a master is connected, print the latest joystick
  // data.
  if (receiver.isConnected())
  {
    JoyBridge::JoystickData data = receiver.getJoystickData();
    Serial.println("Joystick Data Received:");
    Serial.printf(
        "A:%d B:%d X:%d Y:%d LB:%d RB:%d DU:%d DD:%d DL:%d DR:%d START:%d "
        "BACK:%d\n",
        data.a, data.b, data.x, data.y, data.lb, data.rb, data.dpad_up,
        data.dpad_down, data.dpad_left, data.dpad_right, data.start, data.back);
    Serial.printf("LT:%.2f RT:%.2f LX:%.2f LY:%.2f RX:%.2f RY:%.2f\n", data.lt,
                  data.rt, data.left_x, data.left_y, data.right_x,
                  data.right_y);
  }
  else
  {
    Serial.println("Master not connected.");
  }

  delay(100);
}
