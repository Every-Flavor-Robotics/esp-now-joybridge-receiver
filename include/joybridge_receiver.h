
#ifndef JOYBRIDGE_RECEIVER_H
#define JOYBRIDGE_RECEIVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

namespace JoyBridge
{
struct JoystickData
{
  bool a;
  bool b;
  bool x;
  bool y;
  bool lb;
  bool rb;
  bool dpad_up;
  bool dpad_down;
  bool dpad_left;
  bool dpad_right;
  bool start;
  bool back;
  float lt;
  float rt;
  float left_x;
  float left_y;
  float right_x;
  float right_y;
};

struct ServiceAnnouncement
{
  char service_name[16];
};

struct ClientRegistration
{
  char message[16];  // "REGISTER"
};

class JoyBridgeReceiver
{
 public:
  // Constructor.
  JoyBridgeReceiver();

  // Initializes WiFi and ESP-NOW and registers callbacks.
  // Returns true on success, false otherwise.
  bool begin(String service_name);

  // Must be called periodically from the main loop to process ESP-NOW events
  // and check for timeouts.
  void loop();

  // Returns the most recent joystick data received.
  JoystickData getJoystickData() const;

  // Returns whether a master (service) is currently connected.
  bool isConnected() const;

  // Type definition for a callback that gets invoked whenever new joystick
  // data is received.
  typedef void (*DataCallback)(const JoystickData &data);

  // Set a user-provided callback function that is called when new data arrives.
  void setDataCallback(DataCallback callback);

 private:
  char _service_name[16];

  // Latest joystick data.
  JoystickData _latestData;

  // Flag indicating if a master has been registered.
  bool _connected;

  // Timestamp of the last received message (used for timeout checking).
  unsigned long _lastReceivedTime;

  // User-provided callback pointer.
  DataCallback _dataCallback;

  // Static instance pointer for forwarding static callbacks.
  static JoyBridgeReceiver *_instance;

  // Static ESP-NOW data receive callback. This function forwards the event
  // to the instance's handleDataRecv() method.
  static void onDataRecv(const esp_now_recv_info_t *info,
                         const uint8_t *incomingData, int len);

  // Actual member function to process incoming ESP-NOW data.
  void handleDataRecv(const esp_now_recv_info_t *info,
                      const uint8_t *incomingData, int len);

  // Check for timeout conditions (e.g. if no message has been received within
  // a given period, mark as disconnected).
  void checkTimeout();

  // Additional helper methods (e.g., peer registration) can be declared here.
};

}  // namespace JoyBridge

#endif  // JOYBRIDGE_RECEIVER_H