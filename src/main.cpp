#include <WiFi.h>
#include <esp_now.h>

typedef struct JoystickData
{
  float x;
  float y;
} JoystickData;

JoystickData receivedData;

// Callback executed when data is received
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData,
                int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.print("X: ");
  Serial.print(receivedData.x);
  Serial.print("\tY: ");
  Serial.println(receivedData.y);
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
