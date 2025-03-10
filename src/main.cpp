#include <WiFi.h>
#include <esp_now.h>

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

JoystickData joystick;

JoystickData receivedData;

// Callback executed when data is received
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData,
                int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  //   Print all joystick data, 1 line
  Serial.printf(
      "A: %d, B: %d, X: %d, Y: %d, LB: %d, RB: %d, Dpad Up: %d, Dpad Down: %d, "
      "Dpad Left: %d, Dpad Right: %d, Start: %d, Back: %d, LT: %f, RT: %f, "
      "Left X: %f, Left Y: %f, Right X: %f, Right Y: %f\n",
      receivedData.a, receivedData.b, receivedData.x, receivedData.y,
      receivedData.lb, receivedData.rb, receivedData.dpad_up,
      receivedData.dpad_down, receivedData.dpad_left, receivedData.dpad_right,
      receivedData.start, receivedData.back, receivedData.lt, receivedData.rt,
      receivedData.left_x, receivedData.left_y, receivedData.right_x,
      receivedData.right_y);
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {}
